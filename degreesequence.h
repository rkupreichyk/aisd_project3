#ifndef DEGREE_SEQUENCE_H
#define DEGREE_SEQUENCE_H

#include "Vertex.h"

struct DegreeSequence {
    Vertex* vertex;
    int degree;

    DegreeSequence();
    DegreeSequence(Vertex* v, int d);
};

#endif
