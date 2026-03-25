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



LedgerDB再写入的时候需要显式给定一个版本号。
LedgerDB会用skiplist_head_记录每个key最新的版本号。
在扫描的时候，LedgerDB通过skiplist往前追溯一个key的历史版本。



测试指令
```bash
  cd exps/
  cd ../ && ./build_release_all.sh && cd exps/
  DB_NAME=ledgerdb ./scale_benchmark.sh 
  DB_NAME=qldb ./scale_benchmark.sh 
  DB_NAME=sqlledger ./scale_benchmark.sh 
```

# 内存占用差异分析
文件1，2都是运行同一个程序 (`scale_benchmark`) 获得的日志。我观察到，文件1 的内存占用动态变化，但是文件2的内存呈现长期不变、阶梯上升的情况。
```log
# 文件1：
...
write base data version: 58800, memory: 4309536768 bytes (4109.89 MB)
write base data version: 58900, memory: 4452212736 bytes (4245.96 MB)
write base data version: 59000, memory: 4599676928 bytes (4386.59 MB)
write base data version: 59100, memory: 4751302656 bytes (4531.20 MB)
write base data version: 59200, memory: 4379107328 bytes (4176.24 MB)
write base data version: 59300, memory: 4520259584 bytes (4310.86 MB)
write base data version: 59400, memory: 4664733696 bytes (4448.64 MB)
write base data version: 59500, memory: 4287860736 bytes (4089.22 MB)
write base data version: 59600, memory: 4431482880 bytes (4226.19 MB)
write base data version: 59700, memory: 4577206272 bytes (4365.16 MB)
write base data version: 59800, memory: 4731277312 bytes (4512.10 MB)
write base data version: 59900, memory: 4359602176 bytes (4157.64 MB)
write base data version: 60000, memory: 4500414464 bytes (4291.93 MB)
write base data version: 60100, memory: 4645433344 bytes (4430.23 MB)
write base data version: 60200, memory: 4270374912 bytes (4072.55 MB)
write base data version: 60300, memory: 4428726272 bytes (4223.56 MB)
write base data version: 60400, memory: 4572598272 bytes (4360.77 MB)
write base data version: 60500, memory: 4727996416 bytes (4508.97 MB)
write base data version: 60600, memory: 4364091392 bytes (4161.92 MB)
write base data version: 60700, memory: 4519415808 bytes (4310.05 MB)
write base data version: 60800, memory: 4665110528 bytes (4449.00 MB)
write base data version: 60900, memory: 4810219520 bytes (4587.38 MB)
...
```
```log
#文件2
...
write base data version: 58800, memory: 4309536768 bytes (4109.89 MB)
write base data version: 58900, memory: 4452212736 bytes (4245.96 MB)
write base data version: 59000, memory: 4599676928 bytes (4386.59 MB)
write base data version: 59100, memory: 4751302656 bytes (4531.20 MB)
write base data version: 59200, memory: 4379107328 bytes (4176.24 MB)
write base data version: 59300, memory: 4520259584 bytes (4310.86 MB)
write base data version: 59400, memory: 4664733696 bytes (4448.64 MB)
write base data version: 59500, memory: 4287860736 bytes (4089.22 MB)
write base data version: 59600, memory: 4431482880 bytes (4226.19 MB)
write base data version: 59700, memory: 4577206272 bytes (4365.16 MB)
write base data version: 59800, memory: 4731277312 bytes (4512.10 MB)
write base data version: 59900, memory: 4359602176 bytes (4157.64 MB)
write base data version: 60000, memory: 4500414464 bytes (4291.93 MB)
write base data version: 60100, memory: 4645433344 bytes (4430.23 MB)
write base data version: 60200, memory: 4270374912 bytes (4072.55 MB)
write base data version: 60300, memory: 4428726272 bytes (4223.56 MB)
write base data version: 60400, memory: 4572598272 bytes (4360.77 MB)
write base data version: 60500, memory: 4727996416 bytes (4508.97 MB)
write base data version: 60600, memory: 4364091392 bytes (4161.92 MB)
write base data version: 60700, memory: 4519415808 bytes (4310.05 MB)
write base data version: 60800, memory: 4665110528 bytes (4449.00 MB)
write base data version: 60900, memory: 4810219520 bytes (4587.38 MB)
...
```
文件1运行环境是Linux version 5.4.0-216-generic (buildd@lcy02-amd64-014) (gcc version 9.4.0 (Ubuntu 9.4.0-1ubuntu1~20.04.2)) #236-Ubuntu SMP Fri Apr 11 19:53:21 UTC 2025，
文件2的运行环境是Linux version 4.15.0-213-generic (buildd@lcy02-amd64-079) (gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)) #224-Ubuntu SMP Mon Jun 19 13:30:12 UTC 2023，
他们运行的程序是同一个二进制文件的两个副本，理论上没有程序、编译方面的差别。
下面是内存检测的代码：
```cpp
// Get current process memory usage in bytes
uint64_t get_process_memory_usage() {
  FILE* file = fopen("/proc/self/statm", "r");
  if (!file) {
    return 0;
  }

  uint64_t size, resident, share, text, lib, data, dt;
  if (fscanf(file, "%lu %lu %lu %lu %lu %lu %lu", &size, &resident, &share,
             &text, &lib, &data, &dt) != 7) {
    fclose(file);
    return 0;
  }
  fclose(file);

  // resident is the resident set size in pages
  // page size is typically 4096 bytes on Linux
  long page_size = sysconf(_SC_PAGESIZE);
  return resident * page_size;
}
```
请帮我解释一下他们内存检测出来的结果不同的原因。

这是一个非常经典且有趣的系统底层现象。你的 C++ 内存检测代码读取了 `/proc/self/statm` 的第二列（`resident`），这意味着你测量的是进程的 **RSS（Resident Set Size，常驻内存集）**。

虽然两个环境运行的是同一个二进制程序（理论上 `malloc` 和 `free` 或 `new` 和 `delete` 的调用时机和数量完全一致），但由于**操作系统内核（Kernel）**和**C 标准运行库（glibc）**版本的不同，导致了底层物理内存的分配和回收策略出现了显著差异。

以下是导致文件1（动态波动）和文件2（长期不变、阶梯上升）结果不同的核心原因： 

### 1. 测量的是物理内存 (RSS) 而不是虚拟内存
你的代码测量的是 RSS，它代表**进程实际映射并在物理内存（RAM）中占用的空间**。
* 当程序调用 `malloc` 申请内存时，操作系统通常只分配“虚拟内存”。只有当程序**真正去读写**这块内存时，内核才会触发“缺页中断（Page Fault）”，并分配真实的物理内存，此时 RSS 才会增加。
* 当程序调用 `free` 释放内存时，这块内存并不一定会立刻归还给操作系统，因此 **RSS 并不一定会立刻下降**。

### 2. Glibc 内存分配器 (ptmalloc) 的归还策略差异
这是导致阶梯状与动态波动最可能的原因。即使是相同的二进制文件，只要是动态链接（默认情况），它们就会加载各自系统中的 `libc.so`。
* **文件1环境 (Ubuntu 20.04, glibc 2.31):** 较新的 glibc 内存碎片整理和归还机制可能更积极。当程序 `free` 掉一大块内存时，glibc 可能会调用 `munmap` 或 `madvise(MADV_DONTNEED)`，这会**立即**指示操作系统收回物理页面，导致你观察到 RSS 的动态下降和波动。
* **文件2环境 (Ubuntu 18.04, glibc 2.27):** 旧版 glibc 的 ptmalloc（或者其默认阈值配置）倾向于将释放的物理内存保留在进程自己的内存池（Arena / Tcache）中，以备后用。这种“缓存”行为可以减少后续频繁向系统申请内存的系统调用开销。因此，RSS 会长期保持不变（因为物理内存还在进程手里）；当预留的池子不够用了，再向系统申请时，RSS 就会呈现**“阶梯状上升”**。 

### 3. 内存释放系统调用: `MADV_DONTNEED` vs `MADV_FREE`
在 Linux 中，进程告诉内核某块内存不再使用了，通常有两种标记：
* **`MADV_DONTNEED`**: 内核会立刻剥夺这块物理内存，RSS **立刻下降**。
* **`MADV_FREE`**: 内核会将这块内存标记为“可回收”。**如果当前系统物理内存充足，内核会“懒惰”地不回收它**，这块内存依然算在当前进程的 RSS 中（表现为 RSS 居高不下、长期不变）。只有当系统内存紧张时，内核才会真正回收它。
* Linux 内核和 glibc 的组合在不同版本中对这两种方式的默认选择有所不同。文件2的系统可能触发了类似于 `MADV_FREE` 的行为，且物理机内存充足，导致内核懒得回收。

### 4. 操作系统的页面缓存与 Mmap (内核 5.4 vs 4.15)
从日志中看出你运行的是一个数据库基准测试 (`sqlledger`)。数据库系统通常会大量使用文件 I/O 甚至内存映射 (`mmap`)。
* `/proc/self/statm` 中的 RSS 是包含被映射到进程地址空间的文件页面（Page Cache）的。
* **文件1 (Kernel 5.4)** 拥有更现代的内存管理和 LRU（最近最少使用）页面回收算法。它可能会更频繁、更精细地将不活跃的文件缓存页淘汰出物理内存，从而引起 RSS 的上下波动。
* **文件2 (Kernel 4.15)** 的页面回收策略可能不同，或者该环境本身剩余内存极大，内核认为没有必要驱逐这些页面，导致 RSS 一直维持在高位并阶梯上升（随着新文件的读取）。

### 5. 透明大页 (Transparent Huge Pages, THP) 机制的影响

Linux 系统通常默认开启 THP。它试图以 2MB（而不是通常的 4KB）为单位分配内存。
* 如果文件2的环境中 THP 工作得更为活跃，内存分配是以 2MB 为步长“大块”进行的。由于大块内存中只要有一小部分还在被使用，整个 2MB 就无法被释放，这非常容易导致 RSS 呈现**典型的“阶梯状”上升**且难以回落。

### 总结与验证建议
* **文件1 的波动**反映了操作系统/运行库在**积极地**回收空闲内存（用时间换空间）。
* **文件2 的阶梯/不变**反映了操作系统/运行库在**缓存**已申请的物理内存，避免频繁的系统调用（用空间换时间）。

**如果你想在两边得到一致的内存行为：**
你可以尝试在启动程序前，强制更改 glibc 的内存释放策略，例如在两个终端中都设置环境变量，强制 glibc 在 `free` 时立刻归还内存给系统：
```bash
export MALLOC_ARENA_MAX=1
export MALLOC_TRIM_THRESHOLD_=102400
export GLIBC_TUNABLES=glibc.malloc.tcache_count=0
```
或者，更彻底的方法是，不使用系统自带的 `glibc ptmalloc`，而是在编译时链接第三方的内存分配器（如 Google 的 `tcmalloc` 或 Meta 的 `jemalloc`），这能最大限度抹平不同 Linux 发行版底层内存分配的差异。