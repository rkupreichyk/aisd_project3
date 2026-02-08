A high-performance C++ engine developed to solve complex graph theory problems under extreme technical constraints. The project focuses on manual memory management and the implementation of fundamental data structures without relying on any high-level abstractions.

# Project Constraints & Architecture
The primary challenge of this project was the total exclusion of the C++ Standard Template Library (STL) and the std::string type. Every component was built from scratch using C++17 features (classes, templates, and lambdas) to maintain a minimal memory footprint and maximum execution speed.

Key architectural constraints included:
Manual Memory Management: Direct use of new and delete for all dynamic allocations, ensuring precise control over the heap.
Zero Dependencies: All containers, including dynamic arrays and linked lists, were custom-implemented to handle the specific needs of graph processing.
Performance-First Design: Algorithms were optimized to handle large datasets within strict execution time limits.

# Input Data Format
The engine is designed to parse and process graph data sets through standard input in a specific hierarchical format:
k: The number of graphs to follow.
n: The order (number of vertices) of the current graph.
Neighbor Lists: For each of the n vertices, the program reads a list containing
  s: The number of neighbors.
  IDs: n IDs of adjacent vertices (ranging from 1 to n).

# Implementation & Optimization
The engine analyzes several graph parameters, including degree sequences, bipartiteness (via custom DFS), and vertex eccentricity. To solve the NP-complete problem of vertex coloring, I implemented Greedy, LF (Largest First), and SLF (Saturation Largest First) heuristics.

In terms of optimization, the engine effectively handles connectivity analysis. By integrating a disjoint-set union (Union-Find) structure for component tracking, the program identifies connected components almost instantly. This efficiency proved particularly useful as it freed up the computational "budget" for more intensive tasks, such as the detection of C4 subgraphs and calculating the edges of the graph's complement.

# Features Summary
Degree Sequence: Sorted vertex degree calculation.
Connectivity:Efficient component counting and bipartiteness testing.
Graph Coloring: Heuristic-based coloring (Greedy, LF, SLF).
Cycle Detection: Counting unique C4 subgraphs.
Complement Metrics: Determining the number of edges in the complement graph.
