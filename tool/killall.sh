#!/bin/sh

#### A new script for clean up the test cluster ####
#    use the script in one machine of the test     #
#    cluster, as a result, all processes can be    #
#    killed by search will be killed except the    #
#    pts one owned by the current user.            #
####################################################

PSSH_DIR="./pssh-2.3.1"

if [ $# -ne 1 ]; then
	echo "Usage: ./`basename $0` hostlist"
	exit 1
fi
if [ ! -f $1 ]; then
	echo "\"$1\" is not a regular file!"
	echo "Usage: ./`basename $0` hostlist"
	exit 2
fi

# kill all processes can be killed by me of current machine except pts
CUR_PTS="pts/$(basename `tty`)"
ps -ef | grep -v "${CUR_PTS}" | awk '{print $2}' | xargs -i kill -9 {} &>/dev/null
# kill all processes can be killed by me of other machines
CUR_HOST=`hostname`
TMP_FILE="/tmp/.`date +%s | md5sum | awk '{print $1}'`"
grep -v "\#" "$1" | awk 'BEGIN{FS=":"}{print $2}' | sort | uniq | grep -v "${CUR_HOST}" >"${TMP_FILE}"
if [ -f "${TMP_FILE}" ]; then
	HOST_LIST="$TMP_FILE"
else
	echo "generate ${TMP_FILE} from $1 is failed!"
	exit 3
fi

PSSH="`which pssh 2>/dev/null`"
if [ $? -ne 0 ]; then
	if [ ! -d "$PSSH_DIR" ]; then
		echo "pssh is not installed!"
		exit 4
	else
		PSSH="${PSSH_DIR}/bin/pssh"
	fi
fi

CMDLINE="ps -ef | awk '/^search  */' | grep -v notty \
	| grep -v ps | grep -v awk | grep -v xargs \
	| grep -v kill | grep -v grep | awk '{print \$2}'\
	| xargs -i kill -9 {}"
$PSSH -h "$HOST_LIST" -p 50 "$CMDLINE"

rm -rf "${TMP_FILE}"
