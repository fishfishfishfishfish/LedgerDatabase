#ifndef LETUS
#define LETUS
#include <iostream>
#include <fstream>

#include "distributed/store/common/frontend/txnclient.h"
#include "distributed/store/common/timestamp.h"
#include "ledger/ledgerdb/ledgerdb.h"
#include "distributed/proto/strong-proto.pb.h"

namespace letus {

class Letus {
 public:
  Letus(int timeout, std::string dbpath = "/tmp/testdb", std::string ledgerPath = "/tmp/testledger");

  ~Letus();

  bool GetDigest(strongstore::proto::Reply* reply);

  bool GetProof(const std::map<uint64_t, std::vector<std::string>>& keys,
                strongstore::proto::Reply* reply);

  bool GetProof(const uint64_t& seq,
                strongstore::proto::Reply* reply);

  bool GetNVersions(
    std::vector<std::pair<std::string, size_t>>& ver_keys,
    strongstore::proto::Reply* reply);

  void put(const std::vector<std::string> &keys,
           const std::vector<std::string> &values,
           const Timestamp &t,
           strongstore::proto::Reply* reply);

  bool get(const std::string &key,
           const Timestamp &t,
           std::pair<Timestamp, std::string> &value);

  bool BatchGet(const std::vector<std::string>& keys,
      strongstore::proto::Reply* reply);
 private:
  std::ofstream log;
  uint64_t next_block_seq_;
  std::string default_val = "aaaaaaaaaaaaaaaaaaaa";
};


} // namespace letus
#endif // LETUS