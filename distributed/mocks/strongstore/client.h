#ifndef _STRONG_CLIENT_H_
#define _STRONG_CLIENT_H_
#include <iostream>
#include <fstream>

#include "distributed/lib/assert.h"
#include "distributed/lib/message.h"
#include "distributed/lib/configuration.h"
#include "distributed/lib/tcptransport.h"
#include "distributed/store/common/frontend/client.h"
#include "distributed/store/strongstore/shardclient.h"
#include "distributed/store/common/transaction.h"
#include "distributed/proto/strong-proto.pb.h"
#include "distributed/proto/request.pb.h"
#include "distributed/mocks/letus/letus.h"



#include <set>
#include <thread>

namespace mockstrongstore {

class Client : public ::Client
{
public:
    Client();
    Client(int timeout, const std::string& db_path = "/tmp/letusdb");
    ~Client();

    // Overriding functions from ::Client
    void Begin();
    int Get(const string &key);
    int GetNVersions(const string& key, size_t n);
    void BufferKey(const string& key);
    int BatchGet(std::map<std::string, std::string>& values);
    int BatchGet(std::map<std::string, std::string>& values, std::map<int,
        std::map<uint64_t, std::vector<std::string>>>& unverified_keys);
    int GetRange(const string& from, const string& to,
        std::map<std::string, std::string>& values, std::map<int,
        std::map<uint64_t, std::vector<std::string>>>& unverified_keys);
    int Put(const string &key, const string &value);
    // commit without verification
    bool Commit();
    // commit with verification
    bool Commit(std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys);
    void Abort();
    bool Verify(std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys);
    std::vector<int> Stats();
    bool Audit(std::map<int, uint64_t>& seqs);

private:
    // local Prepare function
    int Prepare(uint64_t &ts);

    // get timestamp
    uint64_t GetTime();

    // Unique ID for this client.
    uint64_t client_id;

    // Ongoing transaction ID.
    uint64_t t_id;

    // Transaction to keep track of read and write set.
    Transaction txn;

    std::vector<std::pair<std::string, size_t>> history;

    // std::ofstream log;
    // std::string db_path;
    std::unique_ptr<letus::Letus> store;

    // Synchronization variables.
    std::condition_variable cv;
    std::mutex cv_m;
    string replica_reply;

    // Time spend sleeping for commit.
    int commit_sleep;
};

} // namespace strongstore

#endif /* _STRONG_CLIENT_H_ */
