#!/bin/sh

PSSH_DIR="./pssh-2.3.1"

if [ $# -ne 1 ]; then
	echo "Usage: ./`basename $0` hostlist.txt"
	exit 1
fi
if [ ! -f $1 ]; then
	echo "\"$1\" is not a regular file!"
	echo "Usage: ./`basename $0` hostlist.txt"
	exit 2
fi

# kill all processes can be killed by me of current machine except pts
CUR_PTS="pts/$(basename `tty`)"
ps -ef | grep -v "${CUR_PTS}" | awk '{print $2}' | xargs -i kill -9 {} &>/dev/null
# kill all processes can be killed by me of other machines
CUR_HOST=`hostname`
TMP_FILE="/tmp/.`md5sum $0`"
rm -rf "${TMP_FILE}"
grep "${CUR_HOST}" $1 &>/dev/null && grep -v "${CUR_HOST}" $1 >"${TMP_FILE}"
if [ -f ${TMP_FILE} ]; then
	HOST_LIST="$TMP_FILE"
else
	HOST_LIST="$1"
fi

PSSH="`which pssh 2>/dev/null`"
if [ $? -ne 0 ]; then
	if [ ! -d "$PSSH_DIR" ]; then
		echo "pssh is not installed!"
		exit 3
	else
		PSSH="${PSSH_DIR}/bin/pssh"
	fi
fi

eval ${PSSH} -h "${HOST_LIST}" -p 50 "kill -9 -1"
