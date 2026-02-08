#include "Graph.h"
#include <iostream>
#include <cstring>

Graph::Graph(int size) : size(size) {
    vertices = new Vertex[size];
}

Graph::~Graph() {
    delete[] vertices;
}

void Graph::greedyColoring() {
    int* result = new int[size];
    bool* available = new bool[size];

    for (int i = 0; i < size; ++i) {
        result[i] = -1;
        available[i] = true;
    }

    result[0] = 0;

    for (int u = 1; u < size; ++u) {
        Vertex* vertex = &vertices[u];

        for (int i = 0; i < vertex->numNeighbors; ++i) {
            int neighborIndex = vertex->neighbors[i]->index;
            if (result[neighborIndex] != -1) {
                available[result[neighborIndex]] = false;
            }
        }

        int cr;
        for (cr = 0; cr < size; ++cr) {
            if (available[cr]) {
                break;
            }
        }

        result[u] = cr;

        for (int i = 0; i < vertex->numNeighbors; ++i) {
            int neighborIndex = vertex->neighbors[i]->index;
            if (result[neighborIndex] != -1) {
                available[result[neighborIndex]] = true;
            }
        }
    }

    for (int u = 0; u < size; ++u) {
        std::cout << result[u] + 1 << ' ';
    }
    std::cout << std::endl;

    delete[] result;
    delete[] available;
}

void Graph::LFColoring(DegreeSequence* sortedDegreeSequence) {
    int* result = new int[size];
    bool* available = new bool[size];

    for (int i = 0; i < size; ++i) {
        result[i] = -1;
        available[i] = true;
    }

    for (int i = 0; i < size; ++i) {
        Vertex* vertex = sortedDegreeSequence[i].vertex;

        for (int j = 0; j < vertex->numNeighbors; ++j) {
            int neighborIndex = vertex->neighbors[j]->index;
            if (result[neighborIndex] != -1) {
                available[result[neighborIndex]] = false;
            }
        }

        int cr;
        for (cr = 0; cr < size; ++cr) {
            if (available[cr]) {
                break;
            }
        }

        result[vertex->index] = cr;

        for (int j = 0; j < vertex->numNeighbors; ++j) {
            int neighborIndex = vertex->neighbors[j]->index;
            if (result[neighborIndex] != -1) {
                available[result[neighborIndex]] = true;
            }
        }
    }

    for (int i = 0; i < size; ++i) {
        std::cout << result[i] + 1 << ' ';
    }
    std::cout << std::endl;

    delete[] result;
    delete[] available;
}

void Graph::moveNodeToLeft(List& list, Node* node) {
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    }
    else {
        list.head = node->next;
    }
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    }

    Node* current = list.head;
    while (current != nullptr &&
        (node->saturation < current->saturation ||
            (node->saturation == current->saturation && node->numNeighbors < current->numNeighbors) ||
            (node->saturation == current->saturation && node->numNeighbors == current->numNeighbors && node->index > current->index))) {
        current = current->next;
    }

    if (current == nullptr) {
        if (list.head == nullptr) {
            list.head = node;
            node->next = nullptr;
            node->prev = nullptr;
        }
        else {
            Node* last = list.head;
            while (last->next != nullptr) {
                last = last->next;
            }
            last->next = node;
            node->prev = last;
            node->next = nullptr;
        }
    }
    else {
        if (current->prev != nullptr) {
            current->prev->next = node;
        }
        else {
            list.head = node;
        }
        node->prev = current->prev;
        node->next = current;
        current->prev = node;
    }
}

void Graph::updateSaturationAndMove(List& list, Node* node, int color, int numVertices) {
    if (node->neighborColors[color] == -1) {
        node->saturation++;
    }
    node->neighborColors[color]++;

    moveNodeToLeft(list, node);
}

void Graph::SLFColoring(DegreeSequence* sortedDegrees, int size) {
    List vertexList;
    vertexList.initializeFromSortedDegrees(sortedDegrees, size);

    int* result = new int[size];
    bool* available = new bool[size];
    bool* usedColors = new bool[size];

    for (int i = 0; i < size; ++i) {
        result[i] = -1;
        available[i] = true;
        usedColors[i] = false;
    }

    while (vertexList.head != nullptr) {
        Node* currentNode = vertexList.head;
        vertexList.head = currentNode->next;
        if (vertexList.head != nullptr) {
            vertexList.head->prev = nullptr;
        }

        for (int i = 0; i < currentNode->numNeighbors; ++i) {
            int neighborIndex = currentNode->vertex->neighbors[i]->index;
            if (result[neighborIndex] != -1) {
                usedColors[result[neighborIndex]] = true;
                available[result[neighborIndex]] = false;
            }
        }

        int cr;
        for (cr = 0; cr < size; ++cr) {
            if (available[cr]) {
                break;
            }
        }

        result[currentNode->index] = cr;

        for (int i = 0; i < currentNode->numNeighbors; ++i) {
            int neighborIndex = currentNode->vertex->neighbors[i]->index;
            if (result[neighborIndex] == -1) {
                Node* neighborNode = findNodeByIndex(vertexList.head, neighborIndex);
                if (neighborNode != nullptr) {
                    updateSaturationAndMove(vertexList, neighborNode, cr, size);
                }
            }
        }

        for (int i = 0; i < currentNode->numNeighbors; ++i) {
            int neighborIndex = currentNode->vertex->neighbors[i]->index;
            if (result[neighborIndex] != -1) {
                usedColors[result[neighborIndex]] = false;
                available[result[neighborIndex]] = true;
            }
        }

        delete currentNode;
    }

    for (int i = 0; i < size; ++i) {
        std::cout << result[i] + 1 << ' ';
    }
    std::cout << std::endl;

    delete[] result;
    delete[] available;
    delete[] usedColors;
}


Node* Graph::findNodeByIndex(Node* head, int index) {
    Node* current = head;
    while (current != nullptr) {
        if (current->index == index) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

Vertex* Graph::first_unvisited(int& lastFound) {
    for (int i = lastFound; i < size; ++i) {
        if (vertices[i].isVisited == -1) {
            lastFound = i;
            return &vertices[i];
        }
    }
    for (int i = 0; i < lastFound; ++i) {
        if (vertices[i].isVisited == -1) {
            lastFound = i;
            return &vertices[i];
        }
    }
    return nullptr;
}

bool Graph::isBipartite() {
    int* colors = new int[size];
    for (int i = 0; i < size; ++i) {
        colors[i] = -1;
    }

    for (int i = 0; i < size; ++i) {
        if (colors[i] == -1) {
            if (!dfsCheck(i, 0, colors)) {
                delete[] colors;
                return false;
            }
        }
    }

    delete[] colors;
    return true;
}

bool Graph::dfsCheck(int v, int color, int* colors) {
    colors[v] = color;

    for (int i = 0; i < vertices[v].numNeighbors; ++i) {
        int neighbor = vertices[v].neighbors[i]->index;
        if (colors[neighbor] == -1) {
            if (!dfsCheck(neighbor, 1 - color, colors)) {
                return false;
            }
        }
        else if (colors[neighbor] == color) {
            return false;
        }
    }

    return true;
}

unsigned long long Graph::number_of_subgraphs() {
    unsigned long long sum = 0;

    int* conectionsNum = new int[size];

    for (int start = 0; start < size; ++start) {
        Vertex* parent = &vertices[start];
        int parents_childs = parent->numNeighbors;

        std::memset(conectionsNum, 0, size * sizeof(int));

        for (int i = 0; i < parents_childs; i++) {
            Vertex* neighbor = parent->neighbors[i];
            if (neighbor->index > start) {
                int numNeighborNeighbors = neighbor->numNeighbors;
                for (int j = 0; j < numNeighborNeighbors; j++) {
                    Vertex* neighborOfNeighbor = neighbor->neighbors[j];
                    if (neighborOfNeighbor->index > start) {
                        conectionsNum[neighborOfNeighbor->index]++;
                    }
                }
            }
        }

        for (int i = start + 1; i < size; i++) {
            if (conectionsNum[i] > 1) {
                sum += static_cast<unsigned long long>(conectionsNum[i]) * (conectionsNum[i] - 1) / 2;
            }
        }
    }

    delete[] conectionsNum;

    return sum;
}

unsigned long long Graph::number_of_subgraphs_for_large_graph() {
    int* colors = new int[size];
    unsigned long long countOfSubgraphs = 0;

    for (int i = 0; i < size; ++i) {
        colors[i] = -1;
    }

    for (int start = 0; start < size; ++start) {
        if (colors[start] == -1) {
            int depth = 1;
            dfs_subgraphs(start, colors, depth, start, countOfSubgraphs);
            colors[start] = 0;
        }
    }

    delete[] colors;
    return static_cast<unsigned long long>(countOfSubgraphs / 2);
}

void Graph::dfs_subgraphs(int v, int* colors, int depth, int parent, unsigned long long& countOfSubgraphs) {
    colors[v] = depth;
    Vertex* vertex = &vertices[v];

    for (int i = 0; i < vertex->numNeighbors; ++i) {
        int neighborIndex = vertex->neighbors[i]->index;

        if (colors[neighborIndex] == -1) {
            if (depth < 4) {
                dfs_subgraphs(neighborIndex, colors, depth + 1, parent, countOfSubgraphs);
            }
        }
        else if (depth == 4 && neighborIndex == parent) {
            countOfSubgraphs++;
            break;
        }
    }

    colors[v] = -1;
}
