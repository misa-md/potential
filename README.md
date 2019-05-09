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
 -DOpenMP_ENABLE_FLAG=OFF \
 -DMPI_ENABLE_FLAG=ON \   # enable mpi
 -DTEST_BUILD_ENABLE_FLAG=ON \  # enable unit tests
 -DTEST_MPI_ENABLE_FLAG=ON
$ cmake --build ./build -j 4
```
