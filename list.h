#ifndef LIST_H
#define LIST_H

#include "Node.h"
#include "DegreeSequence.h"

class List {
public:
    Node* head;
    Node* tail;

    List();
    void initializeFromSortedDegrees(DegreeSequence* sortedDegrees, int size);
    ~List();
};

#endif
