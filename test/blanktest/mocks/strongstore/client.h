#ifndef _STRONG_CLIENT_H_
#define _STRONG_CLIENT_H_

#include <map>
#include <set>
#include <string>
#include <thread>
#include <vector>

namespace mockstrongstore {

class Client {
 public:
  Client();
  Client(int timeout, const std::string& db_path = "/tmp/letusdb");
  ~Client();

  void Begin();
  // Overriding functions from ::Client
  int Get(const std::string& key);
  int GetNVersions(const std::string& key, size_t n);
  void BufferKey(const std::string& key);
  int BatchGet(std::map<std::string, std::string>& values);
  int BatchGet(std::map<std::string, std::string>& values,
               std::map<int, std::map<uint64_t, std::vector<std::string>>>&
                   unverified_keys);
  int GetRange(const std::string& from, const std::string& to,
               std::map<std::string, std::string>& values,
               std::map<int, std::map<uint64_t, std::vector<std::string>>>&
                   unverified_keys);
  int Put(const std::string& key, const std::string& value);
  // commit without verification
  bool Commit();
  // commit with verification
  bool Commit(
      std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys);
  void Abort();
  bool Verify(
      std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys);
  std::vector<int> Stats();
  bool Audit(std::map<int, uint64_t>& seqs);

 private:
  // local Prepare function
  int Prepare(uint64_t& ts);

  // get timestamp
  uint64_t GetTime();

  // Unique ID for this client.
  uint64_t client_id;

  // Ongoing transaction ID.
  uint64_t t_id;

  std::vector<std::pair<std::string, size_t>> history;

  // Time spend sleeping for commit.
  int commit_sleep;
};

}  // namespace mockstrongstore

#endif /* _STRONG_CLIENT_H_ */
