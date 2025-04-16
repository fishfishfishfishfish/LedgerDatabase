#include "distributed/mocks/strongstore/client.h"

using namespace std;

namespace mockstrongstore {

Client::Client() {}

Client::Client(int timeout, const std::string& db_path) {}

Client::~Client() {}

/* Begins a transaction. All subsequent operations before a commit() or
 * abort() are part of this transaction.
 *
 * Return a TID for the transaction.
 */
void Client::Begin() {}

int Client::GetNVersions(const std::string& key, size_t n) { return 0; }

int Client::Get(const std::string& key) { return 0; }

int Client::BatchGet(
    std::map<std::string, std::string>& values,
    std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys) {
  return 0;
}

int Client::BatchGet(std::map<std::string, std::string>& values) { return 0; }

int Client::GetRange(
    const std::string& from, const std::string& to,
    std::map<std::string, std::string>& values,
    std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys) {
  return 0;
}

void Client::BufferKey(const std::string& key) {
  // TODO
}

/* Sets the value corresponding to the supplied key. */
int Client::Put(const std::string& key, const std::string& value) { return 0; }

int Client::Prepare(uint64_t& ts) { return 0; }

/* Attempts to commit the ongoing transaction. */
bool Client::Commit() { return true; }

bool Client::Commit(
    std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys) {
  return true;
}

/* Aborts the ongoing transaction. */
void Client::Abort() {}

bool Client::Verify(
    std::map<int, std::map<uint64_t, std::vector<std::string>>>& keys) {
  return true;
}

bool Client::Audit(std::map<int, uint64_t>& seqs) { return true; }

/* Return statistics of most recent transaction. */
vector<int> Client::Stats() {
  vector<int> v;
  return v;
}

uint64_t Client::GetTime() { return 0; }

}  // namespace mockstrongstore
