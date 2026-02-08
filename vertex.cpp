#include "Vertex.h"

Vertex::Vertex() : index(-1), isVisited(-1), neighbors(nullptr), numNeighbors(0) {}

Vertex::Vertex(int idx, int size) : index(idx), isVisited(-1), numNeighbors(size) {
    neighbors = new Vertex * [size];
}

Vertex::~Vertex() {
    delete[] neighbors;
}
