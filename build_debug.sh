if [ $(echo "$1" | awk '{print tolower($0)}') == 'qldb' ]
then
  qldbopt=ON
  ledgerdbopt=OFF
else
  qldbopt=OFF
  ledgerdbopt=ON
fi

. env.sh

mkdir -p build_debug
# rm -rf build_debug/*
cd build_debug
cmake -DCMAKE_BUILD_TYPE=DEBUG -DLEDGERDB=${ledgerdbopt} -DAMZQLDB=${qldbopt} ..
make -j6 VERBOSE=1 