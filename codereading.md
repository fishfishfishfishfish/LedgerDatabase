客户端启动`distributed/exes/tpccClient.cc`（TPC-C）或`distributed/exes/verifyClient.cc`（YCSB）生成transaction并发给数据库服务器。
服务器端启动`distributed/exes/strong-server.cc`，通过TCP接收transaction并处理。


一个简单的transaction过程，以`distributed/exes/verifyClient.cc`中line 188-209为例
主要步骤是
```C++
// line 430
strongstore::Client *client = new strongstore::Client(strongstore::MODE_OCC, configPath,
    nShards, closestReplica, TrueTime(skew, error));
client->Begin();
client->Get(task.keys[j]);
std::map<int, std::map<uint64_t, std::vector<std::string>>> unverified_keys;
status = client->Commit(unverified_keys);
```

strongstore::Client中会调用bclient. `distributed/store/strongstore/client.cc`
```C++
// line 37
/* Start a bclient for each shard. */
for (int i = 0; i < nShards; i++) {
    string shardConfigPath = configPath + to_string(i) + ".config";
    ShardClient *shardclient = new ShardClient(mode, shardConfigPath,
        &transport, client_id, i, closestReplica);
    bclient[i] = new BufferClient(shardclient);
}

for (int i = 0; i < nshards; i++) {
    bclient[i]->Begin(t_id);
}

/* `i`  is determined by key */
bclient[i]->Get(key, &promise);

status = Prepare(ts);
for (auto& p : participants) {
    bclient[p]->Commit(ts);
}
```


bclient会调用`TxnClient* txnclient`, `Transaction txn`. `distributed/store/common/frontend/bufferclient.cc`

```C++
// Begin
txn = Transaction();
txnclient->Begin(tid); // tid在verifyClient每发起一个transaction时会递增

// Get
txn.addReadSet(key, Timestamp());

// Prepare
txnclient->Prepare(tid, txn, timestamp, promise);
// Commit
txnclient->Commit(tid, txn, history, timestamp, promise);
```

先看看`txnclient->Commit`做了什么
`distributed/store/strongstore/client.cc`中指明bclient所使用的txnclient是shardclient类型。
`distributed/store/strongstore/shardclient.cc`
```C++
replication::vr::VRClient *client;

//prepare
txn.serialize(request.mutable_prepare()->mutable_txn());
request.SerializeToString(&request_str);
client->Invoke(request_str, bind(&ShardClient::PrepareCallback, this, placeholders::_1, placeholders::_2));

// Commit
request.SerializeToString(&request_str);
client->Invoke(request_str, bind(&ShardClient::CommitCallback, this, placeholders::_1, placeholders::_2));
```

看看transaction如何序列化为request
```C++
void
Transaction::serialize(TransactionMessage *msg) const
{
    for (auto& read : readSet) {
        ReadMessage *readMsg = msg->add_readset();
        readMsg->set_key(read.first);
        read.second.serialize(readMsg->mutable_readtime());
    }

    for (auto& write : writeSet) {
        WriteMessage *writeMsg = msg->add_writeset();
        writeMsg->set_key(write.first);
        writeMsg->set_value(write.second);
    }
}
```

到这里知道了，txn通过protobuf将key，value序列化为request string。request string通过replication::vr::VRClient的Invoke发送。
`distributed/replication/vr/client.cc`
```C++
PendingRequest *req = new PendingRequest(request, reqId, continuation, timer);
pendingReqs[reqId] = req; // 要记录发出去的request，到时返回可以取到
SendRequest(req);
```
```C++
// in SendRequest
transport->SendMessageToAll(this, reqMsg, false)
```
in `SendMessageToAll`
```C++
for (auto & kv2 : replicaAddresses[cfg])
    SendMessageInternal(src, kv2.second, m, false)

// or =======
auto kv = multicastAddresses.find(cfg);
SendMessageInternal(src, kv->second, m, true);
```
根据`distributed/store/strongstore/client.h:69`, `transport`的类型是`TCPTransport`

```C++
// TCPTransport::SendMessageInternal
auto kv = tcpOutgoing.find(dst);
struct bufferevent *ev = kv->second;
// buf的layout为：
// | MAGIC  | totalLen | typeLen | type   | dataLen | data   |
// | uint32 | size_t   | size_t  | string | size_t  | string |
// MAGIC用于标识消息的起点，起一个分割作用。type这里为protobuf定义的requestMessage
bufferevent_write(ev, buf, totalLen) < 0
```

现在讨论一下dst地址从哪里来的。
TCPTransport::Register被调用的时候，会记录receiver->config的mapping。
config里有dst地址。

config追根溯源，是从verifyClient的命令行参数`-c`指定的文件中读取来的



注意到TCPTransport需要一个receiver接收信息。用`ReceiveMessage`接收
客户端的`VRClient`是receiver, `distributed/replication/vr/client.cc:106`
服务端的`VRReplica`是receiver, `distributed/replication/vr/replica.cc:302`

`VRClient`被调用`ReceiveMessage`后，会调用之前记录在request结构里的`continuation`函数，例如
- `ShardClient::PrepareCallback`
- `ShardClient::CommitCallback`




# 梳理一下verify的流程
1. `distributed/exes/verifyClient.cc:200`的`txnThread`会在Commit的时候记录`unverified_keys`,并写入全局变量`verifymap`。
```C++
std::map<int, std::map<uint64_t, std::set<std::string>>> verifymap;
std::map<int, std::map<uint64_t, std::vector<std::string>>> unverified_keys;
```
2. `distributed/exes/verifyClient.cc`里启动了`verifyThread`，从`verifymap`里读取需要验证的key。`distributed/exes/verifyClient.cc:143`调用`strongstore::Client->Verify`.
3. `distributed/store/strongstore/client.cc:462/473`调用`BufferClient->Verify`. 会创建并传入一个promise结构体。最后使用promise进行阻塞，等待结果返回。
4. `distributed/store/common/frontend/bufferclient.cc:114`调用`TxnClient->GetProof`也就是`ShardClient::GetProof`, 需要验证的key和promise结构体被传入。
5. `distributed/store/strongstore/shardclient.cc:143`定时启动`replication::vr::VRClient->InvokeUnlogged`, 将proof的请求发送到服务器，结果返回的时候，调用`ShardClient::GetProofCallback`函数。shardClient会记录promise结构，在验证结果返回的时候进行恢复。
6. `distributed/store/strongstore/shardclient.cc:346`在接受到返回的结果后，提取reply里的digest和proof，使用prover进行验证。验证结果使用promise返回。