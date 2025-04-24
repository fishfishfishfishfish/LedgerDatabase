if [ $(echo "$1" | awk '{print tolower($0)}') == 'qldb' ]
then
  qldbopt=ON
  ledgerdbopt=OFF
else
  qldbopt=OFF
  ledgerdbopt=ON
fi

. env.sh

mkdir -p build_release
# rm -rf build_release/*
cd build_release
cmake -DLEDGERDB=${ledgerdbopt} -DAMZQLDB=${qldbopt} ..
make -j6 VERBOSE=1 