#ifndef VERTEX_H
#define VERTEX_H

struct Vertex {
    int index;
    int isVisited;
    Vertex** neighbors;
    int numNeighbors;

    Vertex();
    Vertex(int idx, int size);
    ~Vertex();
};

#endif
