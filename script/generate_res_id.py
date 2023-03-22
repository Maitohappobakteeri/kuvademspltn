#!/usr/bin/python3


import os
import sys
import time
import string


SRC_DIR = "src/common"
RES_DIR = "res"
HEADER_FILENAME = "resid.hpp"
ID_TYPE = "const unsigned int"


VAR_ALLOWED = string.ascii_uppercase + string.digits
VAR_REPLACE_CHAR = '_'


def to_variable_name(s):

    varName = ""
    for c in s.upper():
        if c in VAR_ALLOWED:
            varName += c
        else:
            varName += VAR_REPLACE_CHAR
    return varName


def list_files_recursively(root):

    files = []

    paths = os.listdir(root)
    paths = list(map(lambda s: os.path.join(root, s), paths))
    while not len(paths) == 0:
        if os.path.isdir(paths[0]):
            paths.extend(list(map(lambda s: paths[0] + "/" + s, os.listdir(paths[0]))))
        else:
            files.append(paths[0])
        paths.pop(0)

    return files


def create_variable_name(filename, extension, isDuplicate):

    if isDuplicate:
        return to_variable_name(filename + extension)
    else:
        return to_variable_name(filename)


def write_header(out, filepaths):

    # split all paths
    splitPaths = []
    for path in filepaths:
        filename, extension = os.path.splitext(path)
        splitPaths.append([filename, extension, False])

    # mark all duplicate files (duplicate -> same path without the extension)
    for path in splitPaths:
        if path[2] == True:
            continue
        for anotherPath in splitPaths:
            if path is anotherPath:
                continue
            if anotherPath[2] == True:
                continue
            if path[0] == anotherPath[0]:
                path[2] = True
                anotherPath[2] = True

    # print info about generation
    print("// Generated with script: {script}".format(script=__file__), file=out)
    print("// {time}".format(time=time.strftime("%a %d.%m.%Y - %H:%M:%S")), file=out)
    print("", file=out)
    print("", file=out)

    # print include guard
    INCLUDE_GUARD_NAME = to_variable_name(HEADER_FILENAME)
    print("#ifndef {guardName}".format(guardName=INCLUDE_GUARD_NAME), file=out)
    print("#define {guardName}".format(guardName=INCLUDE_GUARD_NAME), file=out)
    print("", file=out)
    print("", file=out)

    # print includes
    print("#include \"residgenerator.hpp\"", file=out)
    print("", file=out)
    print("#include <map>", file=out)
    print("#include <string>", file=out)
    print("", file=out)
    print("", file=out)

    # print all variable declarations
    idCounter = 0
    for splitPath in splitPaths:
        varName = create_variable_name(*splitPath)
        print("{type} {varName} = {ID};".format(type=ID_TYPE, varName=varName, ID=idCounter),
              file=out)
        idCounter += 1
    print("", file=out)
    print("", file=out)

    # print std::map for filename -> ID pairs
    print("const std::map<std::string, unsigned int> RESOURCE_ID_MAP = {", file=out)
    for splitPath in splitPaths:
        varName = create_variable_name(*splitPath)
        print("    {{\"{filename}\", {varName}}},".format(filename=splitPath[0]+splitPath[1],
                                                           varName=varName), file=out)
    print("};", file=out)
    print("", file=out)
    print("", file=out)

    # print header end
    print("#endif", file=out)


if __name__ == "__main__":

    with open(os.path.join(SRC_DIR, HEADER_FILENAME), "w") as f:
        write_header(f, list_files_recursively(RES_DIR))
