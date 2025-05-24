import re
import os
from termcolor import colored


DIRECTORY_PATH = '../src'


def analyze_c_code(file_path):

    with open(file_path, 'r') as file:
        c_code = file.read()

    inside_function = False
    function_line_count = 0

    buf = ""
    previous = ""
    function_name = ""
    long_functions = []
    nb_function = 0
    succ = 0

    for line in c_code.split('\n'):
        if len(line) > 5 and line[0].isalpha() and (line[-1] == ')' or line[-1] == ';' or line[-1] == ','):
            function_name = line
            if file_path[-1] == 'h' and line[-1] == ';':
                if line[-3] == '(':
                    long_functions.append([function_name, colored("Must specify void if no argument.", "red")])
                    succ = 1
                    nb_function += 1
            elif line[-1] == ')':
                if (line[-2] == '('):
                    long_functions.append([function_name, colored("Must specify void if no argument.", "red")])
                    succ = 1
                if not function_name.startswith("static"):
                    nb_function += 1
                inside_function = True
            elif line[-1] == ',':
                inside_function = True
                function_line_count = -1 #Skip the next line due to clang-format

        elif line.startswith('}'):
            if function_line_count > 40 and function_name not in long_functions:
                long_functions.append([function_name, colored(f"Contains {function_line_count}, 40 expected lines.", "red")])
                succ = 1
            function_line_count = 0
            inside_function = False
        elif inside_function:
            if line.strip() != "" and not line.isspace() and not line.strip().startswith('{') and not line.strip().startswith('}') and not line.strip().startswith('/'):
                function_line_count += 1

    if nb_function > 10:
        buf += colored(f" Too many exported functions: {nb_function} found.","red")

    nb_function = len(long_functions) - 1
    for i, function in enumerate(long_functions):
        function_name = function[0]
        branch = "│   " + ("├── " if i < nb_function - 1 else "└── ")
        buf += "\n" + branch + colored("✗ ", "red") + function_name + " " + function[1]


    return succ, buf

def parse_file():
    global DIRECTORY_PATH

    buf = ""

    for root, dirs, files in os.walk(DIRECTORY_PATH):
        long_functions = []
        output = ""

        files = sorted([file for file in files if file.endswith((".h", ".c"))])

        succes = 0

        for i, file in enumerate(files):

            nb_files = len(files)
            branch = "├── " if i < nb_files - 1 else "└── "

            file_path = os.path.join(root, file)
            succ, temp = analyze_c_code(file_path)
            succes += succ

            output += branch + file
            output += temp + "\n"

        dir_name = root.split('/')[-1]
        if succes == 0:
            buf += colored("✓ ", "green") + colored(f"{dir_name}", "yellow") + "\n"
        else:
            buf += colored("✗ ", "red") + colored(f"{dir_name}", "yellow") + "\n"
        buf += output + "\n"


    print(buf)

def clang_tidy():
    parse_file()



clang_tidy()
#analyze_c_code('../src/lexer/lexer.c')
