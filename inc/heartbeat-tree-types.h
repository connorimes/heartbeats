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
} _heartbeat_record_t;

typedef struct {
  char valid;
  uint64_t counter;
#ifdef HEARTBEAT_USE_PTHREADS_LOCK
  pthread_mutex_t mutex;
#endif

  // data
  _heartbeat_time_data td;
} _heartbeat_shared_data;

typedef struct {
  char valid;
  uint64_t counter;

  // data
  _heartbeat_time_data td;
  _heartbeat_work_data wd;

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
