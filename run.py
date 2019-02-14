#!/usr/bin/python
import sys
import os

cmd1 = "qsub BlackPhoenix.qsub"
cmd2 = "watch qstat"

os.system(cmd1)
os.system(cmd2)
