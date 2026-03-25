# rocksdb (≥ 5.8)
检查是否已经安装
```bash
$ rocksdb_dump --version
rocksdb_dump: command not found
```

```bash
~$ git clone git@github.com:facebook/rocksdb.git
~$ git clone https://github.com/facebook/rocksdb.git
Cloning into 'rocksdb'...
remote: Enumerating objects: 138060, done.
remote: Counting objects: 100% (471/471), done.
remote: Compressing objects: 100% (296/296), done.
remote: Total 138060 (delta 326), reused 177 (delta 175), pack-reused 137589 (from 3)
Receiving objects: 100% (138060/138060), 222.78 MiB | 1.36 MiB/s, done.
Resolving deltas: 100% (105734/105734), done.
~$ cd rocksdb/
~/rocksdb$ git checkout rocksdb-5.8.8
HEAD is now at 266ac245a Bumping version to 5.8
~/rocksdb$ make shared_lib
  GEN      util/build_version.cc
  GEN      util/build_version.cc
  CC       shared-objects/cache/clock_cache.o
  CC       shared-objects/cache/lru_cache.o
  CC       shared-objects/cache/sharded_cache.o
  CC       shared-objects/db/builder.o
  CC       shared-objects/db/c.o
  ...
  ln -fs librocksdb.so.5.8.0 librocksdb.so
  ln -fs librocksdb.so.5.8.0 librocksdb.so.5
  ln -fs librocksdb.so.5.8.0 librocksdb.so.5.8
~/rocksdb$ sudo make install-shared
  GEN      util/build_version.cc
install -d /usr/local/lib
for header_dir in `find "include/rocksdb" -type d`; do \
        install -d /usr/local/$header_dir; \
done
for header in `find "include/rocksdb" -type f -name *.h`; do \
        install -C -m 644 $header /usr/local/$header; \
done
install -C -m 755 librocksdb.so.5.8.0 /usr/local/lib && \
        ln -fs librocksdb.so.5.8.0 /usr/local/lib/librocksdb.so.5.8 && \
        ln -fs librocksdb.so.5.8.0 /usr/local/lib/librocksdb.so.5 && \
        ln -fs librocksdb.so.5.8.0 /usr/local/lib/librocksdb.so
```

示例错误
```
open error: Invalid argument: Compression type Snappy is not linked with the binary
```
解决方法
1. 安装 Snappy 库
确保系统已安装 Snappy 库及其开发文件。如果使用的是 Ubuntu，可以运行以下命令：
```
sudo apt-get update
sudo apt-get install libsnappy-dev
```
2. 重新编译 RocksDB
安装 Snappy 后，需要重新编译 RocksDB，以确保其正确链接到 Snappy 库。
```
make clean
make -j$(nproc)
```

示例错误：在编译LedgerDatabase的时候出现：
```bash
CMake Warning at distributed/CMakeLists.txt:69 (ADD_EXECUTABLE):
  Cannot generate a safe runtime search path for target strongstore because
  files in some directories may conflict with libraries in implicit
  directories:

    runtime library [librocksdb.so.5.8] in /usr/lib may be hidden by files in:
      /usr/local/lib

  Some of these libraries may not be found correctly.
```
解决方案：卸载重复的 RocksDB。确定项目需要的版本，删掉另一个：
- 若需要系统版（5.8）：卸载手动安装的
  `cd`到 rocksdb 源码目录，执行 `sudo make uninstall`
  然后删除残留：`sudo rm /usr/local/lib/librocksdb*`
- 若需要手动编译版：卸载系统包管理器的
    Ubuntu/Debian：`sudo apt remove librocksdb5.8 librocksdb-dev`
    CentOS/RHEL：`sudo yum remove rocksdb rocksdb-devel`

卸载完要删除之前编译LedgerDatabase的结果，重新编译


# protobuf (≥ 2.6.1)
检查是否已经安装
```bash
$ protoc --version
libprotoc 3.0.0
```
安装1
1. 下载安装包
```bash
wget https://github.com/protocolbuffers/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz
tar -xvzf protobuf-2.6.1.tar.gz
cd protobuf-2.6.1
```
2. 编译安装
```bash
./configure
make
make check
sudo make install
sudo ldconfig # refresh shared library cache.
```
3. 检查是否安装成功
```bash
protoc --version
libprotoc 2.6.1
```

安装2
1. 先安装bazel
https://github.com/bazelbuild/bazel/releases/tag/8.2.1
```bash
wget https://github.com/bazelbuild/bazel/releases/download/8.2.1/bazel-8.2.1-installer-linux-x86_64.sh
chmod +x bazel-8.2.1-installer-linux-x86_64.sh
./bazel-8.2.1-installer-linux-x86_64.sh
```

2. 下载protobuf
```bash
git clone https://github.com/protocolbuffers/protobuf.git
cd protobuf
git submodule update --init --recursive
git checkout v30.2
```
3. 编译protobuf
```bash
bazel build :protoc :protobuf
cp bazel-bin/protoc /usr/local/bin
```
4. 如果cmake仍然找出旧版本的protobuf
```bash
sudo ldconfig
# 检查 protobuf 库是否被正确识别
ldconfig -p | grep libprotobuf
# 确认库文件权限（应有执行权限）
ls -l /usr/local/lib/libprotobuf.so*
# 示例：删除Ubuntu自带版本
sudo apt remove libprotobuf-dev protobuf-compiler
```
5. 如果发现conda的path在最前面
```
conda init --reverse # 移除 Conda 对 Shell 的影响
```

# cryptopp (≥ 6.1.0)
检查是否已经安装
```bash
$ dpkg -l | grep cryptopp
$  find /usr/lib /usr/local/lib -name "libcryptopp*"
/usr/local/lib/libcryptopp.a
$ find /usr/include /usr/local/include -name "cryptopp"
/usr/local/include/cryptopp
```
安装
```bash
~$ git clone https://github.com/weidai11/cryptopp.git
Cloning into 'cryptopp'...
remote: Enumerating objects: 28997, done.
remote: Counting objects: 100% (185/185), done.
remote: Compressing objects: 100% (57/57), done.
remote: Total 28997 (delta 148), reused 128 (delta 128), pack-reused 28812 (from 2)
Receiving objects: 100% (28997/28997), 27.73 MiB | 1.64 MiB/s, done.
Resolving deltas: 100% (21117/21117), done.
~$ cd cryptopp/
~/cryptopp$ git checkout CRYPTOPP_6_1_0
Note: checking out 'CRYPTOPP_6_1_0'.

You are in 'detached HEAD' state. You can look around, make experimental
changes and commit them, and you can discard any commits you make in this
state without impacting any branches by performing another checkout.

If you want to create a new branch to retain commits you create, you may
do so (now or later) by using -b with the checkout command again. Example:

  git checkout -b <new-branch-name>

HEAD is now at 5be140bc Prepare for Crypto++ 6.1 release
~/cryptopp$ make
g++ -DNDEBUG -g2 -O3 -fPIC -pthread -pipe -c cryptlib.cpp
g++ -DNDEBUG -g2 -O3 -fPIC -pthread -pipe -c cpu.cpp
g++ -DNDEBUG -g2 -O3 -fPIC -pthread -pipe -c integer.cpp
...
g++ -DNDEBUG -g2 -O3 -fPIC -pthread -pipe -c dlltest.cpp
g++ -DNDEBUG -g2 -O3 -fPIC -pthread -pipe -c fipsalgt.cpp
g++ -o cryptest.exe -DNDEBUG -g2 -O3 -fPIC -pthread -pipe adhoc.o test.o bench1.o bench2.o validat0.o validat1.o validat2.o validat3.o validat4.o datatest.o regtest1.o regtest2.o regtest3.o dlltest.o fipsalgt.o ./libcryptopp.a  
~/cryptopp$ sudo make install
install -m 644 *.h /usr/local/include/cryptopp
install -m 644 libcryptopp.a /usr/local/lib
install cryptest.exe /usr/local/bin
install -m 644 TestData/*.dat /usr/local/share/cryptopp/TestData
install -m 644 TestVectors/*.txt /usr/local/share/cryptopp/TestVectors
```

# boost (≥ 1.67)
检查是否安装
```bash
$ dpkg -l | grep boost
ii  libboost-all-dev            1.65.1.0ubuntu1     amd64       Boost C++ Libraries development files (ALL) (default version)
ii  libboost-atomic-dev:amd64   1.65.1.0ubuntu1     amd64       atomic data types, operations, and memory ordering constraints (default version)
...

```
安装
```bash
$ wget https://archives.boost.io/release/1.67.0/source/boost_1_67_0.tar.gz
```
```
--2025-04-16 09:50:45--  https://archives.boost.io/release/1.67.0/source/boost_1_67_0.tar.gz
...
Saving to: ‘boost_1_67_0.tar.gz’
boost_1_67_0.tar.gz                100%[===============================================================>]  98.58M   926KB/s    in 2m 0s   
2025-04-16 09:52:45 (844 KB/s) - ‘boost_1_67_0.tar.gz’ saved [103363944/103363944]
$ tar -xzvf boost_1_67_0.tar.gz 
...
boost_1_67_0/tools/quickbook/test/xml_escape-1_5.quickbook
boost_1_67_0/tools/quickbook/Jamfile.v2
boost_1_67_0/tools/quickbook/_clang-format
boost_1_67_0/tools/quickbook/index.html
boost_1_67_0/tools/Jamfile.v2
boost_1_67_0/tools/index.html
boost_1_67_0/tools/make-cputime-page.pl
```
```bash
$ ./bootstrap.sh --with-libraries=all
$ ./b2
```
```
The Boost C++ Libraries were successfully built!

The following directory should be added to compiler include paths:

    /home/xinyuchen/boost_1_67_0

The following directory should be added to linker library paths:

    /home/xinyuchen/boost_1_67_0/stage/lib
```

> 如果没有输出最后的sucessful built, 则说明安装失败
> 一种情况是找不到pyconfig.h: No such file or directory，说明没有安装python3的开发库
>    solution: `sudo apt install python-dev`, 注意不是python3-dev, 可能要对应系统默认的python版本。安装好之后记得重新`./bootstrap.sh --with-libraries=all`.

```bash
$ sudo ./b2 install --prefix=/usr/local/opt/boost1.67
$ sudo ./b2 install
```
```
(base) xinyu.chen@246:~/Ledgerdatabase_deps/boost_1_67_0$ ./b2 install
/home/xinyu.chen/Ledgerdatabase_deps/boost_1_67_0/libs/predef/check/../tools/check/predef.jam:46: Unescaped special character in argument $(language)::$(expression)
Performing configuration checks

    - default address-model    : 64-bit
    - default architecture     : x86
    - symlinks supported       : yes
    - C++11 mutex              : yes
    - lockfree boost::atomic_flag : yes
    - Boost.Config Feature Check: cxx11_auto_declarations : yes
    - Boost.Config Feature Check: cxx11_constexpr : yes
    - Boost.Config Feature Check: cxx11_defaulted_functions : yes
    - Boost.Config Feature Check: cxx11_final : yes
    - Boost.Config Feature Check: cxx11_hdr_mutex : yes
    - Boost.Config Feature Check: cxx11_hdr_regex : yes
    - Boost.Config Feature Check: cxx11_hdr_tuple : yes
    - Boost.Config Feature Check: cxx11_lambdas : yes
    - Boost.Config Feature Check: cxx11_noexcept : yes
    - Boost.Config Feature Check: cxx11_nullptr : yes
    - Boost.Config Feature Check: cxx11_rvalue_references : yes
    - Boost.Config Feature Check: cxx11_template_aliases : yes
    - Boost.Config Feature Check: cxx11_thread_local : yes
    - Boost.Config Feature Check: cxx11_variadic_templates : yes
    - has_icu builds           : yes
...
```
最后，如果cmake一直没有正确找到boost安装的位置，需要在`cmake/Dependencies.cmake`里指定Boost的路径
```cmake
set(BOOST_ROOT "/usr/local/opt/boost1.67")
set(Boost_NO_SYSTEM_PATHS ON) # 只搜索指定路径
```


# Intel Threading Building Block (tbb_2020 version)
检查是否安装
```bash
$ dpkg -l | grep tbb
ii  libtbb2                   2020.3-0ubuntu1       amd64        Intel Threading Building Blocks
ii  libtbb-dev                2020.3-0ubuntu1       amd64        Intel Threading Building Blocks development files
```


# libevent (2.1.12)
1. 下载
```bash
$ wget https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz
$ tar zxvf libevent-2.1.12-stable.tar.gz 
```
2. 
```bash
$ cd libevent-2.1.12-stable/
$ ./configure
```
输出：
```bash
checking for a BSD-compatible install... /usr/bin/install -c
checking whether build environment is sane... yes
checking for a thread-safe mkdir -p... /bin/mkdir -p
checking for gawk... gawk
checking whether make sets $(MAKE)... yes
checking whether make supports nested variables... yes
checking whether make supports nested variables... (cached) yes
checking whether make supports the include directive... yes (GNU style)
checking for gcc... gcc
checking whether the C compiler works... yes
checking for C compiler default output file name... a.out
...
checking size of pthread_t... 8
checking that generated files are newer than configure... done
configure: creating ./config.status
config.status: creating libevent.pc
config.status: creating libevent_openssl.pc
config.status: creating libevent_pthreads.pc
config.status: creating libevent_core.pc
config.status: creating libevent_extra.pc
config.status: creating Makefile
config.status: creating config.h
config.status: creating evconfig-private.h
config.status: executing depfiles commands
config.status: executing libtool commands
```
在执行期间有些人就会出现报错了，例如：configure: error: openssl is a must but can  not be found.这是由于我们libevent在配置阶段缺失了对openssl的依赖，我们可以用下面的命令来解除这个错误。
```bash
$ sudo apt-get install libssl-dev
```
同时我们需要使用下面的命令来查看OpenSSL库的路径，因为我们后面在运行configure命令时需要添加--with-ssl选项并指定OpenSSL的安装路径
```bash
$ openssl version -d
OPENSSLDIR: "/usr/lib/ssl"
```
接下来我们要将路径复制下来添加到下面的代码后面，重新检测当前系统的安装环境。
```bash
$ ./configure --with-ssl="/usr/lib/ssl"
```
3. 编译
```bash
$ make
```
输出：
```bash
  GEN      test/rpcgen-attempted
  GEN      include/event2/event-config.h
make  all-am
make[1]: Entering directory '/home/xinyuchen/libevent-2.1.12-stable'
  CC       sample/dns-example.o
  CC       buffer.lo
  CC       bufferevent.lo
  CC       bufferevent_filter.lo
  CC       bufferevent_pair.lo
  CC       bufferevent_ratelim.lo
....
 CC       test/regress-regress_rpc.o
  CC       test/regress-regress_testutils.o
  CC       test/regress-regress_util.o
  CC       test/regress-tinytest.o
  CC       test/regress-regress_thread.o
  CC       test/regress-regress_zlib.o
  CC       test/regress-regress_ssl.o
  CCLD     libevent_extra.la
/usr/bin/ld: warning: /lib/x86_64-linux-gnu/libc.so.6: unsupported GNU_PROPERTY_TYPE (5) type: 0xc0008002
  CC       evthread_pthread.lo
  CCLD     libevent_pthreads.la
/usr/bin/ld: warning: /lib/x86_64-linux-gnu/libc.so.6: unsupported GNU_PROPERTY_TYPE (5) type: 0xc0008002
  CCLD     test/regress
/usr/bin/ld: warning: /usr/lib/gcc/x86_64-linux-gnu/11/../../../x86_64-linux-gnu/Scrt1.o: unsupported GNU_PROPERTY_TYPE (5) type: 0xc0008002
/usr/bin/ld: warning: /lib/x86_64-linux-gnu/libc.so.6: unsupported GNU_PROPERTY_TYPE (5) type: 0xc0008002
make[1]: Leaving directory '/home/xinyuchen/libevent-2.1.12-stable'
```
4. 安装
```bash
$ sudo make install
```
输出：
```bash
make  install-am
make[1]: Entering directory '/home/xinyuchen/libevent-2.1.12-stable'
make[2]: Entering directory '/home/xinyuchen/libevent-2.1.12-stable'
 /bin/mkdir -p '/usr/local/bin'
 /usr/bin/install -c event_rpcgen.py '/usr/local/bin'
 /bin/mkdir -p '/usr/local/lib'
 /bin/bash ./libtool   --mode=install /usr/bin/install -c   libevent.la libevent_core.la libevent_extra.la libevent_pthreads.la libevent_openssl.la '/usr/local/lib'
...
See any operating system documentation about shared libraries for
more information, such as the ld(1) and ld.so(8) manual pages.
----------------------------------------------------------------------
 /bin/mkdir -p '/usr/local/include'
 /usr/bin/install -c -m 644 include/evdns.h include/event.h include/evhttp.h include/evrpc.h include/evutil.h '/usr/local/include'
 /bin/mkdir -p '/usr/local/include/event2'
 /usr/bin/install -c -m 644 include/event2/buffer.h include/event2/buffer_compat.h include/event2/bufferevent.h include/event2/bufferevent_compat.h include/event2/bufferevent_struct.h include/event2/dns.h include/event2/dns_compat.h include/event2/dns_struct.h include/event2/event.h include/event2/event_compat.h include/event2/event_struct.h include/event2/http.h include/event2/http_compat.h include/event2/http_struct.h include/event2/keyvalq_struct.h include/event2/listener.h include/event2/rpc.h include/event2/rpc_compat.h include/event2/rpc_struct.h include/event2/tag.h include/event2/tag_compat.h include/event2/thread.h include/event2/util.h include/event2/visibility.h include/event2/bufferevent_ssl.h '/usr/local/include/event2'
 /bin/mkdir -p '/usr/local/include/event2'
 /usr/bin/install -c -m 644 include/event2/event-config.h '/usr/local/include/event2'
 /bin/mkdir -p '/usr/local/lib/pkgconfig'
 /usr/bin/install -c -m 644 libevent.pc libevent_core.pc libevent_extra.pc libevent_pthreads.pc libevent_openssl.pc '/usr/local/lib/pkgconfig'
make[2]: Leaving directory '/home/xinyuchen/libevent-2.1.12-stable'
make[1]: Leaving directory '/home/xinyuchen/libevent-2.1.12-stable'
```
5. 验证
接下来我们就要进行验证了，进入到安装目录的sample目录中。 
```bash
$ cd sample/
```
我们用hello-world.c进行测试，libevent的动态库名是libevent.so
```bash
$ gcc hello-world.c -o hello -levent
/usr/bin/ld: warning: /usr/lib/gcc/x86_64-linux-gnu/11/../../../x86_64-linux-gnu/Scrt1.o: unsupported GNU_PROPERTY_TYPE (5) type: 0xc0008002
/usr/bin/ld: warning: /lib/x86_64-linux-gnu/libc.so.6: unsupported GNU_PROPERTY_TYPE (5) type: 0xc0008002
/usr/bin/ld: warning: //lib/x86_64-linux-gnu/libpthread.so.0: unsupported GNU_PROPERTY_TYPE (5) type: 0xc0008002
$ ldd hello
        linux-vdso.so.1 (0x00007ffd97df6000)
        libevent-2.1.so.6 => /usr/lib/x86_64-linux-gnu/libevent-2.1.so.6 (0x00007fb296e00000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fb296bd8000)
        libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007fb297550000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fb29756e000)
```