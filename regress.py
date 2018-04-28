#!/usr/bin/env python2

import difflib
import subprocess
import time
import argparse
import sys
import os

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('-n', type=int, metavar="NUMAGENTS", nargs='?', default=100, help='Number of agents (0 < n <= 1000)')
parser.add_argument('-t', type=int, metavar="MAXTIME", nargs='?', default=20000, help='Maximum number of time steps')
parser.add_argument('-q', action='store_true', help='Use quadtree')
args = parser.parse_args()

if args.n <= 0 or args.n > 1000:
    print("[-] Invalid number of agents (0 < n <= 1000)")
    sys.exit(0)

print("[.] making libraries")
subprocess.check_output(["make"], cwd="./libpedsim")
subprocess.check_output(["make"], cwd="./libpedsimpar")

print("[.] making test executables")
subprocess.check_output("g++ ./regress/regress.cpp -o regress/regress_seq -Ilibpedsim -lpedsim -Llibpedsim -g -std=c++0x", shell=True)
if os.path.exists("/opt/cuda-8.0/"):
    cudalib = "-L/opt/cuda-8.0/lib64/ -lcudart"
else:
    cudalib = "-L/usr/local/depot/cuda-8.0/lib64/ -lcudart"
subprocess.check_output("g++ ./regress/regress.cpp -o regress/regress_par -Ilibpedsimpar -lpedsimpar -Llibpedsimpar {} -g -std=c++0x".format(cudalib), shell=True)

print("[.] running sequential")
start = time.time()
subprocess.check_output("LD_LIBRARY_PATH=./libpedsim/:${{LD_LIBRARY_PATH}} ./regress/regress_seq -n {} -t {} {}".format(args.n, args.t, "-q" if args.q else ""), shell=True)
subprocess.check_output("mv ./pedsim_out.txt ./pedsim_out_seq.txt", shell=True)
end = time.time()
seqtime = end-start
print("Sequential: {} ms".format(seqtime*1000.0))

print("[.] running parallel")
start = time.time()
subprocess.check_output("LD_LIBRARY_PATH=./libpedsimpar/:${{LD_LIBRARY_PATH}} ./regress/regress_par -n {} -t {} {}".format(args.n, args.t, "-q" if args.q else ""), shell=True)
subprocess.check_output("mv ./pedsim_out.txt ./pedsim_out_par.txt", shell=True)
end = time.time()
partime = end-start
print("Parallel:   {} ms".format(partime*1000.0))
print("Speedup: {}".format(seqtime/partime))

print("[.] checking output")

outputSeq = open("./pedsim_out_seq.txt").read().strip().splitlines()
outputPar = open("./pedsim_out_par.txt").read().strip().splitlines()

if outputSeq != outputPar:
    print("[-] MISMATCH:")
    for line in difflib.unified_diff(outputSeq, outputPar, lineterm=''):
        print(line)
else:
    print("[+] Passed.")
