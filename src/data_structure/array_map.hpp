//
// Created by genshen on 2018-12-22.
//

#ifndef POT_ARRAY_MAP_H
#define POT_ARRAY_MAP_H

#include "types.h"
#include <climits>
#include <mpi.h>
#include <vector>

/**
 * @deprecated ArrayMap is deprecated now. Please use LinearArray instead.
 *
 * Map implementation also can be indexed as array index.
 * The size is usually less then 500.
 */
template <typename K, typename V> class ArrayMap {
public:
  V *elements;

  explicit ArrayMap(array_map::type_map_size capacity);

  ~ArrayMap();

  static const array_map::type_map_size INDEX_UN_LIMITED = USHRT_MAX;

  // insert data and return the index inserted.
  array_map::type_map_size insert(K const key, V const value);

  // return the pointer of the element specified by key.
  V *get(K const key);

  const array_map::type_map_size size();

  /**
   * resize data container.
   * If succeed, true will be returned.
   * @param new_size new size.
   * @return true for succeed, false for otherwise.
   */
  bool resize(array_map::type_map_size new_size);

protected:
  const array_map::type_map_size capacity;
  array_map::type_map_size _size = 0;
  K *keys;

  // return the index by key
  array_map::type_map_size index(K const key);
};

template <typename K, typename V>
ArrayMap<K, V>::ArrayMap(array_map::type_map_size capacity) : capacity(capacity), _size(0) {
  keys = new K[capacity];
  elements = new V[capacity];
}

template <typename K, typename V> ArrayMap<K, V>::~ArrayMap() {
  delete[] keys;
  delete[] elements;
}

template <typename K, typename V> array_map::type_map_size ArrayMap<K, V>::insert(const K key, const V value) {
  if (_size >= capacity) {
    return INDEX_UN_LIMITED; // insert fail.
  }
  array_map::type_map_index i = index(key);
  if (index(key) == INDEX_UN_LIMITED) {
    keys[_size] = key;
    elements[_size] = value;
    return _size++;
  } else {
    elements[i] = value;
    return i;
  }
}

template <typename K, typename V> V *ArrayMap<K, V>::get(const K key) {
  array_map::type_map_index i = index(key);
  if (index(key) == INDEX_UN_LIMITED) {
    return nullptr;
  }
  return &elements[i];
}

template <typename K, typename V> array_map::type_map_size ArrayMap<K, V>::index(const K key) {
  array_map::type_map_size i = 0;
  for (i = 0; i < _size; i++) {
    if (keys[i] == key) {
      return i;
    }
  }
  return INDEX_UN_LIMITED;
}

template <typename K, typename V> const array_map::type_map_size ArrayMap<K, V>::size() { return _size; }

template <typename K, typename V> bool ArrayMap<K, V>::resize(array_map::type_map_size new_size) {
  if (new_size <= capacity) {
    _size = new_size;
    return true;
  }
  return false;
}

#endif // POT_ARRAY_MAP_H
