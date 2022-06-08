#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include "common/poliz.hpp"

template<typename T>
int find(std::vector<T> vec, T x);

template<typename K, typename V>
int find(std::vector<std::pair<K, V>> vec, K x);

template<typename K, typename V>
int find(std::vector<std::pair<K, V>> vec, V x);

void copyPOLIZ(POLIZ & src, POLIZ & dst, int start, int end);

#endif