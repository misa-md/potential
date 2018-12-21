# {{.Application.AppName}}
{{.Application.Describe}}

## Setup
```bash
$ cd {{.Application.AppName}}
$ pkg install # install dependency
```

## Build
```bash
$ cd {{.Application.AppName}}
$ mkdir build
$ cd build
$ cmake ../  \
 -DOpenMP_ENABLE_FLAG=ON \
 -DMPI_ENABLE_FLAG=ON \
 -DMPI_ENABLE_FLAG=ON \
 -DTEST_ENABLE_FLAG=ON \    # enable unit tests
 -DTEST_MPI_ENABLE_FLAG=ON
$ make
$ ./bin/{{- .Application.AppName}}   # run your application, you can change it to another name in file src/CMakeLists.txt
$ ./bin/unit-tests   # run unit tests if you set TEST_ENABLE_FLAG to true.
$ mpirun -n 4 ./bin/{{- .Application.AppName}}   # run application in mpi environment if you set MPI_ENABLE_FLAG to true .
$ mpirun -n 4 ./bin/unit-tests   # run unit tests in mpi environment if you set TEST_ENABLE_FLAG,MPI_ENABLE_FLAG and TEST_MPI_ENABLE_FLAG to true.
```
