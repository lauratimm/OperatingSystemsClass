// -------------------------------------------------------------------------------------
// this is the only file you need to edit
// -------------------------------------------------------------------------------------
//
// (c) 2021, Pavol Federl, pfederl@ucalgary.ca
// Do not distribute this file.

#include "memsim.h"
#include <cassert>
#include <iostream>
#include <set>
#include <unordered_map>
#include <list>
#include <iterator>

using namespace std;

struct Partition {
    int tag;
    int64_t size, addr;
};
typedef std::list<Partition>::iterator PartitionRef;

// helper function for printing a list
void printList(list <Partition> &all_blocks) {
    for (PartitionRef it = all_blocks.begin(); it != all_blocks.end(); it++) {
        cout << "tag: " << it->tag << ", " << "size: " << it->size << ", " << "addr: " << it->addr << endl;
    }
}

// I recommend you implement the simulator as a class. This is only a suggestion.
// If you decide not to use this class, please remove it before submitting it.
struct Simulator {
    // all partitions, in a double linked list
    std::list <Partition> all_blocks;
    std::unordered_map<long, std::vector<PartitionRef>> tagged_blocks;
    int64_t myPageSize;
    int added_pages = 0;

    Simulator(int64_t page_size) {
        myPageSize = page_size;
    }

    void allocate(int tag, int size) {
        bool fitted = false;
        Partition newPartition = Partition();
        newPartition.tag = tag;
        newPartition.size = size;

        //***code given during Help session Dec 10 by Prof. ***
        auto best_block = all_blocks.end();

        for (auto block = all_blocks.begin(); block != all_blocks.end(); block++) {
            if (block->tag == -1 && block->size >= size)
                if (best_block == all_blocks.end() || block->size > best_block->size)
                    best_block = block;
        }
        while (!fitted) {
            if (best_block == all_blocks.end()) {
                added_pages++;
                //allocate more memory to the end
                PartitionRef it = prev(all_blocks.end());

                if (it->tag == -1) {
                    it->size += myPageSize;
                } else {
                    Partition newPage = Partition();
                    newPage.tag = -1;
                    newPage.size = myPageSize;

                    if (all_blocks.size() == 0) {
                        newPage.addr = 0;
                    } else {
                        newPage.addr = it->addr + it->size;
                    }
                    all_blocks.push_back(newPage);
                    it = prev(all_blocks.end());
                }
                best_block = it;
            }


            //allocate and split best block
            if ((best_block->size > newPartition.size)) {
                fitted = true;
                int64_t oldsize = best_block->size;
                newPartition.addr = best_block->addr;
                all_blocks.insert(best_block, newPartition);\
                tagged_blocks[tag].push_back(best_block);

                  std::cout << "tagged_blocks LIST" << "\n";
    for(const auto& [key, v] : tagged_blocks){
        std::cout << key << ": ";
        std::cout << "\n";
    }
                next(best_block);
                best_block->size = oldsize - newPartition.size;
                best_block->addr = newPartition.addr + newPartition.size;
            }
          best_block = all_blocks.end();
        }
      //cout << "********* List All Blocks **********" << endl;
      //printList(all_blocks);
    }

    void deallocate(int tag) {
        //***code given during Help session Dec 10 by Prof. ***
        for (PartitionRef block = tagged_blocks[tag].begin(); block != tagged_blocks[tag].end(); block++) {
            block->tag = -1;
            
            //merge left
            if (block != all_blocks.begin() && prev(block)->tag == -1) {
                block->addr = prev(block)->addr;
                block->size += prev(block)->size;
                all_blocks.erase(prev(block));

            }//merge right
            if (block != all_blocks.end() && next(block)->tag == -1) {
                block->addr = block->addr;
                block->size += next(block)->size;
                all_blocks.erase(next(block));
            }
          tagged_blocks[tag].clear;
        }
      //cout << "********* Dealloc List **********" << endl;
      //printList(all_blocks);
    }

    MemSimResult getStats() {
        // let's guess the result... :)
      MemSimResult result;
      if (!all_blocks.empty()) {
        auto best_block = all_blocks.end();
        for (auto block = all_blocks.begin(); block != all_blocks.end(); block++) {
          if (block->tag == -1)
            if (best_block == all_blocks.end() || block->size > best_block->size)
              best_block = block;

          result.max_free_partition_size = best_block->size;
          result.max_free_partition_address = best_block->addr;
        }
      }
      else {
        result.max_free_partition_size = 0;
        result.max_free_partition_address = 0;
      }



        result.n_pages_requested = added_pages;
        return result;
    }

    void check_consistency() {

        // you do not need to implement this method at all - this is just my suggestion
        // to help you with debugging

        // mem_sim() calls this after every request to make sure all data structures
        // are consistent. Since this will probablly slow down your code, you should
        // disable calling this in the mem_sim() function below before submitting
        // your code for grading.

        // here are some suggestions for consistency checks (see appendix also):

        // make sure the sum of all partition sizes in your linked list is
        // the same as number of page requests * page_size

        // make sure your addresses are correct

        // make sure the number of all partitions in your tag data structure +
        // number of partitions in your free blocks is the same as the size
        // of the linked list

        // make sure that every free partition is in free blocks

        // make sure that every partition in free_blocks is actually free

        // make sure that none of the partition sizes or addresses are < 1
    }
};

// re-implement the following function
// ===================================
// parameters:
//    page_size: integer in range [1..1,000,000]
//    requests: array of requests
// return:
//    some statistics at the end of simulation
MemSimResult mem_sim(int64_t page_size, const std::vector <Request> &requests) {
    // if you decide to use the simulator class, you probably do not need to make
    // any changes to the code below, except to uncomment the call to check_consistency()
    // before submitting your code
    Simulator sim(page_size);
    for (const auto &req: requests) {
        if (req.tag < 0) {
            sim.deallocate(-req.tag);
        } else {
            sim.allocate(req.tag, req.size);
        }
        sim.check_consistency();
    }

    return sim.getStats();
}
