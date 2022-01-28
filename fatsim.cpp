// -------------------------------------------------------------------------------------
// CPSC 457 - Laura Timm
// -------------------------------------------------------------------------------------
//
// (c) 2021, Pavol Federl, pfederl@ucalgary.ca
// Do not distribute this file.

#include "fatsim.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>


void PrintStack(std::stack<std::pair<long, long>> s) {
  std::cout << "stack: ";
  while (!s.empty()) {
    long x = s.top().first;
    long y = s.top().second;
    s.pop();
    std::cout << x << ", "<< y << " ";
  }
  std::cout << "\n";
}

// Graph class represents a directed graph
// using adjacency list representation
//***This code was use from Zilin Ye's Asn 5 dfs recursive.cpp with slight***
//***modifications created by myself***
class Graph {
public:
    std::vector<bool> visited;
    std::vector<long> end_nodes, result;
    std::vector <std::vector<long>> adj;

    // populate the Graph with initial values
    Graph(int size) {
      //intitialize with false
      visited.assign(size, false);
      //populate with empty vectors
      adj.assign(size, std::vector<long>());
    }

    // function to add an edge to graph
    void addEdge(long v, long w) {
      adj[v].push_back(w);    // Add w to v’s list.
    }
    // **** end of copied code ****

    // DFS recursive traversal of the vertices reachable from startNode
    //****skeleton code for DFS from Zilin Ye's tutorial notes****
    //chain wrong tree you need to subtract number
    void DFS(long startNode) {
      //  initialize an empty stack
      std::stack<std::pair<long, long>> stack;
      long counter = 1;
      long max = 0;

      // push the start_node into empty stack
      stack.push({startNode, counter});
      // Mark the start node as visited
      visited[startNode] = true;

      //  while the stack is not empty:
      while (!stack.empty()) {

        //  next_node = pop the top entry from the stack
        long next_node = stack.top().first;
        counter = stack.top().second;
        stack.pop();

        //  mark next_node as visited
        visited[next_node] = true;

        //add unvisited adj nodes to stack
        for (auto n: adj[next_node]) {
          if (!visited[n]) {
            stack.push({n, (counter+1)});
          }
        }

        //at the end of the branch go back to branch node
        if (adj[next_node].empty()) {
          if (max < counter) {
          max = counter;
       }
        }
      }
      result.push_back(max);
    }
};

// reimplement this function
std::vector<long> fat_check(const std::vector<long> &fat) {
  // Create a graph with size of fat
  Graph g((int) fat.size());

  //addEdges to Graph
  for (int i = 0; i < (int) fat.size(); i++) {
    if (fat[i] == -1) { g.end_nodes.push_back(i); }
    else { g.addEdge(fat[i], i); }
  }
  for (auto index: g.end_nodes) {
    g.DFS(index);
  }

  //https://www.journaldev.com/37678/sorting-a-vector-in-c-plus-plus
  //referenced to use the sort function
  if ((int) g.result.size() > 1) {
    std::sort(g.result.begin(), g.result.end());
  }
  return g.result;

}

