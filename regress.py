#!/usr/bin/env python2

import difflib
import subprocess
import time

print("[.] making libraries")
subprocess.check_output(["make"], cwd="./libpedsim")
subprocess.check_output(["make"], cwd="./libpedsimpar")
print("[.] making test executables")
subprocess.check_output("g++ ./regress/regress.cpp -o regress/regress_seq -Ilibpedsim -lpedsim -Llibpedsim -g -std=c++0x", shell=True)
subprocess.check_output("g++ ./regress/regress.cpp -o regress/regress_par -Ilibpedsimpar -lpedsimpar -Llibpedsimpar -g -std=c++0x", shell=True)
print("[.] running sequential")
start = time.time()
subprocess.check_output("LD_LIBRARY_PATH=./libpedsim/:${LD_LIBRARY_PATH} ./regress/regress_seq", shell=True)
subprocess.check_output("mv ./pedsim_out.txt ./pedsim_out_seq.txt", shell=True)
end = time.time()
seqtime = end-start
print("Sequential: {} ms".format(seqtime*1000.0))
print("[.] running parallel")
start = time.time()
subprocess.check_output("LD_LIBRARY_PATH=./libpedsimpar/:${LD_LIBRARY_PATH} ./regress/regress_par", shell=True)
subprocess.check_output("mv ./pedsim_out.txt ./pedsim_out_par.txt", shell=True)
end = time.time()
partime = end-start
print("Parallel:   {} ms".format(partime*1000.0))
print("[.] checking output")

outputSeq = open("./pedsim_out_seq.txt").read().strip().splitlines()
outputPar = open("./pedsim_out_par.txt").read().strip().splitlines()

if outputSeq != outputPar:
    print("[-] MISMATCH:")
    for line in difflib.unified_diff(outputSeq, outputPar, lineterm=''):
        print(line)
else:
    print("[+] Passed.")

subprocess.check_output(["rm", "./pedsim_out_seq.txt"])
subprocess.check_output(["rm", "./pedsim_out_par.txt"])
