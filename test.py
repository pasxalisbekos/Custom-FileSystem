import subprocess
import threading
from random import randint
from time import sleep


# Define a lock for synchronization
lock = threading.Lock()

# Define a function to execute a command
def execute_command(command):
    try:
        subprocess.run(command.split(), check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {command}")
        print(e)

# Define a function to execute commands in multiple threads
def execute_commands(commands):
    threads = []
    for command in commands:
        thread = threading.Thread(target=execute_command, args=(command,))
        threads.append(thread)
        thread.start()

    # Wait for all threads to complete
    for thread in threads:
        thread.join()

# Read commands from the file
with open('operations.txt', 'r') as file:
    commands = file.readlines()
    for i in range(len(commands)):
        commands[i] = "./exec " + commands[i].replace('|',' ').replace('\n','')
        # print(commands[i])


# exit()        
# Strip newline characters from the commands
commands = [cmd.strip() for cmd in commands]

# Execute commands in multiple concurrent threads
def execute_commands_with_timeout(commands, timeout):
    threads = []
    for command in commands:
        temp = command.split(' ')
        if (temp[1] == 'READ'):
            # sleep(randint(0,6))
            thread = threading.Thread(target=execute_command, args=(command+" "+str(randint(1,10)),))
            threads.append(thread)
            thread.start()
        else:
            # sleep(randint(0,6))
            thread = threading.Thread(target=execute_command, args=(command+" random_value:"+str(randint(0,10))+" "+str(randint(0,1))+" "+str(randint(1,10)),))
            threads.append(thread)
            thread.start()
        pass

    # Wait for all threads to complete or until timeout is reached
    for thread in threads:
        thread.join(timeout)

# Set a timeout value in seconds for acquiring the lock
lock_timeout_seconds = 5

# Set a timeout value in seconds for executing commands
execute_timeout_seconds = 10

# Acquire lock with timeout
if lock.acquire(timeout=lock_timeout_seconds):
    try:
        # Execute commands with timeout
        execute_commands_with_timeout(commands, execute_timeout_seconds)
    finally:
        # Release the lock
        lock.release()
else:
    print("Failed to acquire lock within timeout.")
