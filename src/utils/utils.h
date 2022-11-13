#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
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

#endif // UTILS_UTILS_H_
