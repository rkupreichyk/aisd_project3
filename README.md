A high-performance C++ engine developed to solve complex graph theory problems under extreme technical constraints. The project focuses on manual memory management and the implementation of fundamental data structures without relying on any high-level abstractions.

# Project Constraints & Architecture
The primary challenge of this project was the total exclusion of the C++ Standard Template Library (STL) and the std::string type. Every component was built from scratch using C++17 features (classes, templates, and lambdas) to maintain a minimal memory footprint and maximum execution speed.

Key architectural constraints included:
Manual Memory Management: Direct use of new and delete for all dynamic allocations, ensuring precise control over the heap.
Zero Dependencies: All containers, including dynamic arrays and linked lists, were custom-implemented to handle the specific needs of graph processing.
Performance-First Design: Algorithms were optimized to handle large datasets within strict execution time limits.

# Input Data Format
The engine is designed to parse and process graph data sets through standard input in a specific hierarchical format:
* k: The number of graphs to follow.
* n: The order (number of vertices) of the current graph.
* Neighbor Lists: For each of the n vertices, the program reads a list containing
  * s: The number of neighbors.
  * IDs: n IDs of adjacent vertices (ranging from 1 to n).

# Implementation & Optimization
The engine analyzes several graph parameters, including degree sequences, bipartiteness (via custom DFS), and vertex eccentricity. To solve the NP-complete problem of vertex coloring, I implemented Greedy, LF (Largest First), and SLF (Saturation Largest First) heuristics.

In terms of optimization, the engine effectively handles connectivity analysis. By integrating a disjoint-set union (Union-Find) structure for component tracking, the program identifies connected components almost instantly. This efficiency proved particularly useful as it freed up the computational "budget" for more intensive tasks, such as the detection of C4 subgraphs and calculating the edges of the graph's complement.

# Features Summary
* Degree Sequence: Sorted vertex degree calculation.
* Connectivity:Efficient component counting and bipartiteness testing.
* Graph Coloring: Heuristic-based coloring (Greedy, LF, SLF).
* Cycle Detection: Counting unique C4 subgraphs.
* Complement Metrics: Determining the number of edges in the complement graph.

# Running the project
Requires Visual Studio's MSVC build tools (`cl.exe`) on Windows. From a PowerShell prompt in the project root:

```powershell
.\run.ps1                             # build, then run interactively (type input, Ctrl+Z + Enter for EOF)
.\run.ps1 -InputFile path\to\file.txt  # build, then run against one input file
.\run.ps1 -Tests                      # build + run the full test suite, checking time/memory limits
```

`run.ps1` auto-detects the MSVC environment (via `msvc_env.ps1`) even from a plain terminal, so a special "Developer Command Prompt" isn't required. If script execution is blocked, either run once as an admin/user:
`Set-ExecutionPolicy -Scope CurrentUser RemoteSigned`, or invoke with `powershell -ExecutionPolicy Bypass -File .\run.ps1 ...` each time.

# Test suite (`tests/`)
`tests/generate_tests.ps1` generates 16 input files covering distinct graph shapes and edge cases, not just size scaling:

* Structural correctness checks: a tree, a star (extreme degree skew), even/odd cycles (explicit bipartite true/false), an exact 4-vertex cycle (hand-verifiable C4 = 1), a dense clique, a complete bipartite graph (deliberately packed with 4-cycles), and a forest with thousands of disjoint components.
* `10_many_small_sparse_graphs`: 3,000 separate small/sparse graphs packed into one file, some multi-component — stresses total work across many graphs in one input, the scenario that originally required switching to Union-Find to stay within the time limit.
* Buckets bracketing the `LARGE_GRAPH = 100000` threshold (`graph.h`/`main.cpp`) from both sides, at different densities, since that's the exact boundary where the O(n·d²) common-neighbor C4-counting method used to time out before the size-gated DFS fallback (`number_of_subgraphs_for_large_graph`) was added.
* `16_known_bug_line_overflow`: **deliberately reproduces a known bug**, not a test-suite mistake. `main.cpp` reads each vertex's adjacency line into a fixed `char string[10000]` buffer via `std::cin.getline`; a vertex with roughly 1800+ neighbors produces a longer line, which `getline` silently truncates, corrupting the parse and crashing the program (access violation). This bucket is expected to crash until that's fixed (e.g. by reading with `std::getline` into a `std::string` instead of a fixed buffer).

`tests/run_benchmarks.ps1` builds the engine and runs it against every generated file, reporting wall-clock time and peak memory per test, flagging any that exceed configurable time/memory limits (also reachable via `.\run.ps1 -Tests -TimeLimitSeconds ... -MemoryLimitKB ...`).

None of this test data replays an original judge's actual test cases (that data isn't recoverable) — it's a reconstruction sized and shaped to exercise the same stress points and edge cases.

See `PROBLEM_REQUIREMENTS.md` for a reconstruction of the graded problem list this engine was built against, including two problems whose exact statements weren't recorded.
