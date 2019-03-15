set(CMAKE_CXX_STANDARD 11)
# all variables here start with "M_"
set(POT_VERSION "0.2.0")
set(AppName pot)

#############
## options ##
#############
# change to mpicc and mpicxx
#set(CMAKE_C_COMPILER mpicc -cc=gcc -cxx=g++)
#set(CMAKE_CXX_COMPILER mpicxx -cc=gcc -cxx=g++)

option(OpenMP_ENABLE_FLAG "Use OpenMP" OFF) #change this flag to OFF to disable OpenMP
option(MPI_ENABLE_FLAG "Use MPI library" ON) #change this flag to false to disable mpi
option(TEST_BUILD_ENABLE_FLAG "Enable building test" ON) # enable test
option(TEST_MPI_ENABLE_FLAG "Enable MPI in test" ON) # enable mpi in test, its value depends on option MPI_ENABLE_FLAG.

## architecture ralated values.
option(POT_ARCH_SW "Enable sunway athread" OFF) # enable sunway athread if its running on sunway system.

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

if (CMAKE_BUILD_TYPE MATCHES "^(Debug|DEBUG|debug)$")
    set(POT_DEBUG_ENABLE_FLAG ON)
endif ()

#############
## const ##
#############
set(POT_LIB_NAME ${AppName})

# test
set(POT_UINT_TEST_NAME "unit-test")