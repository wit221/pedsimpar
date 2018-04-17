#!/usr/bin/env python2
import os
from os import walk

import numpy as np
import difflib
import subprocess
import datetime
import argparse

#set paths
bench_src_path = os.path.abspath('benchmarks')
bench_cache_path = os.path.abspath('bench_cache')
libpedsim_path = os.path.abspath('libpedsim')
libpedsim_path = os.path.abspath('libpedsimpar')

def bench(args):
    #make dirs
    if not os.path.exists(bench_cache_path):
        os.makedirs(bench_cache_path)

    #make libraries
    print("[.] making libraries")
    subprocess.check_output(["make"], cwd="./libpedsim")
    subprocess.check_output(["make"], cwd="./libpedsimpar")

    #go to benchmarks directory
    #get list of all benchmarks
    bench_files = []
    for (dirpath, dirnames, filenames) in walk(bench_src_path):
        bench_files.extend(filenames)
        break
    bench_files = sorted(bench_files)

    print('[.] making bench files')
    #compile every benchmark file inside
    for bench_file in bench_files:
        # linked against libpedsim
        src = os.path.join(bench_src_path, bench_file)
        out = os.path.join(bench_cache_path, bench_file.split('.')[0])

        cmd = 'g++ {} -o {} -Ilibpedsim -lpedsim -Llibpedsim -g -std=c++0x'.format(src, out)
        subprocess.check_output(cmd, shell=True)
        #linked against libpedsimpar
        cmd = 'g++ {} -o {} -Ilibpedsimpar -lpedsimpar -Llibpedsimpar -g -std=c++0x'.format(src, out+'par')
        subprocess.check_output(cmd, shell=True)

    print('[.] running benchmarks')
    # for every benchmark file:cd
    seq_res = np.array([])
    par_res = np.array([])

    for bench_file in bench_files:
        #run and time sequential
        out_msg = [bench_file+ ': [seq, par, speedup]']
        tstart = datetime.datetime.now()
        cmd = "LD_LIBRARY_PATH=./libpedsim/:$LD_LIBRARY_PATH ./bench_cache/{}".format(bench_file.split('.')[0])
        subprocess.check_output(cmd, shell=True)
        delta = datetime.datetime.now() - tstart
        secs = delta.seconds + 24 * 3600 * delta.days + 1e-6 * delta.microseconds
        out_msg.append("%.2f" % secs)
        seq_res = np.append(seq_res, secs)

        #run and time parallel
        tstart = datetime.datetime.now()
        cmd = "LD_LIBRARY_PATH=./libpedsimpar/:$LD_LIBRARY_PATH ./bench_cache/{}".format(bench_file.split('.')[0]+'par')
        subprocess.check_output(cmd, shell=True)
        delta = datetime.datetime.now() - tstart
        secs = delta.seconds + 24    * 3600 * delta.days + 1e-6 * delta.microseconds
        out_msg.append("%.2f" % secs)
        par_res = np.append(par_res, secs)
        out_msg.append("%.2f" % (seq_res[-1]/par_res[-1]))

        #print results
        print("\t".join(out_msg))

    #print summary
    out_msg = ['Summary: [mean seq, mean par, overll speedup]']
    out_msg.append("%.2f" % np.mean(seq_res))
    out_msg.append("%.2f" % np.mean(par_res))
    out_msg.append("%.2f" % np.mean(np.sum(seq_res)/np.sum(par_res)))
    print("\t".join(out_msg))

if __name__ == '__main__':
    bench(None)
