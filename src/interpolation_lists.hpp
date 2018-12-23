//
// Created by genshen on 2018-12-23.
//

#ifndef POT_INTERPOLATION_LISTS_H
#define POT_INTERPOLATION_LISTS_H


#include "data_structure/array_map.hpp"
#include "interpolation_object.h"
#include "types.h"

template<typename K, typename IN>
class InterpolationLists : public ArrayMap<K, IN> {
    static_assert(std::is_base_of<InterpolationObject, IN>::value, "typename IN must derive from InterpolationObject");
public:
    InterpolationLists(array_map::type_map_size capacity);

    /**
     * sync all @memberof elements from root processor to other processors.
     * @param root the root processor.
     * @param rank MPI rank id of current processor.
     * @param comm communicator.
     */
    void sync(const int root, const int rank, MPI_Comm comm);

    /**
     * sync @memberof elements from root processor to other processors.
     * @note Make sure @param size must be the same on all processors in @param comm.
     *
     * @param size the count of elements to be synced.
     * @param root the root processor.
     * @param rank MPI rank id of current processor.
     * @param comm communicator.
     */
    void sync(const array_map::type_map_size size, const int root, const int rank, MPI_Comm comm);
};

template<typename K, typename IO>
InterpolationLists<K, IO>::InterpolationLists(array_map::type_map_size capacity)
        :ArrayMap<K, IO>(capacity) {}

template<typename K, typename IO>
void InterpolationLists<K, IO>::sync(const int root, const int rank, MPI_Comm comm) {
    array_map::type_map_size size;
    MPI_Bcast(&size, 1, array_map::MPI_Type_MapSize, root, comm); // sync size of array map.
    ArrayMap<K, IO>::resize(size); // make the size the same on all processors.
    for (array_map::type_map_index i = 0; i < ArrayMap<K, IO>::_size; i++) {
        ArrayMap<K, IO>::elements[i].bcastInterpolationObject(root, rank, comm);
    }
}

template<typename K, typename IO>
void
InterpolationLists<K, IO>::sync(const array_map::type_map_size size, const int root, const int rank, MPI_Comm comm) {
    // the size of data elements may be different on each processors, make them the same.
    ArrayMap<K, IO>::resize(size);
    for (array_map::type_map_index i = 0; i < ArrayMap<K, IO>::_size; i++) {
        ArrayMap<K, IO>::elements[i].bcastInterpolationObject(root, rank, comm);
    }
}


#endif //POT_INTERPOLATION_LISTS_H
