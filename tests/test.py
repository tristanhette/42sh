import os
import yaml
import subprocess

from termcolor import colored

tests_files = [f.split('.')[0] for f in os.listdir("tests") if os.path.isfile(os.path.join("tests", f))]
PATH_42SH = "./../src/42sh"
PATH_OUTPUT = ".output.out"
PATH_ERR = ".output.err"
stderr_name = "stderr/stderr.txt"
stdout_name = "stderr/stdout.txt"
nb_test = 0
success = 0

def make():
    # Execute the 'make' command
    if not os.path.exists("stderr"):
        os.makedirs(directory_path)
    with open(stdout_name, 'w+') as output_file:
        os.chdir('..')
        subprocess.run('./make.sh', shell=True, stdout=output_file, stderr=output_file, text=True)

    os.chdir('tests')


def clear():
    if not os.path.exists("stderr"):
        os.makedirs(directory_path)
    with open(stdout_name, 'w+') as output_file:
        os.chdir('..')
        subprocess.run('./clear.sh', shell=True, stdout=output_file, text=True)

def run_test(test, i, tests_length):

    branch = "├── " if i < tests_length - 1 else "└── "
    test_command = test["command"] if "command" in test else None
    test_script = test["script"] if "script" in test else None

    command = PATH_42SH
    if (test_command):
        command += " -c " + f"\'{test_command}\'"
    else:
        command += f" \'scripts/{test_script}\'"

    expected_status = test["status"]
    expected_output = test["output"].strip()

    output = ""
    suc = 0
    try:
        with open(PATH_ERR, "w+") as file_err:
            with open(PATH_OUTPUT, "w+") as file_out:
                actual_status = subprocess.run(command, shell=True, stdout=file_out, stderr=file_err).returncode

        with open(PATH_OUTPUT, "r") as file_out:
            actual_output = file_out.read().strip()

        with open(PATH_ERR, "r") as file_err:
            actual_err = file_err.read().strip()

        if actual_status == expected_status:
            if actual_output == expected_output:
                if actual_status == 0 and actual_err != "":
                    output, suc = branch + colored("✗ ", "red") + test["name"] + colored(f"  Expecting nothing on stderr, got: {actual_err}", "red"), 0
                elif actual_status != 0 and actual_err == "" and test["name"].find("Exit") == -1:
                    output, suc = branch + colored("✗ ", "red") + test["name"] + colored(f"  Expecting something on stderr, got nothing.", "red"), 0
                else:
                    output, suc = branch + colored("✓ ", "green") + test["name"], 1
            else:
                output, suc = branch + colored("✗ ", "red") + test["name"] + colored(f"  Expected: {expected_output}, got: {actual_output}", "red"), 0
        else:
            output, suc = branch + colored("✗ ", "red") + test["name"] + colored(f"  Expected: {expected_status}, got: {actual_status}, err: {actual_err}", "red"), 0

    except Exception as e:
        print(f"Error executing test '{test['name']}': {e}")

    return output, suc

def run_tests():
    global tests_files, nb_test, success

    buff = ""
    for tests in tests_files:
        with open("tests/" + tests + ".yml", "r") as file:
            tests_data = yaml.safe_load(file)["tests"]

        output = []
        test_success = 0
        test_nb_tests = 0
        for i,test in enumerate(tests_data):
            out, suc = run_test(test, i, len(tests_data))
            output.append(out)
            test_nb_tests += 1
            test_success += suc

        nb_test += test_nb_tests
        success += test_success

        if (test_nb_tests == test_success):
            buff += colored("✓ ", "green") + colored(f"{tests}", "yellow") + "\n"
        else:
            buff += colored("✗ ", "red") + colored(f"{tests}", "yellow") + "\n"
        buff += "\n".join(output) + "\n\n"


    ratio = success / nb_test * 100
    length = len("{:.2f}%".format(ratio)) + 21
    print("┌" + "─" * length + "┐")
    if not success == nb_test:
        print("│   CHECK RESULT : " + colored("{:.2f}%".format(ratio), "yellow") + "   │")
    else:
        print("│   CHECK RESULT : " + colored("{:.2f}%".format(ratio), "green") + "   │")
    print("└" + "─" * length + "┘" + "\n")

    print(buff)

if __name__ == "__main__":
    #make()
    run_tests()
    #clear()
