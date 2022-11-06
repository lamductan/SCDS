#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <iostream>
#include <vector>
#include <unordered_map>

template<typename K, typename V>
std::vector<V> getMapValues(const std::unordered_map<K, V>& m) {
    std::vector<V> values;
    for(auto it : m) values.push_back(it.second);
    return values;
}

template<typename K, typename V>
std::vector<K> getMapKeys(const std::map<K, V>& m) {
    std::vector<K> keys;
    for(auto it : m) keys.push_back(it.first);
    return keys;
}

template<typename K, typename V>
std::vector<V> getMapValues(const std::map<K, V>& m) {
    std::vector<V> values;
    for(auto it : m) values.push_back(it.second);
    return values;
}

template<typename T>
std::ostream& print_vector_ptr(const std::vector<T*>& v, char last_char, std::ostream& os) {
    os << "[";
    for(const T* t : v) os << *t << ", ";
    os << "]";
    if (last_char) os << last_char;
    return os;
}

template<typename T>
std::ostream& print_vector_ptr_new_line(const std::vector<T*>& v, char last_char, std::ostream& os) {
    std::string tab = "    ";
    os << "[\n";
    for(const T* t : v) os << tab << *t << ",\n";
    os << "]";
    if (last_char) os << last_char;
    return os;
}

std::string vector_to_string(const std::vector<int>& a) {
    std::string str = "[";
    for(int x : a) {
        str += std::to_string(x);
        str += ",";
    }
    str.pop_back();
    str += "]";
    return str;
}

#endif // UTILS_UTILS_H_
