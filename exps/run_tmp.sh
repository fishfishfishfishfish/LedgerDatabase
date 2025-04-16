cd /home/xinyu.chen/LedgerDatabase/build;
make;
cd /home/xinyu.chen/LedgerDatabase/exps;
/home/xinyu.chen/LedgerDatabase/build/bin/verifyClient -c /home/xinyu.chen/LedgerDatabase/exps/shard -N  -d 10 -l 10 -w 50 -g 50 -m occ -e 0 -s 0 -z 0 -t 100 -x 1;