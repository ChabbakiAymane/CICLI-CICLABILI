import subprocess
import sys
import os
import os.path
import time
import platform
import timeit

if(len(sys.argv) < 2):
    print("\nUsage:  python test.py <eseguibile> \n\n1) <eseguibile> deve trovarsi nella stessa cartella di test.py\n2) Il dataset deve trovarsi nella stessa cartella di test.py, chiamarsi \"cicli\"\n   e contenere due cartelle: \"input\" e \"output\"")
    sys.exit(0)

exe_path = sys.argv[1]
dataset_folder = "cicli1"
len_dataset = len([name for name in os.listdir(f"./{dataset_folder}/output/")])

system = platform.system()
if(system == "Windows"):
    command = f"{exe_path}"
else:
    command = f"./{exe_path}"


outputs = []
punti = 0

tic = timeit.default_timer()

for i in range(len_dataset):
    with open(f"{dataset_folder}/output/output{i}.txt", "r") as f:
        outputs.append(f.read())

for i in range(len_dataset):
    with open(f"{dataset_folder}/input/input{i}.txt", "r") as f:
        input1 = f.read()

    with open("input.txt", "w") as f:
        f.write(input1)

    print(f"\n[*] Test {i}: ")

    timeout = False
    try:
        proc = subprocess.run([command], timeout=2)
    except subprocess.TimeoutExpired:
        timeout = True

    if timeout:
        print("Fuori tempo massimo")
    else:
        with open("output.txt", "r") as f:
            if(f.read() == outputs[i]):
                punti += 5
                print("Corretto")
            else:
                print("Non corretto")

toc = timeit.default_timer()

print(f"{punti}/100")
print(toc - tic)
