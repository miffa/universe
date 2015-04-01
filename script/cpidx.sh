#!/bin/sh

HOSTS="hostlist"

SRC_PATH="/home/search/zmq_gosearch3/*"
DST_PATH="/home/search/wangjingjing/"
declare -i THREAD_NUM=10

mkfifo /tmp/$$.fifo
exec 6<>/tmp/$$.fifo
rm -rf /tmp/$$.fifo

declare -i HOST_NUM=`cat $HOSTS| wc -l`
if [ $HOST_NUM -lt $THREAD_NUM ]; then
    THREAD_NUM=$HOST_NUM
fi

echo "Thread num: $THREAD_NUM"

for (( i = 0; i < THREAD_NUM; ++i )) {
    echo
} >&6

function child() {
    sudo -u search ssh $1 "rsync -r $SRC_PATH $2:$DST_PATH"
}

while read line; do
    read -u6
    src_host=`echo $line | cut -d' ' -f1`
    dst_host=`echo $line | cut -d' ' -f2`
    {
        child $src_host $dst_host && echo "$src_host finished." || echo "$src_host failed."
        echo >&6
    }&
done <$HOSTS

wait
exec 6<&-
echo "Done"

