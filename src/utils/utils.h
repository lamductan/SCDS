#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#define MAX_NUM_BITS 10

std::string join_path(const std::vector<std::string> &tokens);
void mkPath(const char *pathName);

int log_2(int n);
int log_star(int n);
int find_highest_greater_bit(int r_v, int r_u);
std::ostream &print_binary(int x, std::ostream& os=std::cout);

template<typename K, typename V>
std::vector<K> getMapKeys(const std::unordered_map<K, V>& m) {
    std::vector<K> keys;
    for(auto it : m) keys.push_back(it.first);
    std::sort(keys.begin(), keys.end());
    return keys;
}

template<typename T1, typename T2>
std::map<T1,T2> combine_map(const std::map<T1,T2> &m1, const std::map<T1,T2> &m2) {
    std::map<T1,T2> m = m1;
    for(auto it : m2) m[it.first] = it.second;
    return m;
}

template<typename T>
std::set<T> combine_set(const std::set<T> &s1, const std::set<T> &s2) {
    std::set<T> s = s1;
    for(auto it : s2) s.insert(it);
    return s;
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
std::vector<T> combine_vector(const std::vector<T> &a, const std::vector<T> &b) {
    std::vector<T> c = a;
    for(T x : b) c.push_back(x);
    return c;
}

std::vector<bool> map_to_vector(std::map<int,bool> &awake_round_map, int finished_round);
bool is_idle_round(std::vector<std::vector<bool>> &all_awake_round_vec, int round_id);

std::vector<std::string> splitString(const std::string& s, const char delimiter=' ');
bool is_subset(const std::set<int> &a, const std::set<int> &b); //check if a \subset b

template<typename T>
void print(const T &a, const char *name="") {
    std::cout << name << " = [";
    for(auto x : a) std::cout << x << ",";
    std::cout << "]";
}
#endif // UTILS_UTILS_H_
