if [ $(echo "$1" | awk '{print tolower($0)}') == 'qldb' ]
then
  qldbopt=ON
  ledgerdbopt=OFF
  sqlledgeropt=OFF
  build_dir=build_release_qldb
else
  if [ $(echo "$1" | awk '{print tolower($0)}') == 'ledgerdb' ]
  then
    qldbopt=OFF
    ledgerdbopt=ON
    sqlledgeropt=OFF
    build_dir=build_release_ledgerdb
  else
    qldbopt=OFF
    ledgerdbopt=ON
    sqlledgeropt=OFF
    build_dir=build_release_sqlledger
  fi  
fi

. env.sh

mkdir -p ./${build_dir}
# rm -rf ./${build_dir}
cd ./${build_dir}

cmake -DLEDGERDB=${ledgerdbopt} -DAMZQLDB=${qldbopt} -DSQLLEDGER=${sqlledgeropt} ..
make -j6 VERBOSE=1 