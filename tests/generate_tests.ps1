<#
Generates local stress-test inputs for the graph engine in this repo,
matching the input format read by main.cpp:

    k
    n
    <s> <id1> <id2> ... <ids>     (repeated n times, one line per vertex)
    ... (repeated k times total, one n+adjacency block per graph)

- k: number of graphs in the file
- n: vertex count (per graph)
- s: neighbor count for that vertex, followed by s neighbor ids (1-indexed)

Parser quirks this must respect (see process_graph in main.cpp):
- Tokens are separated by exactly one space, and EVERY line -- including
  degree-0 lines -- must end with a trailing space. The first token (s) is
  found by scanning for a space with no fallback path, so a line with no
  space at all reads past the end of the buffer.
- STRING_SIZE is 10000 chars/line, so no vertex may have anywhere near
  that many neighbors listed on one line. Kept well under that here.
- Edges must appear symmetrically in both endpoints' lists -- the parser
  does not symmetrize automatically.

WHY these particular buckets: the first pass of this suite only varied
size/density of a single random-but-always-connected graph per file (k=1
always, built from a spanning tree so it could never be disconnected, and
never anything but "generic random"). That's blind to two things this repo's
actual judge apparently tested:

1. Different graph SHAPES, not just size -- trees, stars, cycles (even and
   odd, to hit both sides of the bipartite check), cliques, complete
   bipartite graphs (deliberately packed with 4-cycles), and forests with
   many small disjoint components. These exercise correctness edge cases
   (exact C4 counts, bipartite true/false, disconnected component counts)
   that a single "random dense/sparse" knob never reaches.
2. MANY graphs in one file (k > 1), each small and sparse -- this is the
   scenario described as needing Union-Find rather than something more
   expensive to avoid a per-problem time limit: lots of small/sparse
   graphs, so total work across the whole file matters, not just one big
   graph's worst case.

None of this replays the original judge data (unrecoverable) -- it's a
reconstruction of the same kinds of stress and edge cases.
#>

$ErrorActionPreference = "Stop"
$outDir = $PSScriptRoot

# ---------------------------------------------------------------------------
# Adjacency generators. Each returns an array of HashSet[int] (0-indexed).
# ---------------------------------------------------------------------------

function New-EmptyAdjacency([int]$n) {
    $adj = New-Object 'System.Collections.Generic.HashSet[int][]' $n
    for ($i = 0; $i -lt $n; $i++) { $adj[$i] = New-Object 'System.Collections.Generic.HashSet[int]' }
    return , $adj
}

function Add-Edge($adj, [int]$a, [int]$b) {
    if ($a -eq $b) { return }
    [void]$adj[$a].Add($b)
    [void]$adj[$b].Add($a)
}

# Random connected graph: spanning tree + extra random edges up to avgDegree.
function New-RandomAdjacency([int]$n, [double]$avgDegree, [System.Random]$rng) {
    $adj = New-EmptyAdjacency $n
    $maxNeighbors = [Math]::Min($n - 1, 1000)
    $targetEdges = [Math]::Max(0, [long]$n * $avgDegree / 2)

    for ($v = 1; $v -lt $n; $v++) {
        $u = $rng.Next(0, $v)
        Add-Edge $adj $v $u
    }

    $edgesAdded = $n - 1
    $maxAttempts = $targetEdges * 20 + 1000
    $attempts = 0
    while ($edgesAdded -lt $targetEdges -and $attempts -lt $maxAttempts) {
        $attempts++
        $a = $rng.Next(0, $n)
        $b = $rng.Next(0, $n)
        if ($a -eq $b) { continue }
        if ($adj[$a].Count -ge $maxNeighbors -or $adj[$b].Count -ge $maxNeighbors) { continue }
        if ($adj[$a].Contains($b)) { continue }
        Add-Edge $adj $a $b
        $edgesAdded++
    }
    return , $adj
}

# Random tree: low, mostly-uniform degree, single component, zero C4s,
# always bipartite.
function New-TreeAdjacency([int]$n, [System.Random]$rng) {
    $adj = New-EmptyAdjacency $n
    for ($v = 1; $v -lt $n; $v++) {
        $u = $rng.Next(0, $v)
        Add-Edge $adj $v $u
    }
    return , $adj
}

# Star: vertex 0 connected to everyone else. Extreme degree skew (one
# vertex has degree n-1, all others degree 1). Tree, so bipartite, zero C4.
function New-StarAdjacency([int]$n) {
    $adj = New-EmptyAdjacency $n
    for ($v = 1; $v -lt $n; $v++) { Add-Edge $adj 0 $v }
    return , $adj
}

# Simple cycle 0-1-2-...-(n-1)-0. 2-regular. Even n -> bipartite; odd n ->
# not bipartite (the classic odd-cycle case). Zero C4 unless n == 4.
function New-CycleAdjacency([int]$n) {
    $adj = New-EmptyAdjacency $n
    for ($v = 0; $v -lt $n; $v++) { Add-Edge $adj $v (($v + 1) % $n) }
    return , $adj
}

# Complete graph K_n: everyone connects to everyone. Worst case for
# coloring (needs exactly n colors) and for the O(n*d^2) C4-counting
# method (huge common-neighbor counts). Keep n small -- edge count is
# O(n^2).
function New-CompleteAdjacency([int]$n) {
    $adj = New-EmptyAdjacency $n
    for ($a = 0; $a -lt $n; $a++) {
        for ($b = $a + 1; $b -lt $n; $b++) { Add-Edge $adj $a $b }
    }
    return , $adj
}

# Complete bipartite K_{m,n}: partition A (size m) / B (size n), every
# cross edge present, no edge within a partition. Always bipartite, and
# deliberately packed with 4-cycles (any 2 vertices in A + 2 in B form
# one) -- a strong correctness+perf check for C4 counting specifically.
function New-CompleteBipartiteAdjacency([int]$m, [int]$n) {
    $total = $m + $n
    $adj = New-EmptyAdjacency $total
    for ($a = 0; $a -lt $m; $a++) {
        for ($b = $m; $b -lt $total; $b++) { Add-Edge $adj $a $b }
    }
    return , $adj
}

# Forest of many small, disjoint random trees. Deliberately produces a
# large connected-component count (stresses Union-Find / component
# counting and repeated bipartite-DFS restarts across many components in
# a single graph), unlike New-RandomAdjacency which is always one
# component by construction.
function New-ForestAdjacency([int]$n, [int]$minComponent, [int]$maxComponent, [System.Random]$rng) {
    $adj = New-EmptyAdjacency $n
    $idx = 0
    while ($idx -lt $n) {
        $size = $rng.Next($minComponent, $maxComponent + 1)
        if ($size -gt ($n - $idx)) { $size = $n - $idx }
        for ($v = 1; $v -lt $size; $v++) {
            $u = $rng.Next(0, $v)
            Add-Edge $adj ($idx + $v) ($idx + $u)
        }
        $idx += $size
    }
    return , $adj
}

# ---------------------------------------------------------------------------
# File writer: takes a list of adjacency arrays (one per graph) and writes
# the k-header + all graphs in the format process_graph expects.
# ---------------------------------------------------------------------------

function Write-GraphsToFile([string]$path, [System.Collections.Generic.HashSet[int][][]]$graphs) {
    $sw = New-Object System.IO.StreamWriter($path, $false)
    $sw.NewLine = "`n"
    try {
        $sw.WriteLine([string]$graphs.Count)
        foreach ($adj in $graphs) {
            $n = $adj.Length
            $sw.WriteLine([string]$n)
            for ($v = 0; $v -lt $n; $v++) {
                $neighbors = $adj[$v] | Sort-Object
                $tokens = New-Object System.Collections.Generic.List[string]
                $tokens.Add([string]$neighbors.Count)
                foreach ($u in $neighbors) { $tokens.Add([string]($u + 1)) }  # 1-indexed
                $sw.WriteLine(($tokens -join " ") + " ")
            }
        }
    }
    finally {
        $sw.Dispose()
    }
}

# ---------------------------------------------------------------------------
# Buckets
# ---------------------------------------------------------------------------

$manifest = @("name,type,graph_count,n,note")

function Emit([string]$name, [string]$type, $graphs, [string]$note) {
    $path = Join-Path $outDir "$name.txt"
    Write-GraphsToFile -path $path -graphs $graphs
    $sizeKb = [Math]::Round((Get-Item $path).Length / 1KB, 0)
    $totalN = ($graphs | ForEach-Object { $_.Length } | Measure-Object -Sum).Sum
    Write-Host "wrote $name.txt  (type=$type, graphs=$($graphs.Count), total_n=$totalN, ~$sizeKb KB)"
    $script:manifest += "$name,$type,$($graphs.Count),$totalN,""$note"""
}

# -- Structural edge cases (correctness, not just scale) --------------------

Emit "01_tiny_random" "random" @(, (New-RandomAdjacency 10 3 ([System.Random]::new(1)))) `
    "tiny sanity check, hand-verifiable"

Emit "02_tree_sparse" "tree" @(, (New-TreeAdjacency 2000 ([System.Random]::new(2)))) `
    "connected tree: 1 component, bipartite=T, zero C4 -- low/uniform degree sequence"

Emit "03_star" "star" @(, (New-StarAdjacency 1500)) `
    "extreme degree skew (hub degree n-1, rest degree 1): bipartite=T, zero C4, coloring needs only 2 colors. Kept under ~1500 so the hub's adjacency line stays under main.cpp's STRING_SIZE=10000 char buffer -- see bucket 16 for what happens past that limit."

Emit "04_cycle_even" "cycle" @(, (New-CycleAdjacency 5000)) `
    "2-regular even cycle: bipartite=T, zero C4 (n != 4)"

Emit "05_cycle_odd" "cycle" @(, (New-CycleAdjacency 4999)) `
    "2-regular odd cycle: bipartite=F -- explicit non-bipartite correctness check"

Emit "06_c4_exact" "cycle" @(, (New-CycleAdjacency 4)) `
    "4-cycle: exactly 1 C4 subgraph -- exact, hand-verifiable correctness check"

Emit "07_complete_small" "complete" @(, (New-CompleteAdjacency 150)) `
    "K150: dense worst case, needs exactly 150 colors, huge C4 count despite small n"

Emit "08_complete_bipartite" "complete-bipartite" @(, (New-CompleteBipartiteAdjacency 100 100)) `
    "K(100,100): always bipartite=T, deliberately packed with 4-cycles -- C4 correctness+perf stress"

Emit "09_forest_many_components" "forest" @(, (New-ForestAdjacency 50000 5 30 ([System.Random]::new(9)))) `
    "many (thousands of) small disjoint trees -- stresses component counting / repeated bipartite DFS restarts"

# -- Many graphs per file (the scenario that originally needed Union-Find) --

$manySmallSeed = [System.Random]::new(10)
$manySmallGraphs = New-Object 'System.Collections.Generic.List[System.Collections.Generic.HashSet[int][]]'
for ($i = 0; $i -lt 3000; $i++) {
    $n = $manySmallSeed.Next(30, 300)
    if ($i % 3 -eq 0) {
        # every third graph is an explicit forest (multiple components)
        $manySmallGraphs.Add((New-ForestAdjacency $n 5 20 $manySmallSeed))
    }
    else {
        $avgDeg = $manySmallSeed.Next(2, 5)
        $manySmallGraphs.Add((New-RandomAdjacency $n $avgDeg $manySmallSeed))
    }
}
Emit "10_many_small_sparse_graphs" "mixed-many" $manySmallGraphs.ToArray() `
    "3000 separate small/sparse graphs (some multi-component) in one file -- total-work-across-many-graphs stress, the Union-Find scenario"

# -- Size/density scaling around the LARGE_GRAPH=100000 threshold -----------

Emit "11_dense_medium" "random" @(, (New-RandomAdjacency 3000 150 ([System.Random]::new(11)))) `
    "dense, under LARGE_GRAPH -- stresses O(n*d^2) C4 counting"

Emit "12_sparse_medium" "random" @(, (New-RandomAdjacency 20000 6 ([System.Random]::new(12)))) `
    "large-ish but sparse, under LARGE_GRAPH"

Emit "13_dense_near_threshold" "random" @(, (New-RandomAdjacency 99000 30 ([System.Random]::new(13)))) `
    "just under LARGE_GRAPH, dense -- the original failure mode"

Emit "14_sparse_near_threshold" "random" @(, (New-RandomAdjacency 99000 4 ([System.Random]::new(14)))) `
    "just under LARGE_GRAPH, sparse -- contrast with #13"

Emit "15_over_threshold" "random" @(, (New-RandomAdjacency 150000 10 ([System.Random]::new(15)))) `
    "above LARGE_GRAPH -- engages the DFS C4 fallback"

# -- Known-limitation demonstration ------------------------------------------
#
# main.cpp's process_graph reads each vertex's line with:
#     char string[STRING_SIZE];               // STRING_SIZE = 10000
#     std::cin.getline(string, STRING_SIZE);
# std::istream::getline silently truncates (and fails) on any physical line
# longer than STRING_SIZE-1 chars -- it does NOT throw or skip cleanly, and
# the unread remainder of that line gets misread as the start of the next
# line. A vertex with roughly 1800+ neighbors (fewer if using 6-digit ids)
# produces a line over that limit. This was found by accident here: an
# earlier, larger version of the "star" bucket above (hub degree 4999)
# triggered exactly this and crashed the engine with an access violation
# (0xC0000005), not a clean "wrong answer" -- see the conversation this
# suite came from for the crash trace. This bucket reproduces it
# deliberately and on purpose, so it is EXPECTED to crash or misbehave
# until main.cpp is changed to read lines without a fixed-size buffer
# (e.g. std::getline into a std::string, or std::cin >> token-by-token).
Emit "16_known_bug_line_overflow" "star" @(, (New-StarAdjacency 3000)) `
    "KNOWN BUG: hub degree 2999 makes its adjacency line ~15000 chars, over STRING_SIZE=10000 in main.cpp -- expected to crash/misparse, not a generator error"

$manifest -join "`n" | Out-File -FilePath (Join-Path $outDir "manifest.csv") -Encoding utf8 -NoNewline
Write-Host ""
Write-Host "Done. See manifest.csv for bucket details."
