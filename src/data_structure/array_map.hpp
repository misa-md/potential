//
// Created by genshen on 2018-12-22.
//

#ifndef POT_MIN_MAP_H
#define POT_MIN_MAP_H

#include <vector>

/**
 * Map implementation also can be indexed as array index.
 * The size is usually less then 500.
 */
namespace array_map {
    typedef unsigned short type_map_size;
    typedef type_map_size type_map_index;
}

template<typename K, typename V>
class ArrayMap {
public:
    V *elements;

    explicit ArrayMap(array_map::type_map_size capacity);

    ~ArrayMap();

    static const array_map::type_map_size INDEX_UN_LIMITED = USHRT_MAX;

    // insert data and return the inserted index.
    array_map::type_map_size insert(K const key, V const value);

    // return the reference of the element specified by key.
    V *get(K const key);

    const array_map::type_map_size size();

protected:
    const array_map::type_map_size capacity;
    array_map::type_map_size _size = 0;
    K *keys;

    // return the index by key
    array_map::type_map_size index(K const key);
};


template<typename K, typename V>
ArrayMap<K, V>::ArrayMap(array_map::type_map_size capacity):capacity(capacity), _size(0) {
    keys = new K[capacity];
    elements = new V[capacity];
}

template<typename K, typename V>
ArrayMap<K, V>::~ArrayMap() {
    delete[]keys;
    delete[]elements;
}

template<typename K, typename V>
array_map::type_map_size ArrayMap<K, V>::insert(const K key, const V value) {
    if (_size >= capacity) {
        return INDEX_UN_LIMITED; // insert fail.
    }
    array_map::type_map_index i = index(key);
    if (index(key) == INDEX_UN_LIMITED) {
        keys[_size] = key;
        elements[_size] = value;
        _size++;
        return _size;
    } else {
        elements[i] = value;
        return i;
    }
}

template<typename K, typename V>
V *ArrayMap<K, V>::get(const K key) {
    array_map::type_map_index i = index(key);
    if (index(key) == INDEX_UN_LIMITED) {
        return nullptr;
    }
    return &elements[i];
}

template<typename K, typename V>
array_map::type_map_size ArrayMap<K, V>::index(const K key) {
    array_map::type_map_size i = 0;
    for (i = 0; i < _size; i++) {
        if (keys[i] == key) {
            return i;
        }
    }
    return INDEX_UN_LIMITED;
}

template<typename K, typename V>
const array_map::type_map_size ArrayMap<K, V>::size() {
    return _size;
}


#endif //POT_MIN_MAP_H
