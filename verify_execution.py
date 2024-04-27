import os
import re
import json

operations_log_path = "/home/snapshots/absolute_paths.txt"
json_tree_path = "/home/snapshots/directory_tree.json"
operations_txt_path = "./operations.txt"

# READ = 0,
# WRITE = 1,
# APPEND = 2
op_type = {
    '0' : 'R',
    '1' : 'W',
    '2' : 'A'
}

per_file_operations_logged = dict()
per_file_operations_executed = dict()
# Because the write performed is random (APPEND/WRITE) in this dictionary we store all write operations as the same
read_writes = dict()

# This function is used to test the operations that where executed (operations.txt) with the operations recorded
# for each file (absolute_paths.txt). If those are the same that means that our file system preserved a log for
# every operation executed in a file.
#
# The results are printed in a json file named "recorded_vs_performed.json"
# 
def verify_operations_log():
    json_output = dict()


    operations_logged = []
    with open(operations_log_path,'r') as f:
        operations_logged = f.readlines()    
        operations_logged = [line.replace('\n', '') for line in operations_logged]

    f.close()

    for line in operations_logged:
        log = line.split('|')
        
        file_name = log[0]
        operation = log[1]

        if file_name not in per_file_operations_logged.keys():
            per_file_operations_logged[file_name] = list()
        
        per_file_operations_logged[file_name].append(op_type[operation])




        if file_name not in read_writes.keys():
            read_writes[file_name] = list()
            
        op = ''
        if operation == '1' or operation == '2':
            read_writes[file_name].append('W')
        else:
            read_writes[file_name].append('R')
        



    operations_performed = []
    with open(operations_txt_path,'r') as f:
        operations_performed = f.readlines()    
        operations_performed = [line.replace('\n', '') for line in operations_performed]


    for line in operations_performed:
        action = line.split('|')
        operation_flag = action[0]
        file = ''
        if(len(action) == 2):
            # Read operation
            file = action[1]
            file = os.path.abspath(file)
        elif (len(action) == 3):
            # Write operation
            file = action[2]
            file = os.path.abspath(file)
        else:
            print("Unknown operation format "+str(line))

        if (len(file) != 0):
            # per_file_operations_executed
            if file not in per_file_operations_executed.keys():
                per_file_operations_executed[file] = list()
            
            if operation_flag == 'WRITE':
                per_file_operations_executed[file].append('W')
            else:
                per_file_operations_executed[file].append('R')
    f.close()


    # Now we check if the logged read and writes are the same as the ones executed
    # We compare the per_file_operations_executed with the read_writes dictionaries
    # if they are the same then it means that we have successfully recorder the 
    # operations that where performed.

    # 1st check the keys (the filenames):
    executed_list = sorted(list(per_file_operations_executed.keys()))
    recorded_list = sorted(list(read_writes.keys()))

    if (set(executed_list) == set(recorded_list)):
        print("File names where operations where performed and file names recorded are the same")

        for file_name in executed_list:
            performed_ops = sorted(per_file_operations_executed[file_name])
            recorded_ops = sorted(read_writes[file_name])
            if(set(performed_ops) == set(recorded_ops)):
                print("Operations performed and recorded on file : {"+ file_name+"} are the same")
            else:
                print("Inconsistency on operations performed and recorded on file : {"+ file_name+"}")

            if file_name not in json_output.keys():
                json_output[file_name] = {
                    "RECORDED OPERATIONS" : str(recorded_ops),
                    "PERFORMED OPERATIONS" : str(performed_ops)
                }

    else:
        print("Inconsistency on recorded and executed file names")


    with open('./recorded_vs_performed_operations.json','w') as f:
        f.write(json.dumps(json_output,indent=4))
        f.close()





# This function will check if the operations that are logged (with respect to difference between a WRITE and an APPEND)
# operation are also preserved in the final directory tree. If so we have managed to also preserve the overall system 
# state correctly.
def verify_json_tree_consistency(json_tree_path):
    operations_dict = {}
    json_output = dict()


    with open(json_tree_path, 'r') as f:
        data = json.load(f)

    def extract_operations(node, parent_path=''):
        if "files" in node:
            for file in node["files"]:
                file_path = file["absolute_path"]
                operations_dict[file_path] = file["operations"]

        if "children" in node:
            for child in node["children"]:
                child_path = os.path.join(parent_path, child["dir_name"])
                extract_operations(child, child_path)

    extract_operations(data)


    # First check that the paths match in both files:
    json_tree_paths = sorted(list(operations_dict.keys()))
    logged_paths = sorted(list(per_file_operations_logged.keys()))

    if (set(json_tree_paths) == set(logged_paths)):
        # print("Same")
        for file_name in operations_dict.keys():
            json_ops = sorted(operations_dict[file_name])
            rec_ops = sorted(per_file_operations_logged[file_name])
            print("==================================================================================================")
            print(file_name)
            print(json_ops)
            print(rec_ops)
            print("==================================================================================================")
            if(set(json_ops) == set(rec_ops)):
                print("Operations performed and logged for file : {"+ file_name+"} are the same")
            else:
                print("Incosistency on recorded paths on json tree and execution log for file: {"+file_name+"}")
        
            if file_name not in json_output.keys():
                json_output[file_name] = {
                    "RECORDED OPERATIONS JSON" : str(json_ops),
                    "RECORDED OPERATIONS LOG" : str(rec_ops)
                }
    else:
        print("Inconsistency on recorded paths on json tree and execution log")



    with open('./json_vs_log_operations.json','w') as f:
        f.write(json.dumps(json_output,indent=4))
        f.close()





    return operations_dict

verify_operations_log()




temp =verify_json_tree_consistency(json_tree_path)





# print(per_file_operations_logged)