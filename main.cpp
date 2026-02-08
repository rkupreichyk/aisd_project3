#include <iostream>
#include <cstring>
#include <cstdlib>

#include "Graph.h"
#include "DegreeSequence.h"
#include "UnionFind.h"

#define STRING_SIZE 10000
#define SMALL_GRAPH 1000
#define LARGE_GRAPH 100000

void merge(DegreeSequence arr[], int left, int mid, int right, DegreeSequence* buffer) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::memcpy(buffer, arr + left, n1 * sizeof(DegreeSequence));
    std::memcpy(buffer + n1, arr + mid + 1, n2 * sizeof(DegreeSequence));

    int i = 0, j = n1, k = left;
    while (i < n1 && j < n1 + n2) {
        if (buffer[i].degree >= buffer[j].degree) {
            arr[k++] = buffer[i++];
        }
        else {
            arr[k++] = buffer[j++];
        }
    }
    while (i < n1) {
        arr[k++] = buffer[i++];
    }
    while (j < n1 + n2) {
        arr[k++] = buffer[j++];
    }
}

void merge_sort(DegreeSequence arr[], int left, int right, DegreeSequence* buffer) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid, buffer);
        merge_sort(arr, mid + 1, right, buffer);
        merge(arr, left, mid, right, buffer);
    }
}

void degree_sequence(DegreeSequence arr[], int size) {
    DegreeSequence* buffer = new DegreeSequence[size];
    merge_sort(arr, 0, size - 1, buffer);
    delete[] buffer;

    for (int i = 0; i < size; ++i) {
        std::cout << arr[i].degree << " ";
    }
    std::cout << std::endl;
}

void answers(DegreeSequence* notSortedDegreeSequence, unsigned long long vertexNumber, UnionFind& uf, Graph* graph, unsigned long long complementsEdgeNumber) {
    degree_sequence(notSortedDegreeSequence, vertexNumber);

    int number_of_components = uf.count_components();
    std::cout << number_of_components << "\n";
    if (graph->isBipartite() == true) {
        std::cout << "T\n";
    }
    else {
        std::cout << "F\n";
    }
    std::cout << "?\n";
    std::cout << "?\n";
    graph->greedyColoring();
    graph->LFColoring(notSortedDegreeSequence);

    if (graph->size < SMALL_GRAPH) {
        graph->SLFColoring(notSortedDegreeSequence, graph->size);
    }
    else {
        std::cout << "?\n";
    }

    if (graph->size > LARGE_GRAPH) {
        printf("%llu\n", graph->number_of_subgraphs_for_large_graph());
    }
    else {
        printf("%llu\n", graph->number_of_subgraphs());
    }

    printf("%llu\n", complementsEdgeNumber);
}

void process_graph(unsigned long long vertexNumber) {
    Graph* graph = new Graph(vertexNumber);
    unsigned long long complementsEdgeNumber = (vertexNumber * (vertexNumber - 1)) / 2;

    UnionFind uf(vertexNumber);
    DegreeSequence* notSortedDegreeSequence = new DegreeSequence[vertexNumber];

    for (unsigned long long j = 0; j < vertexNumber; j++) {
        char string[STRING_SIZE];
        std::cin.getline(string, STRING_SIZE);

        int line_beginning = 0;
        int k = 0;
        int buf = -1;
        int availableVertexNumber = 0;
        char bufString[STRING_SIZE] = "";

        do {
            if (string[k] == ' ') {
                strncpy_s(bufString, string + line_beginning, k - line_beginning);
                bufString[k - line_beginning] = '\0';
                buf = std::atoi(bufString);
                line_beginning = k + 1;
            }
            k++;
        } while (buf == -1);

        Vertex* v = new Vertex(j, buf);
        notSortedDegreeSequence[j] = DegreeSequence(v, buf);

        if (buf != 0) {
            do {
                if (string[k] == ' ') {
                    strncpy_s(bufString, string + line_beginning, k - line_beginning);
                    bufString[k - line_beginning] = '\0';
                    buf = std::atoi(bufString);
                    uf.union_sets(j, buf - 1);
                    line_beginning = k + 1;
                    v->neighbors[availableVertexNumber] = &graph->vertices[buf - 1];
                    availableVertexNumber++;

                    if (buf > j) {
                        complementsEdgeNumber--;
                    }
                }
                k++;
            } while (string[k] != '\0');

            strncpy_s(bufString, string + line_beginning, k - line_beginning);
            bufString[k - line_beginning] = '\0';
            buf = std::atoi(bufString);

            if (bufString[0] != '\0') {
                v->neighbors[availableVertexNumber] = &graph->vertices[buf - 1];
                uf.union_sets(j, buf - 1);
                if (buf > j) {
                    complementsEdgeNumber--;
                }
            }
        }
        graph->vertices[j] = *v;
    }

    answers(notSortedDegreeSequence, vertexNumber, uf, graph, complementsEdgeNumber);

    delete[] notSortedDegreeSequence;
}

int main() {
    int graphNumber;
    std::cin >> graphNumber;
    std::cin.ignore();

    for (int i = 0; i < graphNumber; i++) {
        unsigned long long vertexNumber;
        std::cin >> vertexNumber;
        std::cin.ignore();

        process_graph(vertexNumber);
    }

    return 0;
}
