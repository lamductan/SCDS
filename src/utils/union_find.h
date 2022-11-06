#ifndef UTILS_UNION_FIND_H_
#define UTILS_UNION_FIND_H_

#include <algorithm>
#include <vector>

class UnionFind {
private:
    int size;
    std::vector<int> parent;
    std::vector<int> rk;
public:
    UnionFind(int n) {
        size = n;
        parent.assign(n, 0);
        rk.assign(n, 0);
        for(int i = 0; i < n; ++i) parent[i] = i;
    }

    int find(int x) {
        if (x == parent[x]) return x;
        return parent[x] = find(parent[x]);
    }

    void join(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) return;
        if (rk[x] < rk[y]) std::swap(x, y);
        parent[y] = x;
        if (rk[x] == rk[y]) ++rk[x];
    }
};

#endif //UTILS_UNION_FIND_H_