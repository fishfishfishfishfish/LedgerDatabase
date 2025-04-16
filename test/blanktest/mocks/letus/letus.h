#ifndef LETUS
#define LETUS

namespace letus {

class LETUS {
 public:
  LETUS(int timeout,
        std::string dbpath = "/tmp/testdb",
        std::string ledgerPath = "/tmp/testledger");

  ~LETUS();

  bool GetDigest(strongstore::proto::Reply* reply);

  bool GetProof(const std::map<uint64_t, std::vector<std::string>>& keys,
                strongstore::proto::Reply* reply);

  bool GetProof(const uint64_t& seq,
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
}


} // namespace letus
#endif // LETUS