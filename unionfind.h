#ifndef UNION_FIND_H
#define UNION_FIND_H

class UnionFind {
public:
    UnionFind(int n);
    int find(int x);
    void union_sets(int x, int y);
    int count_components();
    ~UnionFind();

private:
    int size;
    int* parent;
    int* rank;
};

#endif
