#!/usr/bin/python

# Do a head/tail check between specified number of files.

import os
import sys

if len(sys.argv) != 2:
  print "Usage is: fileVerify <number of files generated>\n"
else:
  for i in range(0, int(sys.argv[1])-1):
    headcmd = "head -5 " + str(i+1) + "output.txt"
    tailcmd = "tail -5 " + str(i) + "output.txt"
    print "TAIL file: " + str(i) + "output.txt"
    os.system(tailcmd)
    print "\n"
    print "HEAD file: " + str(i+1) + "output.txt"
    os.system(headcmd)
    print "\n"
