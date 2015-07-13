/**
 *  Example of pipelined heartbeats.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EM_DEFAULT
#include "heartbeat-tree-accuracy-power.h"

unsigned long long energy = 0;
unsigned long long get_energy(void* ref_arg) {
  return energy += 1000000;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage:\n");
    printf("  %s <iterations>\n", argv[0]);
    return -1;
  }

  int i;
  const int iterations = atoi(argv[1]);

  // initialize heartbeats
  heartbeat_t* heart = heartbeat_acc_pow_init(NULL, 20, 20, "heartbeat.log", &get_energy, NULL);
  heartbeat_t* heart_recv = heartbeat_acc_pow_init(heart, 20, 20, "heartbeat_recv.log", &get_energy, NULL);
  heartbeat_t* heart_work = heartbeat_acc_pow_init(heart, 20, 20, "heartbeat_work.log", &get_energy, NULL);
  heartbeat_t* heart_send = heartbeat_acc_pow_init(heart, 20, 20, "heartbeat_send.log", &get_energy, NULL);
  usleep(1000);

  for(i = 0; i < iterations; i++) {
    // receive data
    usleep(100000);
    heartbeat_acc(heart_recv, i, 0, 0.0, heart_send);
    // process data
    usleep(100000);
    heartbeat_acc(heart_work, i, 1, 1.0, heart_recv);
    // send data
    usleep(100000);
    heartbeat_acc(heart_send, i, 0, 0.0, heart_work);
    // complete iteration
    heartbeat_acc(heart, i, 1, 1.0, NULL);
  }

  // cleanup heartbeats
  heartbeat_finish(heart_recv);
  heartbeat_finish(heart_work);
  heartbeat_finish(heart_send);
  heartbeat_finish(heart);
  return 0;
}
