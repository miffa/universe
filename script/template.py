#!/home/search/ttool/py27/bin/python

# func:

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
'''
import subprocess
import time
import datetime
import re
'''

# parse command line arguments
def ArgParse():
    parser = argparse.ArgumentParser(
        description = "Compute qps by monitoring a access log file",
        epilog = "Author: unix1986 Homepage: https://github.com/unix1986")
    '''
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
    '''
    return parser.parse_args()

# main func
def Main(args):
    pass

# entrance
if __name__ == "__main__":
    Main(ArgParse())

