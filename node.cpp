#include "Node.h"

Node::Node(Vertex* v, int numVertices) : vertex(v), saturation(0), numNeighbors(v->numNeighbors), index(v->index), next(nullptr), prev(nullptr) {
    neighborColors = new int[numVertices];
    for (int i = 0; i < numVertices; ++i) {
        neighborColors[i] = -1;
    }
}

Node::~Node() {
    delete[] neighborColors;
}
