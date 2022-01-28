// this is the ONLY file you should edit and submit to D2L

#include "deadlock_detector.h"
#include "common.h"
#include <iostream>
#include <iterator>
#include <atomic>
#include <unordered_map>

using namespace std;
using VS = std::vector<std::string>;

/// this is the function you need to (re)implement
///
/// parameter edges[] contains a list of request- and assignment- edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge. As soon as you detect a deadlock, your function
/// needs to stop processing edges and return an instance of Result structure
/// with edge_index set to the index that caused the deadlock, and dl_procs set
/// to contain with names of processes that are in the deadlock.
///
/// To indicate no deadlock was detected after processing all edges, you must
/// return Result with edge_index = -1 and empty dl_procs[].
///

//If no deadlock is detected after processing all edges,
// your function will indicate this by
// returning Result with edge_index=-1 and an empty dl_procs[]

class Graph {
public:
    std::unordered_map <std::string, std::vector<std::string>> adj_list;
    std::unordered_map<std::string, int> out_counts, out;
    std::vector <string> zeros;
    std::atomic<int> nodeCounter = -1;
    std::vector <std::string> p;

    void addEdge(std::vector <std::string> words) {

        nodeCounter.fetch_add(1, memory_order_relaxed);
        //to add to list adj_list["KEY"] = "value"; KEY = NODE/ VALUE = [Incoming Nodes]
        std::string process = words[0];
        std::string resource = "*" + words[2];
        // if <- use list adj_list["P:" + index[0]] = "R:" + index[2]
        if (words[1] == "<-") {
            adj_list[process].push_back(resource);
            if (!out_counts[process]) {
                out_counts[process] = 0;
            }
            if (!out_counts[resource]) {
                out_counts[resource] = 1;
            } else {
                out_counts[resource]++;
            }
        }
        // if -> use list out_counts["P:" +index[0]] = 1 or ++ current value;
        if (words[1] == "->") {
            adj_list[resource].push_back(process);
            if (!out_counts[process]) {
                out_counts[process] = 1;
            } else {
                out_counts[process]++;
            }
            if (!out_counts[resource]) {
                out_counts[resource] = 0;
            }
        }
    }

    bool top_sort() {
        //int nodes_removed = 0;
        //bool deadlock = false;
        //out = out_counts # copy out_counts so that we can modify it
        zeros.clear();
        out = out_counts;
        //zeros[] = find all nodes in graph with outdegree == 0
        //https://en.cppreference.com/w/cpp/container/unordered_map
        //used to reference how to iterate through an unordered map
        for (const auto &[key, v]: out) {
            if (v == 0) {
                zeros.push_back(key);
            }
        }
        //used the pseudo code from ASN 4 document
        //while zeros is not empty:
        size_t counter = 0;
        while (!zeros.empty()) {
            counter++;
            //n = remove one entry from zeros[]
            string n = zeros.back();
            //out.erase(n);
            zeros.pop_back();
            //for every n2 of adj_list[n]:
            for (auto &n2: adj_list[n]) {
                //Adj[n] = [ list ] this list corresponds without
                if (--out[n2] == 0) {
                    //append n2 to zeros[]
                    zeros.push_back(n2);
                    //out.erase(n2);
                }
            }
        }
        // if (nodes left (check if in out but no zeros) = cycle) deadlock = true

        if (counter == out.size()) return false;

        //deadlock = true;
        //https://en.cppreference.com/w/cpp/container/unordered_map
        //used to reference how to iterate through an unordered map
        for (const auto&[process, v]: out) {
            if (v == 0) continue;

            if (process[0] != '*') {
                p.push_back(process);
            }
        }
        //}
        //else all nodes removed deadlock = false
        //return deadlock;
        return true;

    }
}
        graph;


Result detect_deadlock(const std::vector <std::string> &edges) {

    std::vector <std::string> words;
    Result result;
    Graph g;
//referenced assignment 2 on how to parse through lines 
    for (std::string line: edges) {
        // if there is lines call split into words
        words = split(line);
        //this will add to dictionary
        g.addEdge(words);

        //if deadlock cycle returns values
        if (g.top_sort()) {
            result.dl_procs = g.p;
            result.edge_index = g.nodeCounter;
            return result;
        }
    }
    // if no deadlock
    result.edge_index = -1;
    return result;

}
//    std::cout << "ADJACENCY LIST" << "\n";
//    for (const auto&[key, v]: g.adj_list) {
//        std::cout << key << ": ";
//        std::copy(v.begin(), v.end(), std::ostream_iterator<string>(std::cout, " "));
//        std::cout << "\n";
//    }

//    std::cout << "OUT_COUNTS LIST" << "\n";
//    for(const auto& [key, v] : g.out_counts){
//        std::cout << key << ": ";
//        std::cout << g.out_counts[key];
//        std::cout << "\n";
//    }
//        std::cout << "OUT LIST" << "\n";
//        for (const auto&[key, v]: out) {
//            std::cout << key << ": ";
//            std::cout << out[key];
//            std::cout << "\n";
//        }
//        std::cout << "ZEROS LIST" << "\n";
//        for (auto values: zeros) {
//            std::cout << values;
//            std::cout << "\n";
//        }
