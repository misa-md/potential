# lib-pot
> a lib calculating potential(e.g. eam potential).

## Install dependencies
```bash
$ cd lib-pot
$ pkg install # install dependency
```

## Build
```bash
$ cd lib-pot
$ cmake -B./build -H./ \
 -DOpenMP_ENABLE_FLAG=ON \
 -DMPI_ENABLE_FLAG=ON \   # enable mpi
 -DTEST_BUILD_ENABLE_FLAG=ON \
 -DTEST_MPI_ENABLE_FLAG=ON \    # enable unit tests
 -DTEST_MPI_ENABLE_FLAG=OFF
$ cmake --build ./build -j 4
```
