#ifndef GRAPH_H
#define GRAPH_H

#include "Vertex.h"
#include "DegreeSequence.h"
#include "List.h"

struct Graph {
    Vertex* vertices;
    int size;

    Graph(int size);
    ~Graph();
    void greedyColoring();
    void LFColoring(DegreeSequence* sortedDegreeSequence);
    void SLFColoring(DegreeSequence* sortedDegrees, int size);
    Node* findNodeByIndex(Node* head, int index);
    Vertex* first_unvisited(int& lastFound);
    bool isBipartite();
    unsigned long long number_of_subgraphs();
    unsigned long long number_of_subgraphs_for_large_graph();
    void dfs_subgraphs(int v, int* colors, int depth, int parent, unsigned long long& countOfSubgraphs);
    void updateSaturationAndMove(List& list, Node* node, int color, int numVertices);
    void moveNodeToLeft(List& list, Node* node);

private:
    bool dfsCheck(int v, int color, int* colors);
};

#endif
