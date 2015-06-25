/**
 * Heartbeat API type definitions.
 *
 * @author Connor Imes
 */
#ifndef _HEARTBEAT_TYPES_H_
#define _HEARTBEAT_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#ifdef HEARTBEAT_USE_PTHREADS_LOCK
#include <pthread.h>
#endif
#include <energymon/em-generic.h>

typedef struct {
  int64_t last_timestamp;
  int64_t total_time;
  int64_t window_time;
} _heartbeat_time_data;

typedef struct {
  uint64_t total_work;
  uint64_t window_work;
} _heartbeat_work_data;

typedef struct {
  double total_accuracy;
  double window_accuracy;
} _heartbeat_accuracy_data;

typedef struct {
  unsigned int num_energy_impls;
  em_impl* energy_impls;
} _heartbeat_energy_resource;

typedef struct {
  double last_energy;
  double total_energy;
  double window_energy;
} _heartbeat_energy_data;

typedef struct {
  /*
   * Local values are since the last time this heartbeat was issued.
   * Shared values are since last local or sibling heartbeat.
   */
  uint64_t id;
  uint64_t shared_id;
  uint64_t user_tag;
  uint64_t timestamp;

  uint64_t work;
  int64_t latency;
  double global_perf;
  double window_perf;
  double instant_perf;

  double accuracy;
  double global_acc;
  double window_acc;
  double instant_acc;

  double energy;
  double global_pwr;
  double window_pwr;
  double instant_pwr;
} _heartbeat_record_t;

typedef struct {
  char valid;
  uint64_t counter;
#ifdef HEARTBEAT_USE_PTHREADS_LOCK
  pthread_mutex_t mutex;
#endif

  // data
  _heartbeat_time_data td;
  _heartbeat_energy_resource er;
} _heartbeat_shared_data;

typedef struct {
  char valid;
  uint64_t counter;

  // data
  _heartbeat_time_data td;
  _heartbeat_work_data wd;
  _heartbeat_accuracy_data ad;
  _heartbeat_energy_data ed;

  // logging
  FILE* text_file;
  _heartbeat_record_t* log;
  uint64_t buffer_depth;
  uint64_t buffer_index;
  uint64_t read_index;
} _heartbeat_local_data;

typedef struct _heartbeat_t {
  struct _heartbeat_t* parent;
  uint64_t window_size;
  _heartbeat_shared_data* sd;
  _heartbeat_local_data ld;
} _heartbeat_t;

typedef _heartbeat_t heartbeat_t;
typedef _heartbeat_record_t heartbeat_record_t;

#ifdef __cplusplus
}
#endif

#endif
