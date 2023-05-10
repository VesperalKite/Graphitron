import argparse
import unittest
import subprocess
import os

CXX_COMPILER="${CXX_COMPILER}"
GRAPHITRON_BUILD_DIRECTORY="${GRAPHITRON_BUILD_DIRECTORY}".strip().rstrip('/')
GRAPHITRON_SOURCE_DIRECTORY="${GRAPHITRON_SOURCE_DIRECTORY}".strip().rstrip('/')




def parseArgs():
    parser = argparse.ArgumentParser(description='compiling graphitron files')
    parser.add_argument('-f', dest = 'input_file_name')
    parser.add_argument('-o', dest = 'output_file_name')
    parser.add_argument('-p', dest = 'optimization_option')
    parser.add_argument('-a', dest = 'input_algo_file_name')
    parser.add_argument('-i', dest = 'runtime_include_path', default = GRAPHITRON_SOURCE_DIRECTORY+'/include/')
    parser.add_argument('-l', dest = 'graphitronlib_path', default = GRAPHITRON_BUILD_DIRECTORY+'/lib/libgraphitronlib.a')
    args = parser.parse_args()
    return vars(args)

if __name__ == '__main__':
    args = parseArgs()
    input_file_name = args['input_file_name']
    output_file_name = args['output_file_name']
    runtime_include_path = args['runtime_include_path']
    graphitronlib_path = args['graphitronlib_path']

    #check if user supplied a separate algorithm file from the parameter file
    supplied_separate_algo_file = False

    if (args['input_algo_file_name']):
        # use the separate algorithm file if supplied, and use the input_file only for the parameter
        supplied_separate_algo_file = True
        algo_file_name = args['input_algo_file_name']
    else:
        # use the input_file for both the algorithm and parameter
        algo_file_name = 'algo.gt'

    if (args['optimization_option']):
        optimization_option = args['optimization_option']
    else:
        optimization_option = "0"

    compile_file_name = 'compile.cpp'


    # read the input file
    with open(input_file_name) as f:
        content = f.readlines()

    if not supplied_separate_algo_file:
        # copy lines up to the point of 'parameter:' to 'algo.gt' file
        algo_file = open(algo_file_name, 'w')
    parameter_cmd_list = []
    is_processing_parameter = False

    for line in content:
        if line.startswith("parameter:"):
            is_processing_parameter = True
        elif is_processing_parameter:
            parameter_cmd_list.append(line)
        else:
            if not supplied_separate_algo_file:
                algo_file.write(line)

    if not supplied_separate_algo_file:
        algo_file.close()


    COMPILER_BINARY = ""
    if len(parameter_cmd_list) == 0:
        COMPILER_BINARY=GRAPHITRON_BUILD_DIRECTORY+"/bin/graphitron_compiler/graphitronc"
    else:
        # generate the parameter file parameter.cpp
        compile_file = open(compile_file_name, 'w')

        compile_file.write("#include <graphitron/midend/parameter.h>\n")
        compile_file.write("namespace graphitron {\n")
        compile_file.write("void user_defined_parameter (graphitron::mir::parameter::ProgramParameterNode::Ptr program) {\n")
        for parameter_cmd in parameter_cmd_list:
            compile_file.write(parameter_cmd)
        compile_file.write("}\n")
        compile_file.write("}")


        compile_file.close()

        COMPILER_BINARY = GRAPHITRON_BUILD_DIRECTORY+"/bin/graphitron_compiler/compile.o"
        
        # compile and execute compile.cpp file to complete the compilation
        #TODO: code here uses very fragile relavtive paths, figure out a better way
        # Maybe setting environment variables
        try:
            subprocess.check_call(CXX_COMPILER + " -g -std=c++14 -I {0} {1} -o {2} {3}".format(runtime_include_path, compile_file_name, COMPILER_BINARY, graphitronlib_path), stderr=subprocess.STDOUT, shell=True)
        except subprocess.CalledProcessError as e:
            print(e.output)
            raise

    try:
        subprocess.check_call(COMPILER_BINARY + " -p " + optimization_option + " -f " + algo_file_name +  " -o " + output_file_name , stderr=subprocess.STDOUT, shell=True)
    except subprocess.CalledProcessError as e:
        print(e.output)
        raise

    #subprocess.check_call("g++ -g -std=c++11 -I ../../src/runtime_lib/  " + output_file_name + " -o test.o", shell=True)
    if algo_file_name == "algo.gt" and os.path.exists(algo_file_name):
        os.unlink(algo_file_name)

