#!/bin/bash
db_name=$1
test_name=$2
echo "db_name: $db_name, test_name=$test_name"
# 定义测试参数数组
load_account=(1000000)
# batch_sizes=(500 1000 2000 3000 4000)
batch_sizes=(1000)
# value_sizes=(256 512 1024 2048)
value_sizes=(1024)
num_transaction_version=20
load_batch_size=5000
# load_batch_size=100000
key_size=32


data_path="$PWD/../data/"
result_dir="$PWD/results_${db_name}/micro_benchmark_${test_name}"
echo "data_path: $data_path"
echo "result_dir: $result_dir"

mkdir -p $data_path
mkdir -p ${result_dir}
rm -rf ${result_dir}/*
cd ${result_dir}
mkdir -p "profiling"
cd ../..

# 运行测试
for n_acc in "${load_account[@]}"; do
    for batch_size in "${batch_sizes[@]}"; do
        for value_size in "${value_sizes[@]}"; do
            set -x
            # 清理数据文件夹
            rm -rf $data_path/*
            
            result_path="${result_dir}/e${n_acc}b${batch_size}v${value_size}.csv"
            echo $(date "+%Y-%m-%d %H:%M:%S") 
            echo "num account: ${n_acc}, batch_size: ${batch_size}, value_size: ${value_size}, key_size: ${key_size}"

            # 使用perf进行性能分析
            sudo perf record -g -F 99 ../build_debug_${db_name}/bin/microBenchmark -a $n_acc -b $load_batch_size -t $num_transaction_version -z $batch_size -k $key_size -v $value_size -d $data_path -r $result_path
            
            # 生成火焰图
            sudo perf script | ./FlameGraph/stackcollapse-perf.pl > perf.folded
            ./FlameGraph/flamegraph.pl perf.folded > ${result_dir}/profiling/flamegraph_e${n_acc}b${batch_size}v${value_size}.svg
            
            # 生成性能报告
            sudo perf report --stdio > ${result_dir}/profiling/perf_report_e${n_acc}b${batch_size}v${value_size}.txt

            sleep 5
        done
    done
done

# python3 plot.py get_put_hashed_key
