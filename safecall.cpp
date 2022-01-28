#include "safecall.h"
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <chrono>
#include <sys/wait.h>
#include <thread>


using namespace std;

// ====================================================================
// this is the function you need to reimplement
// ====================================================================
// Assignment #2
// Author: Laura Timm
// Student ID: 30064647
// Date: October 15 2021

static constexpr auto FILENAME= "/tmp/timm-safecall.txt";

int safecall(int i) {
  // un able to mkdir in /tmp *****
  // remove temporary file
  unlink(FILENAME);

  // error handling of the fork
  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "ERROR: Fork failed\n";
    exit(-1);
  }
  //std::cout << pid << "= PID";

  // in child process:
  if (pid == 0) {
    int result = unsafe(i);
    // write result to temporary file -- "wb" will create bit file
    FILE *fp = fopen(FILENAME, "wb");

    if (fp == NULL) {
      std::cerr << "ERROR: creating file\n";
      exit(-1);
    }
    // write contents to file
    fwrite(&result, sizeof(result), 1, fp);
    // close file and exit
    fclose(fp);
    exit(0);
  }

  // in parent process:
  if (pid > 0) {
    //save current time
    auto start_time = std::chrono::steady_clock::now();
    //   busy loop:
    while (1) {
      //     sleep for 0.000001s
      std::this_thread::sleep_for(std::chrono::microseconds(1));
      auto pid_wait = waitpid (pid, NULL, WNOHANG);
      // if waitpid successful
      if (pid_wait > 0) {
        int buffer;
        //  read results from file -- "r" read only
         FILE * fp = fopen(FILENAME, "rb");
         //if reading results was successful, return result if no file return -2
         if(fp == NULL){
           std::cerr << "ERROR: file could not be opened\n";
            return -2;
         }
         fread(&buffer, sizeof(buffer), 1, fp);
        fclose(fp);
        return buffer;

      }
      //otherwise return -2 unsuccessful p_wait = -1 wont exc if false
  if(false) {
    if (pid_wait < 0) {
      cout << pid_wait << " wait < 0 check \n";
      return -2;
    }
  }
      //get elapsed time = curr_time - start_time
      auto curr_time = std::chrono::steady_clock::now();
      double elapsed = std::chrono::duration_cast<std::chrono::microseconds>
                               (curr_time - start_time).count() / 1000000.0;
      //std::cout << elapsed << "\n";

      if (elapsed > 1.0) {
        kill(pid, SIGKILL);
        // wait until kill is finished
        while(wait(NULL) > 0){;}
        return -1;
      }

    }

  }
  return (0);
}


