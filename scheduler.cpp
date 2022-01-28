// you need to modify this file

#include "scheduler.h"
#include "common.h"
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <queue>
#include <vector>


// this is the function you should edit
//
// runs Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrival_times, and bursts
// output:
//   seq[] - will contain the execution sequence but trimmed to max_seq_len size
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
std::queue<int> rq, jq;
//remaining bursts for all processes these can be found with ID
std::vector <int64_t> remaining_bursts;

//This code was given during Lecture on November 24th
void add_to_seq( std::vector<int>& seq,int64_t max_seq_len,int item) {
    if ((int64_t) seq.size() < max_seq_len) {
        //if last item is not the same as item
        //std::cout << "item : " << item << " seq[seq.size() - 1]: " << seq[seq.size() - 1]<< "\n";
        if (seq.size()> 0) {
            if (seq[seq.size() - 1] != item) { seq.push_back(item); }
        }
        else{seq.push_back(item); }
    }
}


//}
//Contents of the Ready Queue (RQ) and Job Queue (JQ)

//**** referenced https://www.cplusplus.com/reference/queue/queue/front/ ***
//on how to use queues
void simulate_rr(int64_t quantum, int64_t max_seq_len, std::vector <Process> &processes, std::vector<int> &seq) {
    // time slice (time increments, max cycle )
    int64_t time_slice = quantum;
    size_t jq_ind = 0;

    //current time
    int64_t curr_time = 0;
    //remaining time slice
    int64_t remaining_burst=0;

    //currently executing process (-1 = idle)
    int cpu = -1;
    seq.clear();

    //add processes to job queue
    for (auto &p: processes) {jq.push(p.id);}
    for (auto &p: processes) {remaining_bursts.push_back(p.burst);}

    while (1) {
        //Print statements for testing
//        std::cout << "------------------------------------------ " << "\n";
//        std::cout << "current time : " << curr_time << "\n";
//        std::cout << "time_slice : " << time_slice << "\n";
//        std::cout << "cpu : " << cpu << "\n";
//        std::cout << "------------------------------------------ " << "\n";

        //if process in cpu is done, here because at 0
        if(cpu != -1 && time_slice ==0 ){

            if (remaining_burst ==0) {
                //mark process done
                //https://stackoverflow.com/questions/16219027/vector-erase-error
                //referenced this website
                jq.pop();
                processes[cpu].finish_time = curr_time;
                //set CPU to idle
                cpu = -1;
                //reset the time slice
                time_slice = quantum;
                //jobs remaining...
            }
            if  (remaining_burst !=0){
                //update burst time and add back to rq
                remaining_bursts[cpu] = remaining_burst;
                rq.push(cpu);
                //set CPU to idle
                cpu = -1;
                //reset the time slice
                time_slice = quantum;
                //jobs remaining...

            }

        }
        if(cpu != -1 && time_slice !=0 && remaining_burst ==0 ) {
            //std::cout << "remaining_burst remal : " << remaining_burst << " for CPU: " << cpu << "\n";
            //mark process done
            //https://stackoverflow.com/questions/16219027/vector-erase-error
            //referenced this website
            jq.pop();
            processes[cpu].finish_time = curr_time;
            //set CPU to idle
            cpu = -1;
            //reset the time slice
            time_slice = quantum;
            //jobs remaining...
        }

        //if a new process arriving
        if (jq_ind < processes.size() && processes[jq_ind].arrival_time == curr_time) {
            //add new process to RQ
            rq.push(jq_ind);
            //increase index
            jq_ind ++;
            continue;
        }

//       //if CPU is free and RQ not empty
        if (((cpu == -1) && (rq.size()) > 0)) {
//          // move process from RQ to CPU
            //set CPU index
            cpu = rq.front();
            rq.pop();
            if(processes[cpu].start_time == -1) {
                processes[cpu].start_time = curr_time;
            }
            //reset time slice
            time_slice = quantum;
            remaining_burst = remaining_bursts[cpu];

        }
        if (jq.size() == 0) break;
        //std::cout << "cpu : " << cpu << "\n";
        add_to_seq(seq, max_seq_len, cpu);

        // if process in cpu not done add back to the rq
        if(cpu != -1 && remaining_burst!= 0){

            //update burst time and add back to rq
            remaining_burst = remaining_burst - 1;
        }


        curr_time++;
        time_slice--;

        //for testing
      //if (curr_time == 1)break;
    }

}