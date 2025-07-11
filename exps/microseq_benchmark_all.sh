./microseq_benchmark.sh sqlledger > microseq_benchmark_sqlledger.log 2>&1
python3 plot_micro_benchmark.py sqlledger microseq_benchmark
./microseq_benchmark.sh ledgerdb > microseq_benchmark_ledgerdb.log 2>&1
python3 plot_micro_benchmark.py ledgerdb microseq_benchmark
./microseq_benchmark.sh qldb > microseq_benchmark_qldb.log 2>&1
python3 plot_micro_benchmark.py qldb microseq_benchmark