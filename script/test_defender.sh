#!/bin/sh

#test_defender.sh:
#    作用: 保护运行该脚本的机器上待测试程序
#          测试过程中不受其他用户和进程的干扰
#    限制: 仅作用于公用账号，比如默认是search
#    比如: nohup ./test_defender.sh -t 3600 -i 5 -u apache --host 10.16.28.12 nginx &>/dev/null &
#          以上命令执行: 表示运行命令的机器上, apache用户启动的程序(该程序的启动命令中含有关键字nginx),
#          不会被杀死，apache账号启动的其他程序(包括登陆伪终端)都将被杀死; 该脚本程序存活3600s, 每隔5s执行一次.
#          host选项表示, 指定的ip可以登陆到该机器上，而不会被该脚本程序杀死.
#    #unix1986@qq.com

declare -i RUN_TIME=60 # 1min
declare -i RUN_INTERVAL=5 # 5s
declare -i IS_TEST=0
KEY_WORD="nginx"
PROG_NAME=`basename $0`
CUR_USER="apache"
LOGIN_HOST="127.0.0.1"

function usage() {
    echo
    echo -e "Auto kill the processes except the being tested ones\n"
    echo -e "Usage: ./$PROG_NAME [options] keyword\n"
    echo -e "\tkeyword: represent the being tested processes\n"
    echo -e "\t-h"
    echo -e "\t\tprint the usage message\n"
    echo -e "\t-t time(s)"
    echo -e "\t\tset the running time\n"
    echo -e "\t-i time(s)"
    echo -e "\t\tset interval time\n"
    echo -e "\t-u cur_user"
    echo -e "\t\tuser to be managed\n"
    echo -e "\t--host host_ip"
    echo -e "\t\thost ip can login in\n"
    echo -e "\t--test"
    echo -e "\t\tdon't kill any process, only echo pids\n"
    echo
}

function parse_args() {
    OPTARGS=`getopt -o k:t:i:u:h -l "host:,test" -n "$PROG_NAME" -- "$@"`
    [ $? -ne 0 ] && usage && exit 1
    eval set -- "$OPTARGS"

    while true; do
        case $1 in
            -h) usage; exit 0;;
            -t) RUN_TIME=$2; shift 2;;
            -i) RUN_INTERVAL=$2; shift 2;;
            -u) CUR_USER=$2; shift 2;;
            --host) LOGIN_HOST=$2; shift 2;;
            --test) IS_TEST=1; shift;;
            --) shift; break;;
            *) echo "Invalid parameters [$1]!"; usage; exit 1;;
        esac
    done

    if [ $# -ne 1 ]; then
        echo "Must be one position parameter, current number $#."
        usage
        exit 1
    fi
    KEY_WORD=$1
}

function check() {
    items=`w | grep "^${CUR_USER:0:8}.*$LOGIN_HOST" | awk '{print $2}'`
    extra_condition=""
    for item in ${items[@]}; do
       extra_condition="$extra_condition\\|$item"     
    done
    pids=`ps -U$CUR_USER -u$CUR_USER uh | grep -v \
        "$KEY_WORD\|$PROG_NAME\|ps\|awk$extra_condition\|grep" \
        | awk '{print $2}'`
    if [ $IS_TEST -eq 1 ]; then
        echo "[$LOGIN_HOST]"
        echo $pids
    else
        echo $pids | xargs -n1 | xargs -i kill -9 {}
    fi
}

function main() {
    parse_args $@
    declare -i cur_time=`date +%s`
    declare -i end_time=$((cur_time + RUN_TIME))
    while [ $cur_time -lt  $end_time ]; do
        check
        sleep $RUN_INTERVAL
        cur_time=$((cur_time + RUN_INTERVAL))
    done
}

main $@
