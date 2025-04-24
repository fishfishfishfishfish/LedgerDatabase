1. 如何获取`shard$i.config`一系列文件？运行`exps/load.sh $1 $2`. $1是shard个数，`exps/run_exp.sh`中设置为16. $2可以不加或者为`-r`, 表示是否启动replication。每个shard对应`exps/replicas`中一个ip。如果启用replication，则一个shard对应3个ip。
2. mock的任务：去掉transport，timeserver
3. 
```
                     client                                    server
Get()    ->  transaction.addReadSet()
Put()    ->  transaction.addWriteSet()
Commit() ->  Prepare(),                         ->    record tid -> transaction
                pass the transaction to the server
             Commit(tid)                        ->    commit the transaction of tid 
```

4. put的时候
会将timestamp传入作为version
每个value的形式都为 "{block}@{value}"


5. 
```
cmake -DLEDGERDB=ON -DAMZQLDB=OFF ..;
```


6. dependencies
   1. libevent: https://blog.csdn.net/m0_59068776/article/details/133785983
   1. rocksdb: https://www.cnblogs.com/renjc/p/rocksdb-install.html
   1. cryptopp: https://www.jianshu.com/p/4c470d0e6af4
   1. boost: https://www.boost.org/users/history/, https://zhuanlan.zhihu.com/p/539122040
   1. jsoncpp: https://github.com/open-source-parsers/jsoncpp, https://www.cnblogs.com/paw5zx/p/18245875

7. protobuf的版本是有限考虑PATH中靠前路径的那个。
先`whereis protoc`找出系统中所有的protoc，然后一个个进行 --version检查版本。
把需要的那个protoc的路径加到PATH最前。
```
export PATH=/usr/bin:$PATH
```

8. crypto++
```
install -m 644 *.h /usr/local/include/cryptopp
install -m 644 libcryptopp.a /usr/local/lib
install cryptest.exe /usr/local/bin
install -m 644 TestData/*.dat /usr/local/share/cryptopp/TestData
install -m 644 TestVectors/*.txt /usr/local/share/cryptopp/TestVectors
```

9. boost 导致 Attempt to free invalid pointer, core dumped
1. 更新boost库的版本到1.67. 先apt-get autoremove旧版本，再使用source安装。
1. https://blog.csdn.net/qq_38876396/article/details/143093305，将tcmalloc库包含到cmake

cd /home/xinyu.chen/LedgerDatabase/build/distributed && /opt/cmake-3.12.0/bin/cmake -E cmake_link_script CMakeFiles/verifyClient.dir/link.txt --verbose=1
/usr/bin/c++   -std=c++11 -O2 -pthread -lssl -Wall -Wno-unused-function -fPIC   CMakeFiles/verifyClient.dir/exes/verifyClient-mock.cc.o  -o ../bin/verifyClient -Wl,
-rpath,/home/xinyu.chen/LedgerDatabase/build/lib:/usr/local/lib 
../lib/libdist.so 
../lib/libledger.so 
/usr/local/lib/libevent.so 
/usr/local/lib/libevent_openssl.so 
/usr/local/lib/libevent_core.so 
/usr/local/lib/libevent_extra.so 
/usr/local/lib/libevent_pthreads.so 
/usr/local/lib/libevent_extra.so 
/usr/local/lib/libevent_pthreads.so 
-levent_openssl -lcrypto 
/usr/lib/x86_64-linux-gnu/libprotobuf.so 
/usr/lib/x86_64-linux-gnu/libboost_filesystem.so 
/usr/lib/x86_64-linux-gnu/libboost_program_options.so 
/usr/lib/x86_64-linux-gnu/libboost_thread.so 
/usr/lib/x86_64-linux-gnu/libboost_system.so 
/usr/lib/x86_64-linux-gnu/libboost_chrono.so 
/usr/lib/x86_64-linux-gnu/libboost_date_time.so 
/usr/lib/x86_64-linux-gnu/libboost_atomic.so 
/usr/local/lib/librocksdb.so 
/usr/lib/x86_64-linux-gnu/libcryptopp.so 
-ltbb 



# test storage
**Server** `distributed/store/strongstore/server.h`定义的是整个服务器，其实现在`distributed/exes/strong-server.cc`. Server的类属性`TxnStore *store;`使用的是`strongstore::OCCStore`
**OCCStore**`distributed/store/strongstore/occstore.h`的类属性`VersionedKVStore`
**VersionedKVStore**`distributed/store/common/backend/versionstore.h`其实才是ledger database的通用接口。

VersionedKVStore的构造函数会根据编译的宏定义（`#ifdef`）选取对应的ledger database: LedgerDB, QLDB, SQLLedger
接口有以下函数, 都需要通过宏定义实现各个ledger databse
```C++
  VersionedKVStore();
  VersionedKVStore(const std::string& db_path, int timeout);
  ~VersionedKVStore();
```

获取根节点hash。ledger database接收一个`tip`（树的id）和一个`hash`, 将根节点hash写入`hash`
```C++
  bool GetDigest(strongstore::proto::Reply* reply);
```

```
  bool BatchGet(const std::vector<std::string>& keys,
      strongstore::proto::Reply* reply);
```


```C++
  bool GetNVersions(std::vector<std::pair<std::string, size_t>>& ver_keys,
      strongstore::proto::Reply* reply);
```
```C++
  bool GetProof(const std::map<uint64_t, std::vector<std::string>>& keys,
                strongstore::proto::Reply* reply);

  bool GetProof(const uint64_t& seq,
                strongstore::proto::Reply* reply);

  bool GetRange(const std::string &start, const std::string &end,
                strongstore::proto::Reply* reply);

  void put(const std::vector<std::string> &keys,
           const std::vector<std::string> &values,
           const Timestamp &t,
           strongstore::proto::Reply* reply);

  bool get(const std::string &key,
           const Timestamp &t,
           std::pair<Timestamp, std::string> &value);
```


value的形式
## LEDGERDB
嵌套的pair
```
(timestamp, (block, value))
```

## SQLLEDGER
string
```
"block|<unknown>|<unknown>|key|value|timestamp"
```


## AMZQLDB
Document类型，使用string进行构造
```
class Document{
  BlockAddress addr_;
  Data data_;
  MetaData metadata_;
}
struct BlockAddress {
  Slice ledger_name;
  uint64_t seq_no;
};

struct Data {
  Slice key;
  Slice val;
};
struct MetaData{
  size_t doc_seq;
  size_t version;
  uint64_t time;
}
```