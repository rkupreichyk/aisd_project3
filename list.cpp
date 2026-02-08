#include "List.h"

List::List() : head(nullptr), tail(nullptr) {}

void List::initializeFromSortedDegrees(DegreeSequence* sortedDegrees, int size) {
    if (size == 0) return;

    head = new Node(sortedDegrees[0].vertex, size);
    Node* current = head;

    for (int i = 1; i < size; ++i) {
        Node* newNode = new Node(sortedDegrees[i].vertex, size);
        current->next = newNode;
        newNode->prev = current;
        current = newNode;
    }

    tail = current;
}

List::~List() {
    Node* current = head;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}
