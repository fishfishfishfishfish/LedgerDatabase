#include <stdio.h>

#include <time.h>
#include <future>

#include "tbb/concurrent_hash_map.h"
#include "tbb/concurrent_queue.h"
#include "boost/thread.hpp"


#include "distributed/store/common/truetime.h"
#include "distributed/mocks/strongstore/client.h" // mock

using namespace std;

int main(int argc, char **argv) {
  printf("Hello boost %d\n", BOOST_VERSION);
}
