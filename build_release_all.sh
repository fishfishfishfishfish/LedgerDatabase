export LD_LIBRARY_PATH=/home/xinyuchen/oneTBB-tbb_2020/build/linux_intel64_gcc_cc11.4.0_libc2.35_kernel4.15.0_release:$LD_LIBRARY_PATH
export TBB_DIR=/home/xinyuchen/oneTBB-tbb_2020/build/linux_intel64_gcc_cc11.4.0_libc2.35_kernel4.15.0_release
./build_release.sh qldb > build_release.log 2>&1
./build_release.sh sqlledger >> build_release.log 2>&1
./build_release.sh ledgerdb >> build_release.log 2>&1