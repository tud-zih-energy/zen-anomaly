#!/usr/bin/env python3


#run module load metricq before starting

import subprocess, os, csv, re
import time

my_env = os.environ
my_env["LANG"] = "en_US.UTF-8"
my_env["LC_ALL"] = "en_US.UTF-8"

header = ["threads", "power", "execution_time", "mean_cycles", "mean_mperf", "mean_aperf", "main_voltage"]
raw_header = ["threads",  "core", "cycles", "mperf", "aperf", "core_voltage", "execution_time"]

filename = str(int(time.time()))
raw_report_file = open(filename + "_raw.csv", "w")
raw_64_report_file = open(filename + "_raw64.csv", "w");
report_file = open(filename +  ".csv", 'w')
output_file = open(filename + ".stdout", 'w')

def run(command):
    output = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=my_env, encoding='utf-8')
    output_file.write(output.stdout)
    return output

report = csv.writer(report_file)
raw_64_report = csv.writer(raw_64_report_file);
raw_report = csv.writer(raw_report_file)

report.writerow(header)
raw_report.writerow(raw_header)
raw_64_report.writerow(raw_header)

def mean_over_threads(values, threads):
    mean = 0
    for i in values[0:threads] + values[32:32+threads]:
        mean += i
    return mean/threads
run("elab frequency turbo")

for threads in range(1, 33):
    print("executing on : ", threads)
    run("elab cstate enable")
    if threads != 32:
        cpus = ','.join(str(e) + ',' + str(e+32) for e in range(threads, 32))
        print(cpus);
        run(["elab cstate disable --cpus " + cpus + " C1 POLL"])
    for i in range(10):
        print("Iteration: ", i)
        run("elab summary")
        output = run(["metricq-summary --server=\"amqps://rabbitmq.metricq.zih.tu-dresden.de\" -m elab.charon.power -- perf stat -a -A -e cycles,mperf,aperf ./powerread ./compute " + ' '.join(str(e) for e in range (0, threads))])
        #Texas Regex Massacre
        adapt_voltage = re.findall(r'Whole: ([0-9.]+)', output.stdout, re.MULTILINE);
        print(adapt_voltage)
        core_voltage = re.findall(r'PerCpuVolt: ([0-9.]+)', output.stdout, re.MULTILINE);
        power = re.findall(r'"elab.charon.power",[^,]+,[^,]+,([0-9.]+)', output.stdout, re.MULTILINE)
        exec_time = re.findall(r'([0-9.]+) seconds time elapsed', output.stdout)[0]
        cycles= [float(x.replace(",", "")) for x in re.findall(r'([0-9,]+)\s+cycles', output.stdout)]
        mperf= [float(x.replace(",", "")) for x in re.findall(r'([0-9,]+)\s+mperf', output.stdout)]
        aperf = [float(x.replace(",", "")) for x in re.findall(r'([0-9,]+)\s+aperf', output.stdout)]

        report.writerow([threads] + power + [exec_time] + [mean_over_threads(cycles, threads)] + [mean_over_threads(mperf, threads)] + [mean_over_threads(aperf, threads)] + adapt_voltage)
        for i in range(32):
            raw_report.writerow([threads, i] + [cycles[i] + cycles[i+32]] + [mperf[i] + mperf[i+32]] + [aperf[i] + aperf[i+32]] + [core_voltage[i]] + [exec_time])
        for i in range(64):
            raw_64_report.writerow([threads, i] + [cycles[i]] + [mperf[i]] + [aperf[i]] + [core_voltage[i % 32]] + [exec_time])
raw_report_file.close()
raw_64_report_file.close()
report_file.close()
output_file.close()
