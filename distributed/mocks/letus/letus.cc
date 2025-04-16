#include "distributed/mocks/letus/letus.h"

#include <json/json.h>

using namespace std;

namespace letus {

Letus::Letus(int timeout, std::string dbpath, std::string ledgerPath):
  log(dbpath){
  next_block_seq_ = 0;
}

Letus::~Letus(){
  log.close();
}


bool 
Letus::GetDigest(strongstore::proto::Reply* reply){
  Json::Value trace;
  trace["op"] = "GetDigest";
  Json::StreamWriterBuilder writer; 
  string trace_str = Json::writeString(writer, trace);
  trace_str.erase(std::remove_if(trace_str.begin(), trace_str.end(), ::isspace), trace_str.end());
  log << trace_str << endl;
  return true;
}

bool 
Letus::GetProof(const std::map<uint64_t, std::vector<std::string>>& keys,
            strongstore::proto::Reply* reply){
  Json::Value trace;
  trace["op"] = "GetProof";
  for(auto& k : keys){
    Json::Value block;
    block["id"] = k.first;
    for(int j = 0; j < k.second.size(); j++){
      block["key"].append(k.second[j]);
    }
    trace["block"].append(block);
  }
  Json::StreamWriterBuilder writer; 
  string trace_str = Json::writeString(writer, trace);
  trace_str.erase(std::remove_if(trace_str.begin(), trace_str.end(), ::isspace), trace_str.end());
  log << trace_str << endl;
  return true;
}

bool 
Letus::GetProof(const uint64_t& seq,
            strongstore::proto::Reply* reply){
  log << "GetProof" << endl;
  return true;
}

bool 
Letus::GetNVersions(
    std::vector<std::pair<std::string, size_t>>& ver_keys,
    strongstore::proto::Reply* reply){
  Json::Value trace;
  trace["op"] = "GetNVersions";
  for(int i = 0; i < ver_keys.size(); i++){
    trace["keys"][i] = ver_keys[i].first;
    trace["n"][i] = ver_keys[i].second;
  }
  Json::StreamWriterBuilder writer; 
  string trace_str = Json::writeString(writer, trace);
  trace_str.erase(std::remove_if(trace_str.begin(), trace_str.end(), ::isspace), trace_str.end());
  log << trace_str << endl;
  // make reply
  auto blk_seq = next_block_seq_-1;
  for(int i = 0; i < ver_keys.size(); i++){
    for (size_t j = 0; j < ver_keys[i].second; j++) {
      auto kv = reply->add_values();
      kv->set_key(ver_keys[i].first);
      kv->set_val(default_val);
      kv->set_estimate_block(blk_seq - j);
    }
  }
  return true;
}

void 
Letus::put(const std::vector<std::string> &keys,
        const std::vector<std::string> &values,
        const Timestamp &t,
        strongstore::proto::Reply* reply){
  Json::Value trace;
  trace["op"] = "put";
  for(int i = 0; i < keys.size(); i++){
    trace["keys"][i] = keys[i];
    trace["values"][i] = values[i];
  }
  Json::StreamWriterBuilder writer; 
  string trace_str = Json::writeString(writer, trace);
  trace_str.erase(std::remove_if(trace_str.begin(), trace_str.end(), ::isspace), trace_str.end());
  log << trace_str << endl;
  // make reply
  auto blk_seq = next_block_seq_++;
  for (size_t i = 0; i < keys.size(); ++i) {
    auto kv = reply->add_values();
    kv->set_key(keys[i]);
    kv->set_val(values[i]);
    kv->set_estimate_block(blk_seq);
  }
}

bool 
Letus::get(const std::string &key,
        const Timestamp &t,
        std::pair<Timestamp, std::string> &value){
  log << "get" << endl;
  return true;
}

bool 
Letus::BatchGet(const std::vector<std::string>& keys,
    strongstore::proto::Reply* reply){
  Json::Value trace;
  trace["op"] = "BatchGet";
  for(int i = 0; i < keys.size(); i++){
    trace["keys"][i] = keys[i];
  }
  Json::StreamWriterBuilder writer; 
  string trace_str = Json::writeString(writer, trace);
  trace_str.erase(std::remove_if(trace_str.begin(), trace_str.end(), ::isspace), trace_str.end());
  log << trace_str << endl;
  // make reply
  auto blk_seq = next_block_seq_ > 0 ? next_block_seq_-1 : 0;
  for (size_t i = 0; i < keys.size(); ++i) {
    auto kv = reply->add_values();
    kv->set_key(keys[i]);
    kv->set_val(default_val);
    kv->set_estimate_block(blk_seq);
  }
  return true;
}


} // namespace letus