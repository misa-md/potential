# lib-pot
> a lib calculating potential(e.g. eam potential).

## Install dependencies
```bash
# enter project root directory
$ pkf fetch
$ pkg install # install dependency
```

## Build
```bash
$ cmake -B./build -H./ \
 -DPOT_OpenMP_ENABLE_FLAG=OFF \
 -DPOT_MPI_ENABLE_FLAG=ON \   # enable mpi
 -DPOT_TEST_BUILD_ENABLE_FLAG=ON \  # enable unit tests
 -DPOT_TEST_MPI_ENABLE_FLAG=ON
$ cmake --build ./build -j 4
```
