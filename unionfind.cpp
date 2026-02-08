#include "UnionFind.h"

UnionFind::UnionFind(int n) : size(n) {
    rank = new int[n];
    parent = new int[n];

    for (int i = 0; i < n; ++i) {
        parent[i] = i;
        rank[i] = 0;
    }
}

int UnionFind::find(int x) {
    if (parent[x] != x) {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

void UnionFind::union_sets(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);

    if (rootX != rootY) {

        if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        }
        else if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        }
        else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
    }
}

int UnionFind::count_components() {
    int count = 0;

    for (int i = 0; i < size; ++i) {
        if (parent[i] == i) {
            count++;
        }
    }
    return count;
}

UnionFind::~UnionFind() {
    delete[] rank;
    delete[] parent;
}
