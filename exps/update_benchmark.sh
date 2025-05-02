db_name=$1
echo "db_name: $db_name"
# 定义测试参数数组
load_account=(500 1000 2000 3000 4000 5000)
value_sizes=(1024)
key_size=32
update_count=100

data_path="$PWD/../data/"
result_dir="$PWD/results_${db_name}/update_benchmark"
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
        
        result_path="${result_dir}/e${n_acc}u${update_count}v${value_size}.csv"
        echo $(date "+%Y-%m-%d %H:%M:%S") 
        echo "num account: ${n_acc}, update count:${update_count}, value_size: ${value_size}, key_size: ${key_size}" 
        # 运行测试并提取结果
        ../build_release_${db_name}/bin/updateBenchmark -a $n_acc -t $update_count -k $key_size -v $value_size -d $data_path -r $result_path
        sleep 5
        set +x
    done
done