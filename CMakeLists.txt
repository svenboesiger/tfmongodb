cmake_minimum_required(VERSION 3.9)
project(TFMongoDB)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")
link_directories( /usr/local/lib ) # for specific path

if(APPLE)
    set(ADDITIONAL_GCC_OPTIONS "-undefined dynamic_lookup")
endif()
set( CMAKE_CXX_FLAGS "${ADDITIONAL_GCC_OPTIONS}" )

###########################################################################################
## target definitions #####################################################################
###########################################################################################
add_library(TFMongoDB SHARED
        src/cpp/op/tfmongodb.cc
        src/cpp/kernel/tfmongodb_kernel.cc
        src/cpp/connector/mongodb_connector.cpp
        src/cpp/converter/bsonconverter.cpp
        src/cpp/converter/element_to_string.cpp)

###########################################################################################
## dependencies ###########################################################################
###########################################################################################
find_package(libmongocxx)
find_package(libbsoncxx)
include_directories(${LIBMONGOCXX_INCLUDE_DIRS} ${LIBBSONCXX_INCLUDE_DIRS})

set(PYTHON_CMD "${CMAKE_SOURCE_DIR}/venv_tf/bin/python")
set(PYTHON_CMD_PARAMETER "-c 'import tensorflow as tf\; print(tf.sysconfig.get_include())'")
exec_program(${PYTHON_CMD}
        ARGS ${PYTHON_CMD_PARAMETER}
        OUTPUT_VARIABLE TENSORFLOW_C_FLAGS)
MESSAGE(STATUS "Tensorflow include directory:" ${TENSORFLOW_C_FLAGS})
include_directories(${TENSORFLOW_C_FLAGS} "${TENSORFLOW_C_FLAGS}/external/nsync/public")

set(PYTHON_CMD_PARAMETER2 "-c'import tensorflow as tf\; print(tf.sysconfig.get_lib())'")
exec_program(${PYTHON_CMD}
        ARGS ${PYTHON_CMD_PARAMETER2}
        OUTPUT_VARIABLE TENSORFLOW_L_FLAGS)
MESSAGE(STATUS "Tensorflow library path:" ${TENSORFLOW_L_FLAGS})
link_directories(${TENSORFLOW_L_FLAGS})

target_link_libraries(TFMongoDB ${LIBMONGOCXX_LIBRARIES} ${TENSORFLOW_L_FLAGS}/libtensorflow_framework.so ${TENSORFLOW_L_FLAGS}/python/_pywrap_tensorflow_internal.so)

###########################################################################################
## testing ################################################################################
###########################################################################################
#include_directories(third-party/googletest/include)
#link_directories(third-party/googletest)
#add_executable( run_converter_tests
#        test/converter_test.cpp
#        src/cpp/converter/bsonconverter.cpp
#        src/cpp/converter/element_to_string.cpp)
#target_link_libraries(run_converter_tests ${LIBBSONCXX_LIBRARIES} gtest gtest_main pthread)
