#============= Parameters to fill ============
rtime=10          # duration to run, second
delay=100          # verification delay

tlen=10            # transaction length, how many op in a task/transaction
wper=50            # writes percentage, percentage of write op
rper=50            # reads percentage, percentage of read op
zalpha=0           # zipf alpha

. env.sh
client="tpccClient"
mode="occ"
txnrate=5        # number of transaction every second

echo "$bindir/$client -c $expdir/shard -N $nshard -d $rtime -m $mode -e $err -s $skew -x $txnrate"
$bindir/$client -c $expdir/shard -N $nshard -d $rtime -m $mode -e $err -s $skew -x $txnrate