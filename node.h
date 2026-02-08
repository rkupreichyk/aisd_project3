#ifndef NODE_H
#define NODE_H

#include "Vertex.h"

struct Node {
    Vertex* vertex;
    int saturation;
    int numNeighbors;
    int index;
    int* neighborColors;
    Node* next;
    Node* prev;

    Node(Vertex* v, int numVertices);
    ~Node();
};

#endif
