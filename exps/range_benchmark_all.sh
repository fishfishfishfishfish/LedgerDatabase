timestamp=$(date +"%Y%m%d_%H%M%S")
# timestamp="test"
echo $timestamp

./range_benchmark.sh qldb $timestamp > range_benchmark_qldb_${timestamp}.log 2>&1
python3 plot_range_benchmark.py qldb range_benchmark_${timestamp}
# ./range_benchmark.sh sqlledger $timestamp > range_benchmark_sqlledger_${timestamp}.log 2>&1
# python3 plot_range_benchmark.py sqlledger range_benchmark_${timestamp}
# ./range_benchmark.sh ledgerdb $timestamp > range_benchmark_ledgerdb_${timestamp}.log 2>&1
# python3 plot_range_benchmark.py ledgerdb range_benchmark_${timestamp}
