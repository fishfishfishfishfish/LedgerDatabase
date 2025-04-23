if [ $(echo "$1" | awk '{print tolower($0)}') == 'qldb' ]
then
  qldbopt=ON
  ledgerdbopt=OFF
else
  qldbopt=OFF
  ledgerdbopt=ON
fi

. env.sh

cd ..
mkdir -p build
cd build
cmake -DLEDGERDB=${ledgerdbopt} -DAMZQLDB=${qldbopt} ..
make -j6