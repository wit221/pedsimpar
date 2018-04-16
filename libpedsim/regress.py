#!/usr/bin/env python2

import difflib
import subprocess

subprocess.check_output(["make", "regress"])
subprocess.check_output(["./regress"])

output = open("./pedsim_out.txt").read().strip().splitlines()
ref = open("./regress_src/pedsim_out_ref.txt").read().strip().splitlines()

if output != ref:
    print("MISMATCH:")
    for line in difflib.unified_diff(output, ref, lineterm=''):
        print(line)
else:
    print("Passed.")

subprocess.check_output(["rm", "./pedsim_out.txt"])
