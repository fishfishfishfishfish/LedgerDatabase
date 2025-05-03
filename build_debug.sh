if [ $(echo "$1" | awk '{print tolower($0)}') == 'qldb' ]
then
  qldbopt=ON
  ledgerdbopt=OFF
  sqlledgeropt=OFF
  build_dir=build_debug_qldb
else
  if [ $(echo "$1" | awk '{print tolower($0)}') == 'ledgerdb' ]
  then
    qldbopt=OFF
    ledgerdbopt=ON
    sqlledgeropt=OFF
    build_dir=build_debug_ledgerdb
  else
    qldbopt=OFF
    ledgerdbopt=OFF
    sqlledgeropt=ON
    build_dir=build_debug_sqlledger
  fi  
fi
. env.sh

mkdir -p ./${build_dir}
# rm -rf ./${build_dir}
cd ./${build_dir}

cmake -DCMAKE_BUILD_TYPE=DEBUG -DLEDGERDB=${ledgerdbopt} -DAMZQLDB=${qldbopt} -DSQLLEDGER=${sqlledgeropt} ..
make -j6 VERBOSE=1 