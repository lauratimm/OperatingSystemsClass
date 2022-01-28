// =============================================================================
// You must modify this file and then submit it for grading to D2L.
//Laura Timm
//3064647
//*** Referenced All Tutorial code for pthreads and barriers***
// =============================================================================

#include "detectPrimes.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <vector>
#include <unistd.h>

using namespace std;
pthread_mutex_t myMutex;
pthread_barrier_t myBarrier;
std::vector <int64_t> result;
std::vector <int64_t> num;
std::atomic<bool> global_finished;
std::atomic<bool> divisor;
std::atomic<int> counter = -1;
int nthreads;
std::vector <int64_t> primes;


// returns true if n is prime, otherwise returns false
// -----------------------------------------------------------------------------
// to get full credit for this assignment, you will need to adjust or even
// re-write the code in this function to make it multithreaded.
//static bool thread_task(void *i) {


static bool serial() {
    int64_t n = num[counter];
    //std::cout << "counter" << counter << "\n"<< std::endl;
    // handle trivial cases
    if (n < 2) {
        return false;
    }
    // 2 and 3 are primes
    if (n <= 3) {
        result.push_back(n);
        return false;
    }
    if (n % 2 == 0) {
        //std::cout << "num" << n << "\n"<< std::endl;
        return false;
    } // handle multiples of 2
    if (n % 3 == 0) {
        return false;
    } // handle multiples of 3
    return true;
}

void *task(void *tid) {

    //used from barrierExp2.cpp
    //repeat forever
    while (1) {
        // synchronize all threads
        int res = pthread_barrier_wait(&myBarrier);
        //serial task -- pick one thread using barrier
        if (res == PTHREAD_BARRIER_SERIAL_THREAD) {
            //check if there is a number set global variable
            if (counter < (int) num.size()) {
                bool prime = false;
                while (!prime) {
                    counter.fetch_add(1, memory_order_relaxed);
                    pthread_mutex_lock(&myMutex);
                    divisor = true;
                    pthread_mutex_unlock(&myMutex);
                    prime = serial();
                }
            }
        }
        //if no more numbers left: set global_finished=true to indicate to all threads to quit
        if (counter >= (int) num.size()) {
            global_finished = true;
        }
        // synchronize again before starting parallel work
        pthread_barrier_wait(&myBarrier);
        // run thread_task in parallel
        if (global_finished == false)       // if read was successful, run threads
        {
            //bool check = thread_task(tid);
            //std::cout << "check bool: " << check << "\n" << std::endl;
            //divide work for each thread
            int64_t n = num[counter];
            double end = sqrt(n);
            intptr_t id = (intptr_t) tid;
            int64_t start = 5 + (id * 6);

            while (start <= end && divisor) {

                if (n % start == 0) {
                  pthread_mutex_lock(&myMutex);
                  divisor = false;
                  pthread_mutex_unlock(&myMutex);
                }
                if (n % (start + 2) == 0) {
                  pthread_mutex_lock(&myMutex);
                  divisor = false;
                  pthread_mutex_unlock(&myMutex);
                }
                //increase by increments by multiples of thead 6
                start += 6 * nthreads;
            }
            //serial task – pick one thread using barrier
          int res = pthread_barrier_wait(&myBarrier);
          //serial task -- pick one thread using barrier
          if (res == PTHREAD_BARRIER_SERIAL_THREAD) {
//            std::cout << "num" << n << "\n"<< std::endl;
//            std::cout << "bool " << divisor << "\n"<< std::endl;
                if (divisor) {
                    //this is recording for all threads
                    result.push_back(n);
                }
            }


        } else {
            break;//if global_finished flag is set, break- exit thread
        }


    }
    pthread_exit(0);
}


// This function takes a list of numbers in nums[] and returns only numbers that
// are primes.
//
// The parameter n_threads indicates how many threads should be created to speed
// up the computation.
// -----------------------------------------------------------------------------
// You will most likely need to re-implement this function entirely.
// Note that the current implementation ignores n_threads. Your multithreaded
// implementation must use this parameter.
std::vector <int64_t>
detect_primes(const std::vector <int64_t> &nums, int n_threads) {
    num = nums;
    nthreads = n_threads;
    //prepare the memory for each thread
    pthread_t threads[n_threads];
    // create a barrier for total number of threads.
    pthread_barrier_init(&myBarrier, NULL, n_threads);

    //set global_finished = false
    global_finished = false;
    // create threads
    for (int64_t i = 0; i < n_threads; i++) {
        pthread_create(&threads[i], NULL, task, (void *) i);
    }
    // join threads
    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

// destroy barrier
    pthread_barrier_destroy(&myBarrier);

    return result;
}


//ORIGINAL CODE
//static bool is_prime(int64_t n)
//{
//  // handle trivial cases
//  if (n < 2) return false;
//  if (n <= 3) return true; // 2 and 3 are primes
//  if (n % 2 == 0) return false; // handle multiples of 2
//  if (n % 3 == 0) return false; // handle multiples of 3
//  // try to divide n by every number 5 .. sqrt(n)
//  int64_t i = 5;
//  int64_t max = sqrt(n);
//  while (i <= max) {
//    if (n % i == 0) return false;
//    if (n % (i + 2) == 0) return false;
//    i += 6;
//  }
//  // didn't find any divisors, so it must be a prime
//  return true;
//}