// ======================================================================
// You must modify this file and then submit it for grading to D2L.
// ======================================================================
//
// count_pi() calculates the number of pixels that fall into a circle
// using the algorithm explained here:
//
// https://en.wikipedia.org/wiki/Approximations_of_%CF%80
//
// count_pixels() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create
//
// Currently the function ignores the n_threads parameter. Your job is to
// parallelize the function so that it uses n_threads threads to do
// the computation.

#include "calcpi.h"
#include <cmath>
#include <thread>
#include <unistd.h>
#include <math.h>
#include <iostream>


//global thread doesnt alter
double rsq;
//thread inserts count to result[thread_id]
double result[256];

// thread function
void count_pixels_threads(int radius, double start, double end, int thread_id) {
  uint64_t count=0;
  //std::cout << "Thread #" << thread_id << " " <<  std::endl;
  for(double x = start+1; x <= end ; x++) {
  // std::cout << x << std::endl;
   for (double y = 0; y <= radius; y++) {
      if (x * x + y * y <= rsq) count++;
    }
  }
  // Store result to id index
  result[thread_id] = count;
}

uint64_t count_pixels(int r, int n_threads){
  rsq = double(r) * r;
  double partition  = floor(r/n_threads);

  // create threads
  std::thread threadPool[n_threads];
  for (int i = 0; i < n_threads; i++) {
      double start = i * partition;
      double end;
        if (i == n_threads - 1){
          end = r;
        }
        else{
         end =  (i+1) * partition;
        }
      threadPool[i] = std::thread(count_pixels_threads, r, start, end, i);
  }

  // join threads
  for (int i = 0; i < n_threads; i++) {
    threadPool[i].join();
  }
  // Add up result array
  long total = 0;
  for (int i = 0; i < n_threads; i++)
    total += result[i];
  //std::cout << i << result[i] << " " <<  std::endl;

  return total * 4 + 1;
}

