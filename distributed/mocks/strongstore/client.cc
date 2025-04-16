#include "distributed/mocks/strongstore/client.h"

using namespace std;

namespace mockstrongstore {

Client::Client()
{}

Client::Client(int timeout, const std::string& db_path)
{
    // Initialize all state here;
    client_id = 0;
    t_id = 0;

    // TODO: open a rocksdb with db_path
    store.reset(new letus::Letus(timeout, db_path));
}

Client::~Client()
{}

/* Begins a transaction. All subsequent operations before a commit() or
 * abort() are part of this transaction.
 *
 * Return a TID for the transaction.
 */
void
Client::Begin()
{
    t_id++;
    // Initialize a transaction.
    txn = Transaction();
    history.clear();
}

int Client::GetNVersions(const string& key, size_t n) {
    Promise promise(GET_TIMEOUT);

    history.emplace_back(std::make_pair(key, n));

    promise.Reply(REPLY_OK);
    return promise.GetReply();
}

int Client::Get(const string &key)
{
    Promise promise(GET_TIMEOUT);

    // Read your own writes, check the write set first.
    if (txn.getWriteSet().find(key) != txn.getWriteSet().end()) {
        promise.Reply(REPLY_OK);
        return REPLY_OK;
    }

    if (txn.getReadSet().find(key) == txn.getReadSet().end()) {
        txn.addReadSet(key, Timestamp());
    }
    promise.Reply(REPLY_OK);
    return promise.GetReply();
}

int Client::BatchGet(std::map<std::string, std::string>& values, std::map<int,
    std::map<uint64_t, std::vector<std::string>>>& keys) {
  int status = REPLY_OK;
  // TODO
  return status;
}

int Client::BatchGet(std::map<std::string, std::string>& values) {
  int status = REPLY_OK;
  // TODO
  return status;
}

int Client::GetRange(const string& from, const string& to,
    std::map<std::string, std::string>& values, std::map<int,
    std::map<uint64_t, std::vector<std::string>>>& keys) {
  int status = REPLY_OK;
  // TODO
  return status;
}

void Client::BufferKey(const std::string& key) {
  // TODO
}

/* Sets the value corresponding to the supplied key. */
int
Client::Put(const string &key, const string &value)
{
    Promise promise(PUT_TIMEOUT);

    // Update the write set.
    txn.addWriteSet(key, value);
    promise.Reply(REPLY_OK);

    return promise.GetReply();
}

int
Client::Prepare(uint64_t &ts)
{
    int status;

    // 0. go get a timestamp for OCC

    // 1. Send commit-prepare to all shards.
    Promise promise(PUT_TIMEOUT);
    // TODO: send current transaction to the server
    // prepared[tid] = transaction
    // generate request str
    // string request_str;
    // strongstore::proto::Request request;
    // request.set_op(strongstore::proto::Request::PREPARE);
    // request.set_txnid(t_id);
    // txn.serialize(request.mutable_prepare()->mutable_txn());
    // ofstream log(db_path + to_string(t_id) + "Prepare");
    // request.SerializeToOstream(&log);

    promise.Reply(REPLY_OK, Timestamp(GetTime()));

    // 2. Wait for reply from all shards. (abort on timeout)
    status = promise.GetReply();
    ts = promise.GetTimestamp().getTimestamp();
    // TODO: If any shard returned false, abort the transaction.

    return status;
}

/* Attempts to commit the ongoing transaction. */
bool
Client::Commit()
{
    return true;
}

bool Client::Commit(std::map<int, std::map<uint64_t, std::vector<std::string>>>& vkeys) {
    /* keys: 2d mapping, (shard, block) -> key */
  // Implementing 2 Phase Commit
  uint64_t ts = 0;
  int status;

  for (int i = 0; i < COMMIT_RETRIES; i++) {
      status = Prepare(ts);
      if (status == REPLY_OK || status == REPLY_FAIL) {
          break;
      }
  }

  if (status == REPLY_OK) {
    //   Promise promise(PREPARE_TIMEOUT);
    // generate request str
    //   string request_str;
    //   strongstore::proto::Request request;
    //   request.set_op(strongstore::proto::Request::COMMIT);
    //   request.set_txnid(t_id);
    //   request.mutable_commit()->set_timestamp(ts);
    //   if (history.size() > 0) {
    //       auto ver_msg = request.mutable_version();
    //       for (auto& vk : history) {
    //       auto ver_keys = ver_msg->add_versionedkeys();
    //       ver_keys->set_key(vk.first);
    //       ver_keys->set_nversions(vk.second);
    //       }
    //   }
    //   ofstream log(db_path + to_string(t_id) + "Commit");
    //   request.SerializeToOstream(&log);
    // Send commits
    Promise promise(PREPARE_TIMEOUT);
    strongstore::proto::Reply reply;

    store->GetDigest(&reply);
    std::vector<std::string> keys, vals;
    for (auto &read: txn.getReadSet()) {
      keys.push_back(read.first);
    }
    if (keys.size() > 0) {
      store->BatchGet(keys, &reply);
      keys.clear();
    }

    if (history.size() > 0) {
      store->GetNVersions(history, &reply);
    }

    for (auto &write : txn.getWriteSet()) {
      keys.push_back(write.first);
      vals.push_back(write.second);
    }
    if (keys.size() > 0) {
      store->put(keys, vals, Timestamp(ts), &reply);
    }
    reply.set_status(0);

    for (size_t i = 0; i < reply.values_size(); ++i) {
      auto values = reply.values(i);
      auto block = values.estimate_block();
      auto key = values.key();
      /* we only use one shard, shard = 0 */
      if (vkeys.find(0) != vkeys.end()) {
        if (vkeys[0].find(block) != vkeys[0].end()) {
          vkeys[0][block].emplace_back(key);
        } else {
          vkeys[0].emplace(block, std::vector<std::string>{key});
        }
      } else {
        std::map<uint64_t, std::vector<std::string>> innermap;
        innermap.emplace(block, std::vector<std::string>{key});
        vkeys.emplace(0, innermap);
      }
    }
      
    return true;
  }

  // 4. If not, send abort to all shards.
  Abort();
  return false;
}

/* Aborts the ongoing transaction. */
void
Client::Abort()
{}

bool Client::Verify(std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys) {
  strongstore::proto::Reply reply;
  size_t nkeys = 0;
  for (auto k = keys.begin(); k != keys.end();) {
    for (auto block = k->second.begin(); block != k->second.end();) {
      while (block->second.size() > 10000) {
        std::vector<std::string> newvec(block->second.begin(), block->second.begin() + 10000);
        std::map<uint64_t, std::vector<std::string>> keys;
        keys.emplace(block->first, newvec);
        store->GetProof(keys, &reply);
        block->second.erase(block->second.begin(), block->second.begin() + 10000);
        nkeys += 10000;
      }
      std::map<uint64_t, std::vector<std::string>> keys;
      keys.emplace(block->first, block->second);
      store->GetProof(keys, &reply);
      nkeys += block->second.size();
      ++block;
    }
    if (k->second.size() == 0) {
        k = keys.erase(k);
    } else {
        ++k;
    }
  }
  return true;
}

bool Client::Audit(std::map<int, uint64_t>& seqs) {
    // Contact the appropriate shard to set the value.
    bool status = true;
    // TODO: audit
    return status;
}


/* Return statistics of most recent transaction. */
vector<int>
Client::Stats()
{
    vector<int> v;
    return v;
}


uint64_t
Client::GetTime()
{
    struct timeval now;
    uint64_t timestamp;

    gettimeofday(&now, NULL);

    // now.tv_usec += simSkew;
    if (now.tv_usec > 999999) {
        now.tv_usec -= 1000000;
        now.tv_sec++;
    } else if (now.tv_usec < 0) {
        now.tv_usec += 1000000;
        now.tv_sec--;
    }

    timestamp = ((uint64_t)now.tv_sec << 32) | (uint64_t) (now.tv_usec);

    
    return timestamp;
}


} // namespace strongstore
