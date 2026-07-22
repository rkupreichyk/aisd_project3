# Reconstructed problem list

Context: the judge scored 10 problems, each worth 10% of the project grade.
Each problem had its own ~10 sub-tests with time/memory limits; failing *any*
sub-test on time/memory (even if the output was correct) zeroed the whole
10% for that problem — partial-credit only applied *within* a problem's
sub-tests (see the screenshot table: 73% average across 10 accepted tests),
never across problems.

The program prints exactly 10 lines per graph (see `answers()` in
`main.cpp`), which lines up 1:1 with the 10 graded problems. 8 of the 10 are
fully implemented and clearly identifiable from the code. Two are still
stub lines (`std::cout << "?\n";`) — these are the two you don't remember.

| # | Output line | Problem | Status |
|---|-------------|---------|--------|
| 1 | degree sequence (desc) | Compute the degree sequence of the graph | done — merge sort |
| 2 | integer | Count connected components | done — Union-Find |
| 3 | `T`/`F` | Decide if the graph is bipartite | done — DFS 2-coloring |
| 4 | `?` | **UNKNOWN — see hypothesis below** | not implemented |
| 5 | `?` | **UNKNOWN — see hypothesis below** | not implemented |
| 6 | coloring | Greedy vertex coloring | done |
| 7 | coloring | LF (Largest First) vertex coloring | done |
| 8 | coloring | SLF/DSATUR vertex coloring (graphs < 1000 vertices only) | done |
| 9 | integer | Count C4 (4-cycle) subgraphs | done — two algorithms, gated by `LARGE_GRAPH = 100000` |
| 10 | integer | Count edges in the complement graph | done — computed during parsing |

## Hypothesis for problems #4 and #5

The README (written by you, presumably closer to when this was fresh)
says:

> The engine analyzes several graph parameters, including degree
> sequences, bipartiteness (via custom DFS), and **vertex eccentricity**.

"Vertex eccentricity" is never implemented anywhere in the code, and it
sits — thematically — right where the two `?` stubs are: between
"bipartite" and "coloring". The two most standard eccentricity-derived
problems that get taught/tested as a *pair* are:

- **Problem #4: Graph radius** — `min` over vertices of `eccentricity(v)`,
  where `eccentricity(v) = max` shortest-path distance from `v` to any
  other reachable vertex (computed via BFS from every vertex, or from
  every vertex per connected component if the graph can be disconnected).
- **Problem #5: Graph diameter** — `max` over vertices of `eccentricity(v)`.

Both are typically computed with the same routine: BFS from each vertex
to get its eccentricity, then reduce with min/max across all vertices.
Watch out for:
- **Disconnected graphs** — since problem #2 already proves the graph can
  have >1 component, radius/diameter need a defined convention for that
  case (often: compute per-component, or print a sentinel like `-1`).
  Given the existing "?" convention, printing something explicit for the
  disconnected case would match the project's style.
- **Complexity** — naive all-pairs BFS is `O(V*(V+E))`. For the
  `LARGE_GRAPH` (>100000 vertices) cases that already forced a second
  algorithm for C4-counting, plain eccentricity-for-every-vertex would
  likely need the same kind of size-gated fallback (e.g. sampling a
  handful of vertices instead of all of them, or skipping with `?` above
  a threshold — following the exact pattern already used for
  `SLFColoring` (`< SMALL_GRAPH`) and C4 counting (`> LARGE_GRAPH`)).

**This is a reconstruction, not a certainty** — you mentioned the last two
felt "linked to graph coloring" rather than eccentricity. Given greedy/LF/
SLF are already fully implemented and accounted for (lines 6-8), the two
remaining coloring-adjacent problems that would plausibly follow them on a
course problem sheet are things like:

- **Chromatic number** (exact, e.g. via backtracking, not a heuristic)
- **Maximum independent set** / **clique number** (often bundled with
  coloring assignments since they're all related via the complement graph
  — and this project already computes complement edge counts, which fits)

If you have the original course/judge problem statements (PDF, problem
IDs, judge name), that would resolve this for certain — otherwise, the
eccentricity hypothesis is the stronger bet purely from evidence already
sitting in this repo (README text + the exact position of the stubs).

## What to do with this

1. Try to track down the original problem set (course page, judge
   archive, email) — that's the only way to be *certain* rather than
   inferring from two leftover `?` marks.
2. If eccentricity is confirmed: implement BFS-based eccentricity per
   vertex, then radius = min, diameter = max, replacing the two `"?\n"`
   lines in `answers()` (`main.cpp:68-69`).
3. If it's coloring-derived instead: chromatic number and
   independent-set/clique-number are the most likely candidates given
   what's already built.
