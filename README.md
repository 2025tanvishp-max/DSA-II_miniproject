
# Campus Navigation & Landmark Explorer

A console-based campus navigation system built with **C++17** and core **Data Structures and Algorithms**. It models a campus as an undirected, weighted graph and helps users explore landmarks, inspect connections, and find routes.

The project compares two navigation algorithms:

- **Breadth-First Search (BFS):** Finds a route with the fewest walkways.
- **Dijkstra's Algorithm:** Finds the route with the shortest total walking distance using a manually implemented **Min-Heap**.

---

## Table of Contents

- [Overview](#overview)
- [Problem Statement](#problem-statement)
- [Objectives](#objectives)
- [DSA Concepts Used](#dsa-concepts-used)
- [Campus Model](#campus-model)
- [How the Algorithms Work](#how-the-algorithms-work)
- [Features](#features)
- [Getting Started](#getting-started)
- [Application Menu](#application-menu)
- [Example Output](#example-output)
- [Complexity Analysis](#complexity-analysis)
- [Testing](#testing)
- [Error Handling](#error-handling)
- [Limitations and Future Improvements](#limitations-and-future-improvements)

## Overview

Finding a route around a large campus can be confusing, especially when the route with the fewest walkways is not necessarily the route with the shortest walking distance.

This application represents campus landmarks as **vertices** and walkways as **weighted edges**. Users can explore the campus and compare routes based on the number of walkways or total distance.

The project is implemented in a single C++ source file and does not require external libraries, APIs, a database, or a graphical interface.

## Problem Statement

New students and visitors may struggle to identify efficient routes between campus buildings. A useful navigation system should distinguish between:

1. The route that uses the **fewest walkways**.
2. The route that covers the **shortest total distance**.

These routes can be different. This project demonstrates that distinction by applying graph traversal and shortest-path algorithms to a campus map.

## Objectives

- Represent a campus using an adjacency-list graph.
- Use BFS to explore landmarks and find fewest-hop routes.
- Use Dijkstra's Algorithm to calculate shortest walking distances.
- Implement a Min-Heap from scratch and use it as Dijkstra's priority queue.
- Reconstruct and display routes in the correct order.
- Display the BFS traversal tree.
- Validate user input and handle invalid entries without crashing.

## DSA Concepts Used

| Concept | Role in the Project |
|---|---|
| Weighted Undirected Graph | Represents campus landmarks and two-way walkways |
| Adjacency List | Stores connected landmarks and distances |
| BFS | Finds routes with the fewest edges |
| Dijkstra's Algorithm | Finds the shortest total walking distance |
| Min-Heap | Efficiently selects the next closest landmark |
| Queue | Manages BFS traversal |
| Stack | Reconstructs routes in forward order |
| BFS Spanning Tree | Displays the landmark discovery hierarchy |

### Min-Heap Implementation

The project implements a Min-Heap manually rather than relying on the STL priority queue for Dijkstra's Algorithm.

For an element at array index `i`:

- Parent: `(i - 1) / 2`
- Left Child: `2 * i + 1`
- Right Child: `2 * i + 2`

| Operation | Description | Time Complexity |
|---|---|---|
| `push` | Inserts an item using heapify-up | O(log n) |
| `pop` | Removes the minimum item using heapify-down | O(log n) |
| `peek` | Reads the minimum item | O(1) |
| `empty` | Checks whether the heap is empty | O(1) |

### Graph Representation

Each edge stores a destination landmark and its walking distance:

```cpp
struct Edge {
    int to;
    int distance;
};

vector<vector<Edge>> campusGraph;
```

Since the graph is undirected, every walkway `(u, v, d)` is stored in both directions.

## Campus Model

The campus graph contains:

- **12 vertices:** Campus landmarks.
- **22 undirected edges:** Two-way walkways.
- **Edge weights:** Walking distances measured in meters.

| ID | Landmark | Connections |
|---|---|---|
| 1 | Main Gate | 3 |
| 2 | Admin Block | 5 |
| 3 | Library | 3 |
| 4 | Computer Science Block | 4 |
| 5 | Engineering Block | 3 |
| 6 | Canteen | 4 |
| 7 | Auditorium | 4 |
| 8 | Student Activity Center | 5 |
| 9 | Sports Ground | 3 |
| 10 | Hostel | 4 |
| 11 | Medical Center | 3 |
| 12 | Parking Area | 3 |

## How the Algorithms Work

### 1. Breadth-First Search (BFS)

BFS uses a queue to explore the graph level by level.

1. Start from the selected source landmark.
2. Visit its unvisited neighbours.
3. Add newly discovered landmarks to the queue.
4. Continue until the queue is empty.
5. Store parent information to reconstruct the route.

BFS minimises the **number of walkways**, not the total distance in meters. It also checks whether all 12 landmarks are reachable.

### 2. Dijkstra's Algorithm

Dijkstra's Algorithm finds the minimum total distance from a source landmark to other landmarks.

1. Initialise all distances to infinity and set the source distance to `0`.
2. Insert `(0, source)` into the Min-Heap.
3. Remove the entry with the smallest distance.
4. Check each neighbouring landmark.
5. If a shorter route is found, update its distance and parent, then insert the updated entry into the heap.
6. Continue until the heap is empty.

The implementation uses **lazy deletion**: outdated heap entries are skipped when popped instead of being removed immediately.

> Dijkstra's Algorithm requires non-negative edge weights. This condition is satisfied because walking distances cannot be negative.

### 3. Route Reconstruction

Both algorithms record parent information while exploring the graph. The application follows parent links backwards from the destination, pushes the landmarks onto a stack, and then pops them to display the route in forward order.

### 4. BFS Traversal Tree

The parent relationships recorded by BFS form a spanning tree rooted at the selected starting landmark. A recursive print function displays this tree using branch markers such as `├──` and `└──`.

## Features

- View the campus map.
- Explore the campus using BFS.
- Find the shortest-distance route using Dijkstra's Algorithm.
- Search for landmarks without case sensitivity.
- View the connections of a selected landmark.
- Display the BFS traversal tree.
- Compare BFS and Dijkstra routes.
- Handle invalid IDs, non-numeric input, empty searches, and end-of-input safely.

## Getting Started

### Prerequisites

- A C++17-compatible compiler.
- A terminal or command prompt.

No third-party libraries or package installations are required.

### Project Structure

```text
Campus-Navigation/
└── main.cpp
```

### Compile and Run

**macOS / Linux**

```bash
g++ -std=c++17 -Wall -Wextra main.cpp -o campus_navigation
./campus_navigation
```

**Windows**

```bash
g++ -std=c++17 -Wall -Wextra main.cpp -o campus_navigation.exe
campus_navigation.exe
```

## Application Menu

When the application starts, it displays the following menu:

```text
1. View Campus Map
2. Explore Campus (BFS)
3. Find Shortest Route (Dijkstra)
4. Search Landmark
5. View Landmark Connections
6. View Campus Traversal Tree
7. Compare Routes
8. Exit
```

Choose an option by entering its corresponding number.

## Example Output

### Find a Route: Main Gate to Canteen

**Source:** Main Gate (ID 1)  
**Destination:** Canteen (ID 6)

```text
Main Gate
   | 120 m
Admin Block
   | 90 m
Library
   | 140 m
Canteen

Total Walking Distance: 350 meters
Number of Walkways Traversed: 3
```

### Compare Routes: Main Gate to Sports Ground

```text
[1] BREADTH-FIRST SEARCH (BFS) ROUTE:
  Walkways (Hops)   : 3
  Total Distance    : 700 meters
  Path              : Main Gate -> Auditorium -> Student Activity Center -> Sports Ground

[2] DIJKSTRA'S ALGORITHM ROUTE:
  Walkways (Hops)   : 5
  Total Distance    : 580 meters
  Path              : Main Gate -> Admin Block -> Library -> Canteen -> Student Activity Center -> Sports Ground
```

**What this demonstrates:** The route with fewer walkways is not always the route with the shortest total distance. In this example, Dijkstra's route uses two additional walkways but saves **120 meters**.

### BFS Traversal Tree

Example traversal tree rooted at Main Gate (ID 1):

```text
[1] Main Gate
├── [2] Admin Block
│   ├── [3] Library
│   │   └── [6] Canteen
│   └── [4] Computer Science Block
│       └── [5] Engineering Block
├── [12] Parking Area
│   └── [11] Medical Center
│       └── [10] Hostel
└── [7] Auditorium
    └── [8] Student Activity Center
        └── [9] Sports Ground
```

## Complexity Analysis

Let:

- `V` = Number of vertices (landmarks).
- `E` = Number of edges (walkways).
- `L` = Length of the longest landmark name.

| Operation | Time Complexity | Space Complexity |
|---|---|---|
| Adjacency List Storage | O(V + E) | O(V + E) |
| Heap Push / Pop | O(log n) | O(n) |
| Dijkstra with Min-Heap | O((V + E) log V) | O(V + E) |
| Dijkstra with Array Scan | O(V²) | O(V) |
| BFS Traversal | O(V + E) | O(V) |
| BFS Spanning Tree | O(V + E) | O(V) |
| Route Reconstruction | O(V) | O(V) |
| Landmark Search | O(V × L) | O(V) |

**Dijkstra Space Note:** Because the implementation uses lazy deletion, the heap can contain up to O(E) entries. This is why the overall auxiliary space is stated as O(V + E).

## Testing

The project includes the following test scenarios:

| # | Test Type | Input | Expected Result |
|---|---|---|---|
| 1 | Normal Route | Option 3: `1 → 6` | 350 m and 3 walkways |
| 2 | Route Comparison | Option 7: `1 → 7` | See recorded project test values |
| 3 | Route Comparison | Option 7: `1 → 9` | BFS: 700 m, 3 hops; Dijkstra: 580 m, 5 hops |
| 4 | Edge Case | Option 3: `4 → 4` | Same source and destination; 0 m |
| 5 | Invalid ID | Enter `0` or `13` | ID range error |
| 6 | Invalid Input | Enter `abc` | Error message; no crash |
| 7 | Search | Search `lib`, then `LIBRARY` | Library found in both cases |
| 8 | Empty Search | Press Enter without a query | Empty-query error |
| 9 | Connectivity | Option 2, start at ID `1` | All 12 landmarks visited |
| 10 | Input Termination | End input without choosing option 8 | Application exits cleanly |

**Min-Heap Correctness Check:** The manually implemented Min-Heap version of Dijkstra was compared against an STL `priority_queue` implementation for all 144 source-destination pairs. The recorded result was that distances and paths matched.

## Error Handling

The application accounts for common input and routing errors:

- **Non-numeric input:** Clears the input error state, discards the invalid line, and displays a message.
- **Out-of-range landmark ID:** Accepts only IDs from 1 to 12.
- **Same source and destination:** Returns a distance of 0 meters.
- **Empty landmark search:** Requests a non-empty query.
- **Unreachable destination:** Displays a no-path message and avoids reconstructing an invalid route.
- **End of input:** Exits cleanly instead of entering an endless menu loop.

## Limitations and Future Improvements

### Current Limitations

1. The campus map is fixed in the source code.
2. Distances are measured in meters; elevation and crowd conditions are not considered.
3. Dijkstra's Algorithm supports non-negative edge weights only.

### Possible Future Improvements

1. Load campus landmarks and walkways from an external file.
2. Estimate walking time using an assumed walking speed (for example, 1.4 m/s).
3. Support closed walkways and automatically reroute users around them.
4. Add Bellman-Ford if future route costs need to support negative weights.

---

## Academic Context

| Detail | Information |
|---|---|
| Project | Campus Navigation & Landmark Explorer |
| Subject | Data Structures and Algorithms – II |
| Programming Language | C++17 |
| Application Type | Console-Based Mini Project |
