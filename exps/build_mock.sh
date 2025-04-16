rootdir=/home/xinyu.chen/LedgerDatabase;
mkdir -p ${rootdir}/build; 
cd ${rootdir}/build; 
# rm -rf *; 
export PATH=/usr/include:/usr/local/include:/usr/include/crypto++:/usr/lib/x86_64-linux-gnu:/usr/lib:/usr/bin:/usr/local/sbin:/usr/local/lib:/usr/local/bin:/usr/sbin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin;
export LD_LIBRARY_PATH=/usr/include:/usr/local/lib:/usr/local/include:/usr/include/crypto++:/usr/lib/x86_64-linux-gnu;
# cmake -DLEDGERDB=ON -DAMZQLDB=OFF -DBUILD_STATIC_LIBS=OFF -DCMAKE_VERBOSE_MAKEFILE=ON  ..; 
make;