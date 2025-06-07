# Data_Structures_and_Algorithms_Assignment_3

This assignment focuses on implementing graph data structures and using them in the context of multi-layer neural networks, specifically for topological sorting.

## 1. Introduction

This assignment consists of two main tasks:

*   **Task 1:** Implement Graph data structures.
*   **Task 2:** Utilize the implemented graph data structure for backpropagation, specifically topological sorting.

## 2. Project Structure

The project's structure is similar to Assignment 2.

*   The source code for Task 1 is located in the `./include/graph` directory.

## 3. Task 1: Graph Data Structures

### 3.1. Overview

The graph data structure is designed with the following classes:

*   **IGraph:** This abstract class defines the API for graph implementations.
    *   Uses templates for flexible data types.
    *   All APIs are pure virtual methods.
*   **AbstractGraph:** An abstract class that partially implements the `IGraph` interface using an adjacency list representation.
*   **DGraphModel:** A concrete implementation of a directed graph, inheriting from `AbstractGraph`.
*   **UGraphModel:** A concrete implementation of an undirected graph, inheriting from `AbstractGraph`.

### 3.2. Key APIs

*   `add(T vertex)`: Adds a vertex to the graph.
*   `remove(T vertex)`: Removes a vertex and its associated edges.
*   `contains(T vertex)`: Checks if a vertex exists.
*   `connect(T from, T to, float weight = 0)`: Connects two vertices with a weighted edge.
*   `disconnect(T from, T to)`: Disconnects two vertices.
*   `connected(T from, T to)`: Checks if two vertices are connected.
*   `weight(T from, T to)`: Returns the weight of the edge between two vertices.
*   `getOutwardEdges(T from)`: Returns a list of outward edges from a vertex.
*   `getInwardEdges(T to)`: Returns a list of inward edges to a vertex.
*   `size()`: Returns the number of vertices.
*   `empty()`: Checks if the graph is empty.
*   `clear()`: Clears the graph.
*   `inDegree(T vertex)`: Returns the in-degree of a vertex.
*   `outDegree(T vertex)`: Returns the out-degree of a vertex.
*   `vertices()`: Returns a list of all vertices.
*   `toString()`: Returns a string representation of the graph.

## 4. Task 2: Topological Sorting

### 4.1. Overview

Task 2 focuses on implementing topological sorting, a crucial step in backpropagation for neural networks.  Topological sorting arranges the nodes of a Directed Acyclic Graph (DAG) in an order where for every directed edge (u, v), vertex u comes before vertex v in the ordering.

### 4.2. TopoSorter Class

*   `TopoSorter<T>`: This class implements topological sorting algorithms for DAGs.

### 4.3. Key Methods
*   `DLinkedList<T> sort(int mode = 0, bool sorted = true)`: Performs topological sorting using DFS or BFS.
    *  `mode`: 0 for DFS, 1 for BFS.
    *  `sorted`: if `true`, sorts the vertices in increasing order before processing.
* `DLinkedList<T> bfsSort(bool sorted = true)`: Performs the topological sort using BFS.
* `DLinkedList<T> dfsSort(bool sorted = true)`: Performs the topological sort using DFS.

### 4.4 Supporting classes
The project provides the prototype, and implementation instructions for `Stack`, `Queue`, and `DLinkedListSE`. These classes should be utilized to implement TopoSorter. If no implementation is provided then the original files should not be altered.
