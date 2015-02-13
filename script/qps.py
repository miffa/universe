#!/bin/env python

# common qps statistics tool based on log

__author__ = "unix1986@qq.com"

import sys
import platform

# check python version
if sys.version_info < (2, 7, 0):
    print ("Current python version %s, " 
           "but require version %s or above") % \
			(platform.python_version(), "2.7.0")
    sys.exit(1)

import os
import argparse
import subprocess
import time
import datetime
import re

# parse command line arguments
def ArgParse():
    parser = argparse.ArgumentParser(
        description = "Compute qps by monitoring a access log file",
        epilog = "Author: unix1986 Homepage: https://github.com/unix1986")
    parser.add_argument(
        "logfile",
        help = "for example: /path/access.qfedd.log")
    parser.add_argument(
        "-i", "--interval", type = int,
        help = "interval time (s) for statistics")
    parser.add_argument(
        "-e", "--expr",
        help = "regular expression for getting special lines")
    parser.add_argument(
        "-r", "--reverse", action = "store_true",
        help = "reverse regular expression for filtering special lines, must be used with -e")
    parser.add_argument(
        "-f", "--flush", action = "store_true",
        help = "flush file buffer")
    return parser.parse_args()

# filter
def FilterLineNum(lines, reobj):
    if not reobj:
        return 0
    return len(filter(reobj.search, lines.split("\n")))

# main func
def Main(args):
    try:
        interval = 30
        if args.interval and args.interval > 0:
            interval = args.interval
        reobj = None
        if args.expr:
            reobj = re.compile(args.expr)
        fobj = open(args.logfile, 'r+b')
        beg_size = os.stat(args.logfile).st_size
        fobj.seek(beg_size)
        elapsed_time = 0
        while True:
            time.sleep(interval)
            end_size = os.stat(args.logfile).st_size
            beg_time = datetime.datetime.now()
            append_content = fobj.read(end_size - beg_size)
            results_num = append_content.count("\n")
            filter_num = FilterLineNum(append_content, reobj)
            if args.reverse:
                results_num -= filter_num
            elif reobj:
                results_num = filter_num
            print "[" + time.strftime("%m-%d %H:%M:%S", time.localtime(time.time())) + "] ",
            print "QPS: %d q/s" % int(float(results_num) / (interval * 1000 + elapsed_time) * 1000)
            if args.flush:
                sys.stdout.flush()
            beg_size = end_size
            time_delta = datetime.datetime.now() - beg_time
            elapsed_time = time_delta.seconds * 1000 + time_delta.microseconds / 1000
    except IOError, e:
        print "open [%s] error: %s" % (args.logfile, e)
        sys.exit(1)
    except:
        print "END"
    finally:
        if fobj:
            fobj.close()

# entrance
if __name__ == "__main__":
    Main(ArgParse())

