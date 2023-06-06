#!/bin/python3

import os
import atexit
from colorama import Fore, Style

def run_test(program):
    ret = os.system(program)
    if ret == 0:
        print(Fore.GREEN + f"test {program} run successfully" + Style.RESET_ALL)
        return (1,1)
    else:
        exitcode = ret >> 8
        signal = ret & 0xff
        print(Fore.RED + f"test {program} FAILED with exit code {exitcode} signal {signal}" + Style.RESET_ALL)
        return (0,1)

def run_test_if_exists(program):
    if os.path.isfile(program):
        print(f"found test {program}")
        return run_test(program)
    print(Fore.YELLOW + f"did not find test {program}" + Style.RESET_ALL)
    return (0,0)

results = []

results += [run_test_if_exists("./../build/test/test_e2ap_encode_asn")]
results += [run_test_if_exists("./../build/test/test_e2ap_enc_dec_asn")]
results += [run_test_if_exists("./../build/test/test_e2ap_enc_dec_fb")]
results += [run_test_if_exists("./../build/test/test_near_ric")]
results += [run_test_if_exists("./../build/src/sm/kpm_sm_v2.02/test/test_kpm_sm")]

# sums elementwise (x,y) in results
result = tuple(map(sum, zip(*results)))
if result[0] == result[1]:
    print(Fore.GREEN, end="")
else:
    print(Fore.RED, end="")
print(f"result: {result[0]} out of {result[1]} tests ran successfully" + Style.RESET_ALL)
