#include "distributed/store/strongstore/client.h"

using namespace std;

namespace mockstrongstore {

Client::Client()
{
    // Initialize all state here;
    client_id = 0;
    while (client_id == 0) {
        random_device rd;
        mt19937_64 gen(rd());
        uniform_int_distribution<uint64_t> dis;
        client_id = dis(gen);
    }
    t_id = (client_id/10000)*10000;
}

Client::~Client()
{}

/* Runs the transport event loop. */
void
Client::run_client()
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
    participants.clear();
    commit_sleep = -1;

    // Initialize a transaction.
    txn = Transaction();
}

int Client::GetNVersions(const string& key, size_t n) {
    Promise promise(GET_TIMEOUT);

    history.emplace_back(std::make_pair(key, n));

    promise->Reply(REPLY_OK);
    return promise.GetReply();
}

int Client::Get(const string &key)
{
    Promise promise(GET_TIMEOUT);

    // Read your own writes, check the write set first.
    if (txn.getWriteSet().find(key) != txn.getWriteSet().end()) {
        promise->Reply(REPLY_OK);
        return;
    }

    if (txn.getReadSet().find(key) == txn.getReadSet().end()) {
        txn.addReadSet(key, Timestamp());
    }
    promise->Reply(REPLY_OK);
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
    promise->Reply(REPLY_OK);

    return promise.GetReply();
}

int
Client::Prepare(uint64_t &ts)
{
    int status;

    // 0. go get a timestamp for OCC
    if (mode == MODE_OCC) {
        // Have to go to timestamp server
        ts = 0;
    }

    // 1. Send commit-prepare to all shards.
    Promise promise(PUT_TIMEOUT);
    // TODO: Ask the server to prepare
    promise.Reply(REPLY_OK, Timestamp());

    // 2. Wait for reply from all shards. (abort on timeout)
    status = REPLY_OK;
    // If any shard returned false, abort the transaction.
    if (promise->GetReply() != REPLY_OK) {
        if (status != REPLY_FAIL) {
            status = p->GetReply();
        }
    }
    // Also, find the max of all prepare timestamp returned.
    if (promise->GetTimestamp().getTimestamp() > ts) {
        ts = p->GetTimestamp().getTimestamp();
    }

    return status;
}

/* Attempts to commit the ongoing transaction. */
bool
Client::Commit()
{
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
        // TODO: Send commits
        return true;
    }

    // 4. If not, send abort to all shards.
    Abort();
    return false;
}

bool Client::Commit(std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys) {
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
        // Send commits
        Promise promise(PREPARE_TIMEOUT);
        bclient[p]->Commit(ts, promise);

        promise->GetReply();
        for (size_t i = 0; i < promise->EstimateBlockSize(); ++i) {
          auto block = promise->GetEstimateBlock(i);
          auto key = promise->GetUnverifiedKey(i);
          if (keys.find(entry.first) != keys.end()) {
            if (keys[entry.first].find(block) != keys[entry.first].end()) {
              keys[entry.first][block].emplace_back(key);
            } else {
              keys[entry.first].emplace(block, std::vector<std::string>{key});
            }
          } else {
            std::map<uint64_t, std::vector<std::string>> innermap;
            innermap.emplace(block, std::vector<std::string>{key});
            keys.emplace(entry.first, innermap);
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
{
        for (auto& p : participants) {
        bclient[p]->Abort();
    }
}

bool Client::Verify(std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys) {
    // Contact the appropriate shard to set the value.
    bool is_successful = true;

#ifndef AMZQLDB
    list<Promise*> promises;
    size_t nkeys = 0;

    for (auto k = keys.begin(); k != keys.end();) {
        bool verifiable = true;
        for (auto block = k->second.begin(); block != k->second.end();) {
            if (!verifiable) {
                block = k->second.erase(block);
            } else {
                while (block->second.size() > 10000) {
                  promises.push_back(new Promise());
                  std::vector<std::string> newvec(block->second.begin(),
                      block->second.begin() + 10000);
                  if (!bclient[k->first]->Verify(block->first, newvec,
                      promises.back())) {
                    verifiable = false;
                    promises.pop_back();
                    break;
                  }
                  block->second.erase(block->second.begin(), block->second.begin() + 10000);
                  nkeys += 10000;
                }

                promises.push_back(new Promise());
                if (!verifiable || !bclient[k->first]->Verify(block->first,
                                            block->second,
                                            promises.back())) {
                    verifiable = false;
                    block = k->second.erase(block);
                    promises.pop_back();
                } else {
                    nkeys += block->second.size();
                    ++block;
                }
            }
        }
        if (k->second.size() == 0) {
            k = keys.erase(k);
        } else {
            ++k;
        }
    }
    std::cout << "verifynkeys " << nkeys << std::endl;

    for (auto& p : promises) {
        if (p->GetReply() != REPLY_OK ||
            p->GetVerifyStatus() != VerifyStatus::PASS) {
          is_successful = false;
        }
        delete p;
    }
#endif

    return is_successful;
}

bool Client::Audit(std::map<int, uint64_t>& seqs) {
    // Contact the appropriate shard to set the value.
    bool status = true;
    list<Promise *> promises;

    if (seqs.size() == 0) {
      for(size_t n = 0; n < nshards; ++n) {
        seqs.emplace(n, 0);
      }
    }

    for (auto k = seqs.begin(); k != seqs.end(); ++k) {
        promises.push_back(new Promise());
        bclient[k->first]->Audit(k->second, promises.back());
    }

    int n = 0;
    for (auto p : promises) {
        if (p->GetReply() != REPLY_OK) {
          status = false;
        } else if (p->GetVerifyStatus() != VerifyStatus::UNVERIFIED) {
          ++seqs[n];
        }
        ++n;
        delete p;
    }
    return status;
}


/* Return statistics of most recent transaction. */
vector<int>
Client::Stats()
{
    vector<int> v;
    return v;
}

/* Callback from a tss replica upon any request. */
void
Client::tssCallback(const string &request, const string &reply)
{
    lock_guard<mutex> lock(cv_m);

    // Copy reply to "replica_reply".
    replica_reply = reply;

    // Wake up thread waiting for the reply.
    cv.notify_all();
}

} // namespace strongstore
