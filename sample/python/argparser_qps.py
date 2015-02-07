#!/bin/env python

import sys
import platform

# check python version
if sys.version_info < (2, 7, 0):
    print ("Current python version %s, " 
           "but require version %s or above") % \
			(platform.python_version(), "2.7.0")
    sys.exit(1)

import argparse
import subprocess
import time

# parse command line arguments
def ArgParse():
    parser = argparse.ArgumentParser(
        description = "Compute qps by monitoring a access log file")
    parser.add_argument(
        "logfile",
        help = "for example: /path/access.qfedd.log")
    parser.add_argument(
        "-i", "--interval", type = int,
        help = "interval time (s) for statistics")
    parser.add_argument(
        "-f", "--flush", action = "store_true",
        help = "flush file buffer")
    return parser.parse_args()

# child func
def LineNum(file):
    try:
        return int(subprocess.check_output(['wc', '-l', file]).split()[0])
    except Exception, e:
        print e
        sys.exit(1)

# main func
def Main(args):
    interval = 30
    if args.interval and args.interval > 0:
        interval = args.interval
    start_line_num = LineNum(args.logfile)
    try:
        while True:
            time.sleep(interval)
            cur_line_num = LineNum(args.logfile)
            print "[" + time.strftime("%m-%d %H:%M:%S", time.localtime(time.time())) + "] ",
            print "QPS: %d q/s" % ((cur_line_num - start_line_num) / interval)
            if args.flush:
                sys.stdout.flush()
            start_line_num = cur_line_num
    except:
        print "END"

# entrance
if __name__ == "__main__":
    Main(ArgParse())

