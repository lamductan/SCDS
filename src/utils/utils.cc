#include <bitset>
#include <math.h>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <sys/stat.h>

#include "utils/utils.h"

std::string join_path(const std::vector<std::string> &tokens)
{
    std::string path = "";
    for (int i = 0; i < tokens.size() - 1; ++i)
        path += tokens[i] + "/";
    path += tokens.back();
    return path;
}

void splitFileName(const char *pathname, std::string &dir,
                   std::string &fnameonly)
{
    if (!pathname || !*pathname) {
        dir = ".";
        fnameonly = "";
        return;
    }

    // find last "/" or "\"
    const char *s = pathname + strlen(pathname) - 1;
    s--; // ignore potential trailing "/"
    while (s > pathname && *s != '\\' && *s != '/')
        s--;
    const char *sep = s <= pathname ? nullptr : s;

    // split along that
    if (!sep) {
        // no slash or colon
        if (strchr(pathname, ':') || strcmp(pathname, ".") == 0 ||
            strcmp(pathname, "..") == 0) {
            fnameonly = "";
            dir = pathname;
        } else {
            fnameonly = pathname;
            dir = ".";
        }
    } else {
        fnameonly = s + 1;
        dir = std::string(pathname, s - pathname + 1);
    }
}

bool fileExists(const char *pathname)
{
    // Note: stat("foo/") ==> error, even when "foo" exists and is a directory!
    struct stat statbuf;
    return stat(pathname, &statbuf) == 0;
}

void mkPath(const char *pathname)
{
    if (!fileExists(pathname)) {
        std::string pathprefix, dummy;
        splitFileName(pathname, pathprefix, dummy);
        mkPath(pathprefix.c_str());
        // note: anomaly with slash-terminated dirnames: stat("foo/") says
        // it does not exist, and mkdir("foo/") says cannot create (EEXIST):
        if (mkdir(pathname, 0755) != 0 && errno != EEXIST) {
            char error_msg[1024];
            snprintf(error_msg, 1024, "Cannot create directory '%s': %s",
                     pathname, strerror(errno));
            throw std::runtime_error(error_msg);
        }
    }
}

int log_2(int n) { return ceil(log(n) / log(2)); }

int log_star(int n)
{
    if (n <= 2)
        return 0;
    return log_star(log_2(n)) + 1;
}

int find_highest_greater_bit(int r_v, int r_u)
{
    if (r_v <= r_u)
        return 0;
    std::bitset<MAX_NUM_BITS> y_v(r_v);
    std::bitset<MAX_NUM_BITS> y_u(r_u);
    for (int k = MAX_NUM_BITS - 1; k >= 0; --k) {
        if (y_v[k] > y_u[k])
            return k + 1;
    }
    return -1;
}

std::ostream &print_binary(int x, std::ostream &os)
{
    std::bitset<MAX_NUM_BITS> bs_x(x);
    os << bs_x.to_string();
    return os;
}

std::map<int, bool> combine(std::map<int, bool> &m1, std::map<int, bool> &m2)
{
    std::map<int, bool> m;
    m = m1;
    for (auto it : m2)
        m[it.first] = it.second;
    return m;
}

std::vector<bool> map_to_vector(std::map<int, bool> &awake_round_map,
                                int finished_round)
{
    std::vector<bool> awake_round_vec(finished_round);
    for (int i = 1; i < finished_round; ++i)
        awake_round_vec[i] = awake_round_map[i];
    return awake_round_vec;
}

bool is_idle_round(std::vector<std::vector<bool>> &all_awake_round_vec,
                   int round_id)
{
    int n_nodes = all_awake_round_vec.size();
    for (int i = 0; i < n_nodes; ++i) {
        if (all_awake_round_vec[i][round_id])
            return false;
    }
    return true;
}

std::vector<std::string> splitString(const std::string &s, char delimiter)
{
    std::stringstream ss(s);
    std::string token;
    std::vector<std::string> tokens;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool is_subset(const std::set<int> &a, const std::set<int> &b)
{
    for (int x : a)
        if (b.count(x) == 0)
            return false;
    return a.size() < b.size();
}

std::string tuple_to_string(const std::tuple<int, int, int> &tup)
{
    std::string s = "[";
    s += std::to_string(std::get<0>(tup));
    s += ',';
    s += std::to_string(std::get<1>(tup));
    s += ',';
    s += std::to_string(std::get<2>(tup));
    s += ']';
    return s;
}