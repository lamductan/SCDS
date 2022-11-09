#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <string>
#include <vector>
#include <iostream>

#define MAX_NUM_BITS 10

std::string join_path(const std::vector<std::string> &tokens);
void mkPath(const char *pathName);

int log_2(int n);
int log_star(int n);
int find_highest_greater_bit(int r_v, int r_u);
std::ostream &print_binary(int x, std::ostream& os=std::cout);

#endif // UTILS_UTILS_H_
