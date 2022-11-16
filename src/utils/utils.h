#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>

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

std::map<int,bool> combine_map(std::map<int,bool> &m1, std::map<int,bool> &m2);

template<typename T>
std::vector<T> combine_vector(const std::vector<T> &a, const std::vector<T> &b) {
    std::vector<T> c = a;
    for(T x : b) c.push_back(x);
    return c;
}

std::vector<bool> map_to_vector(std::map<int,bool> &awake_round_map, int finished_round);
bool is_idle_round(std::vector<std::vector<bool>> &all_awake_round_vec, int round_id);

#endif // UTILS_UTILS_H_
