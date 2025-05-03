db_name=$1
echo "db_name: $db_name"
# 定义测试参数数组
# load_account=(1000000 10000000)
load_account=(10000000)
load_batch_size=10000
value_sizes=(1024)
# value_sizes=(256 512 1024 2048)
key_size=32

if [ $db_name == "ledgerdb" ]; then
    num_transaction_account=20
    num_transaction_version=0
    get_proof="-p true"
elif [ $db_name == "sqlledger" ]; then
    num_transaction_account=20
    num_transaction_version=45
    get_proof="-p true"
elif [ $db_name == "qldb" ]; then
    num_transaction_account=20
    num_transaction_version=0
    get_proof="" # qldb get proof when call GetNVersion
else
    echo "Invalid db_name. Please use ledgerdb, sqlledger or qldb."
    exit 1
fi
query_versions="2,4,10,20,40"


data_path="$PWD/../data/"
result_dir="$PWD/results_${db_name}/lineage_benchmark"
echo "data_path: $data_path"
echo "result_dir: $result_dir"


mkdir -p $data_path
mkdir -p ${result_dir}
# rm -rf ${result_dir}/*

# 运行测试
for n_acc in "${load_account[@]}"; do
    for value_size in "${value_sizes[@]}"; do
        set -x
        # 清理数据文件夹
        rm -rf $data_path/*
        
        result_path="${result_dir}/e${n_acc}v${value_size}.csv"
        echo $(date "+%Y-%m-%d %H:%M:%S") 
        echo "num account: ${n_acc}, value_size: ${value_size}, key_size: ${key_size}" 
        # 运行测试并提取结果
        # ../build_release_${db_name}/bin/lineageBenchmark -a $n_acc -b $load_batch_size -t $num_transaction_version -z $num_transaction_account -l $query_versions -k $key_size -v $value_size -d $data_path -r $result_path
        ../build_release_${db_name}/bin/lineageBenchmark -a $n_acc -b $load_batch_size -t $num_transaction_version -z $num_transaction_account $get_proof -l $query_versions -k $key_size -v $value_size -d $data_path -r $result_path
        sleep 5
        set +x
    done

done