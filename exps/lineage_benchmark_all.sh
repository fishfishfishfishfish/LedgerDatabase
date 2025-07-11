timestamp=$(date +"%Y%m%d_%H%M%S")
echo $timestamp

# ./lineage_benchmark.sh ledgerdb $timestamp > lineage_benchmark_ledgerdb_${timestamp}.log 2>&1
# python3 plot_lineage_benchmark.py ledgerdb lineage_benchmark_${timestamp}

./lineage_benchmark.sh sqlledger $timestamp > lineage_benchmark_sqlledger_${timestamp}.log 2>&1
python3 plot_lineage_benchmark.py sqlledger lineage_benchmark_${timestamp}

# ./lineage_benchmark.sh qldb $timestamp > lineage_benchmark_qldb_${timestamp}.log 2>&1
# python3 plot_lineage_benchmark.py qldb lineage_benchmark_${timestamp}


