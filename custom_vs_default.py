import os
import re
import sys
import json
import matplotlib.pyplot as plt
import numpy as np

custom_path = './htop_custom.txt'
default_path = './htop_default.txt'

def get_custom():
    lines = []
    custom_results = []
    with open(custom_path,'r') as f:
        lines = f.readlines()
        for line in lines:
            if 'demo' in line:
                
                custom_results.append(line.replace('\n',''))

    f.close()
    return custom_results

def get_default():
    lines = []
    default_results = []
    with open(default_path,'r') as f:
        lines = f.readlines()
        for line in lines:
            if 'demo' in line:
                default_results.append(line.replace('\n',''))


    f.close()
    return default_results

def get_diffs_sequential():
    custom = get_custom()
    default = get_default()

    custom_results = []
    for line in custom:
        line = line.split(' ')
        line = [string for string in line if string != ""]
        temp = {
            "PID": line[0],
            "USER":line[1],
            "PRIORITY":line[2],
            "NICE VAL":line[3],
            "VIRT MEM":line[4],
            "RES MEM":line[5],
            "SHARED MEM":line[6],
            "STATUS":line[7],
            "%CPU":line[8],
            "%MEM":line[9],
            "CPU TIME":line[10],
        }

        custom_results.append(temp)

    default_results = []
    for line in default:
        line = line.split(' ')
        line = [string for string in line if string != ""]
        temp = {
            "PID": line[0],
            "USER":line[1],
            "PRIORITY":line[2],
            "NICE VAL":line[3],
            "VIRT MEM":line[4],
            "RES MEM":line[5],
            "SHARED MEM":line[6],
            "STATUS":line[7],
            "%CPU":line[8],
            "%MEM":line[9],
            "CPU TIME":line[10],
        }

        default_results.append(temp)

    print("============================================================================================================")
    print("CUSTOM IMPLEMENTATION CPU UTILIZATION:")
    for item in custom_results:
        print("> "+item["CPU TIME"])
    print("============================================================================================================")
    print("DEFAULT R/W CPU UTILIZATION:")
    for item in default_results:
        print("> "+item["CPU TIME"])
    print("============================================================================================================")
    # Plot the memory usage
    x_labels = ['{} secs'.format((i+1) * 30) for i in range(len(custom_results))]
    res_mem_process1 = [int(entry['RES MEM']) for entry in custom_results]
    shared_mem_process1 = [int(entry['SHARED MEM']) for entry in custom_results]
    cpu_time_process1 = [float(entry['CPU TIME'].split(':')[1]) for entry in custom_results]

    res_mem_process2 = [int(entry['RES MEM']) for entry in default_results]
    shared_mem_process2 = [int(entry['SHARED MEM']) for entry in default_results]
    cpu_time_process2 = [float(entry['CPU TIME'].split(':')[1]) for entry in default_results]


    fig, ax = plt.subplots()

    bar_width = 0.2

    r1 = np.arange(len(x_labels))
    r2 = [x + bar_width for x in r1]
    r3 = [x + bar_width * 2 for x in r1]
    r4 = [x + bar_width * 3 for x in r1]

    ax.bar(r1, res_mem_process1, color='b', width=bar_width, edgecolor='grey', label='RES MEM - Custom File Sys')
    ax.bar(r2, shared_mem_process1, color='lightblue', width=bar_width, edgecolor='grey', label='SHARED MEM - Custom File Sys')
    ax.bar(r3, res_mem_process2, color='r', width=bar_width, edgecolor='grey', label='RES MEM - Default R/W')
    ax.bar(r4, shared_mem_process2, color='salmon', width=bar_width, edgecolor='grey', label='SHARED MEM - Default R/W')

    ax.set_xlabel('Execution time', fontweight='bold')
    ax.set_ylabel('KB of usage', fontweight='bold')
    ax.set_xticks([r + 1.5 * bar_width for r in range(len(x_labels))])
    ax.set_xticklabels(x_labels, rotation=45, ha='right')

    ax.legend()
    plt.tight_layout()
    plt.show()



get_diffs_sequential()