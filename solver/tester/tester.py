import glob
import os
import sys
import time
import json
from collections import OrderedDict
import numpy as np
from tqdm import tqdm
import subprocess



def output_result(time_data, ops_data, test_cases):
  print("\nTime:")
  print("  Average Time: %6.3f[ms]"%(sum(time_data) / len(time_data)), f"({len(test_cases)} cases)")
  print("  Maximum Time: %6.3f[ms]"%(max(time_data)), f"(case: {test_cases[np.argmax(time_data)]})")
  print("  Minimum Time: %6.3f[ms]"%(min(time_data)), f"(case: {test_cases[np.argmin(time_data)]})")
  print("\nOperations:")
  print("  Average Ops: %5d"%(sum(ops_data) / len(ops_data)), f"({len(test_cases)} cases)")
  print("  Maximum Ops: %5d"%(max(ops_data)), f"(case: {test_cases[np.argmax(ops_data)]})")
  print("  Minimum Ops: %5d"%(min(ops_data)), f"(case: {test_cases[np.argmin(ops_data)]})")


def create_input_data(data) -> str:
  result = "%d %d\n"%(data["H"], data["W"])
  result += "\n".join(data["start"]) + "\n"
  result += "\n".join(data["goal"]) + "\n"
  return result


def run_test(test_cases, cpp_dir):
  print("Start test")

  time_data, ops_data = [], []
  total_time = 0.0 # [ms]
  count = 0

  with tqdm(test_cases) as filenames:
    for filename in filenames:
      with open(filename, "r") as f:
        input_data = create_input_data(json.load(f))

        start_time = time.perf_counter()

        # run
        solver_process = subprocess.run(solver_path, input=input_data, text=True, capture_output=True)
        output = solver_process.stdout

        elapsed_time = (time.perf_counter() - start_time) * 1000
        
        # check
        ops = int(output.splitlines()[0])
        tester_input = input_data + output
        tester_process = subprocess.run(cpp_dir + "/tester", input=tester_input, text=True, capture_output=True)
        result = tester_process.stdout

        total_time += elapsed_time
        count += 1
        time_data.append(elapsed_time)
        ops_data.append(ops)
        filenames.set_postfix(OrderedDict(avg="%6.3fms"%(total_time / count)))

        if result == "OK\n": continue

        print("NG")
        print(f"testcase {filename} failed.")
        exit()

  output_result(time_data, ops_data, test_cases)



if __name__ == "__main__":
  if len(sys.argv) < 2:
    print("Argument Error: python3 tester.py <solver program>")
    exit()

  source_dir = os.path.dirname(os.path.abspath(__file__))
  cpp_dir = source_dir + "/cpp"
  running_dir = os.getcwd()
  solver_path = os.path.join(running_dir, sys.argv[1])

  if not os.path.isfile(solver_path):
    print("solver program was not found")
    exit()

  test_cases = glob.glob(source_dir + "/output/*-*--*-in.json")
  if len(sys.argv) >= 4:
    test_cases = glob.glob(f"{source_dir}/output/{sys.argv[2]}-{sys.argv[3]}--*-in.json")

  if len(test_cases) == 0:
    print("number of test cases is 0")
    exit()

  # compile tester
  if not os.path.exists(cpp_dir+"/tester"):
    subprocess.run(["g++", cpp_dir+"/tester.cpp", "-O2", "-o", cpp_dir+"/tester"])

  run_test(test_cases, cpp_dir)
