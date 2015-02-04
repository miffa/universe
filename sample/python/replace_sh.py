#!/usr/bin/env python
# replace shell command using python

from subprocess import call
from subprocess import Popen
from subprocess import PIPE

import shlex

call("ls -al replace_sh.py", shell = True) # call shell command
call(['ls', '-al', 'replace_sh.py']) # as above
call("echo replace_sh.py | sed 's/replace/cmd/g'", shell = True)
call(shlex.split("ls -al replace_sh.py")) # use shlex module

# deal with output, errput, returncode
p = Popen("ls -al ./*", stdout = PIPE, stderr = PIPE, shell = True)
output, errput = p.communicate()
print "output: %s" % output[:-1] # [:-1] slice, remove \n
print "errput: %s" % errput[:-1]
print "returncode: %d" % p.returncode
