/**
 * Implementation of heartbeat-tree-accuracy-power.h
 *
 * @author Connor Imes
 */
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <energymon/em-generic.h>
#include "heartbeat-tree-accuracy-power.h"

#define __STDC_FORMAT_MACROS

#ifndef HEARTBEAT_ACCURACY_DEFAULT
  #define HEARTBEAT_ACCURACY_DEFAULT 0.0
#endif

static inline void finish_energy_readings(uint64_t num_energy_impls,
                                          em_impl* energy_impls) {
  uint64_t i;
  char energy_source[100];
  for (i = 0; energy_impls != NULL && i < num_energy_impls; i++) {
    if (energy_impls[i].ffinish != NULL) {
      energy_impls[i].fsource(energy_source);
      if (energy_impls[i].ffinish(&energy_impls[i])) {
        fprintf(stderr, "Error finishing energy reading from: %s\n",
                energy_source);
      } else {
        printf("Finished energy reading from: %s\n", energy_source);
      }
    }
  }
}

static inline void init_time_data(_heartbeat_time_data* td) {
  td->last_timestamp = -1;
  td->total_time = 0;
  td->window_time = 0;
}

static inline void init_work_data(_heartbeat_work_data* td) {
  td->total_work = 0;
  td->window_work = 0;
}

static inline void init_accuracy_data(_heartbeat_accuracy_data* ad) {
  ad->total_accuracy = 0;
  ad->window_accuracy = 0;
}

static inline void init_energy_data(_heartbeat_energy_data* ed) {
  ed->last_energy = 0;
  ed->total_energy = 0;
  ed->window_energy = 0;
}

static inline int init_energy_resource(_heartbeat_energy_resource* er,
                                       uint64_t num_energy_impls,
                                       em_impl* energy_impls) {
  uint64_t i;
  char em_src[100];

  er->num_energy_impls = 0;
  er->energy_impls = NULL;
  if (energy_impls != NULL) {
    for (i = 0; i < num_energy_impls; i++) {
      // fread and fsource functions are required, finit and ffinish are not
      if (energy_impls[i].fread == NULL || energy_impls[i].fsource == NULL) {
        fprintf(stderr, "hb-energy implementation at index %"PRIu64
                " is missing fread and/or fsource\n", i);
        // cleanup previously started implementations
        finish_energy_readings(i, energy_impls);
        return 1;
      }
      energy_impls[i].fsource(em_src);
      if (energy_impls[i].finit != NULL && energy_impls[i].finit(&energy_impls[i])) {
        fprintf(stderr, "Failed to initialize energy reading from: %s\n",
                em_src);
        // cleanup previously started implementations
        finish_energy_readings(i, energy_impls);
        return 1;
      }
      printf("Initialized energy reading from: %s\n", em_src);
    }
    er->num_energy_impls = num_energy_impls;
    er->energy_impls = energy_impls;
  }
  return 0;
}

static inline int init_local_data(_heartbeat_local_data* ld,
                                  uint64_t buffer_depth,
                                  const char* log_name) {
  ld->valid = 0;
  ld->counter = 0;
  ld->buffer_depth = buffer_depth;
  ld->buffer_index = 0;
  ld->read_index = 0;
  init_time_data(&ld->td);
  init_work_data(&ld->wd);
  init_accuracy_data(&ld->ad);
  init_energy_data(&ld->ed);

  // allocate log buffer
  ld->log = malloc(buffer_depth * sizeof(_heartbeat_record_t));
  if (ld->log == NULL) {
    return 1;
  }
  // set initial values to 0 (necessary for managing window data)
  memset(ld->log, 0, buffer_depth * sizeof(_heartbeat_record_t));

  // open log file
  if (log_name != NULL) {
    ld->text_file = fopen(log_name, "w");
    if (ld->text_file == NULL) {
      perror("Failed to open heartbeat log file");
      // cleanup log buffer
      free(ld->log);
      ld->log = NULL;
      return 1;
    }
    fprintf(ld->text_file,
            "LID    SID    Tag    Timestamp    "
            "Work    Latency    Global_Perf    Window_Perf    Instant_Perf    "
            "Accuracy    Global_Acc    Window_Acc    Instant_Acc    "
            "Energy    Global_Pwr    Window_Pwr    Instant_Pwr\n");
  }
  return 0;
}

static inline int init_shared_data(_heartbeat_shared_data* sd,
                                   uint64_t num_energy_impls,
                                   em_impl* energy_impls) {
  sd->valid = 0;
  sd->counter = 0;
  // setup energy readers
  if (init_energy_resource(&sd->er, num_energy_impls, energy_impls)) {
    return 1;
  }
#ifdef HEARTBEAT_USE_PTHREADS_LOCK
  pthread_mutex_init(&sd->mutex, NULL);
#endif
  return 0;
}

heartbeat_t* heartbeat_acc_pow_init(heartbeat_t* parent,
                                    uint64_t window_size,
                                    uint64_t buffer_depth,
                                    const char* log_name,
                                    uint64_t num_energy_impls,
                                    em_impl* energy_impls) {
  if (buffer_depth < window_size) {
    fprintf(stderr, "Buffer depth must be >= window size\n");
    return NULL;
  }

  heartbeat_t* hb = malloc(sizeof(heartbeat_t));
  if (hb == NULL) {
    perror("Failed to malloc heartbeat");
    return NULL;
  }

  hb->parent = parent;
  hb->window_size = window_size;

  // initialize to null in case we have to cleanup
  hb->ld.log = NULL;
  hb->ld.text_file = NULL;
  hb->sd = NULL;

  // allocate or point to existing shared data
  if (hb->parent == NULL) {
    // allocate shared data
    hb->sd = malloc(sizeof(_heartbeat_shared_data));
    if (hb->sd == NULL) {
      perror("Failed to malloc heartbeat shared data");
      heartbeat_finish(hb);
      return NULL;
    }
    if (init_shared_data(hb->sd, num_energy_impls, energy_impls)) {
      heartbeat_finish(hb);
      return NULL;
    }
  } else {
    // point to parent's shared data
    hb->sd = hb->parent->sd;
  }

  // local data
  if (init_local_data(&hb->ld, buffer_depth, log_name)) {
    heartbeat_finish(hb);
    return NULL;
  }

  return hb;
}

heartbeat_t* heartbeat_acc_init(heartbeat_t* parent,
                                uint64_t window_size,
                                uint64_t buffer_depth,
                                const char* log_name) {
  return heartbeat_acc_pow_init(parent, window_size, buffer_depth, log_name,
                                0, NULL);
}

heartbeat_t* heartbeat_init(heartbeat_t* parent,
                            uint64_t window_size,
                            uint64_t buffer_depth,
                            const char* log_name) {
  return heartbeat_acc_pow_init(parent, window_size, buffer_depth, log_name,
                                0, NULL);
}

/**
 * Write log to file.
 */
static void hb_flush_buffer(heartbeat_t* hb) {
  uint64_t i;
  if (hb->ld.text_file != NULL) {
    for (i = 0; i < hb->ld.buffer_index; i++) {
      fprintf(hb->ld.text_file,
              "%" PRIu64"    %" PRIu64"    %" PRIu64"    %" PRIu64"    "
              "%" PRIu64"    %" PRIu64"    %f    %f    %f    "
              "%f    %f    %f    %f    "
              "%f    %f    %f    %f\n",
              hb->ld.log[i].id,
              hb->ld.log[i].shared_id,
              hb->ld.log[i].user_tag,
              hb->ld.log[i].timestamp,

              hb->ld.log[i].work,
              hb->ld.log[i].latency,
              hb->ld.log[i].global_perf,
              hb->ld.log[i].window_perf,
              hb->ld.log[i].instant_perf,

              hb->ld.log[i].accuracy,
              hb->ld.log[i].global_acc,
              hb->ld.log[i].window_acc,
              hb->ld.log[i].instant_acc,

              hb->ld.log[i].energy,
              hb->ld.log[i].global_pwr,
              hb->ld.log[i].window_pwr,
              hb->ld.log[i].instant_pwr);
    }
    fflush(hb->ld.text_file);
  }
}

void heartbeat_finish(heartbeat_t* hb) {
  if (hb != NULL) {
    if (hb->parent == NULL && hb->sd != NULL) {
      // this hb owns the shared data - clean it up
      finish_energy_readings(hb->sd->er.num_energy_impls, hb->sd->er.energy_impls);
#ifdef HEARTBEAT_USE_PTHREADS_LOCK
      pthread_mutex_destroy(&hb->sd->mutex);
#endif
      free(hb->sd);
    }
    // cleanup local data
    if (hb->ld.text_file != NULL) {
      hb_flush_buffer(hb);
      fclose(hb->ld.text_file);
    }
    free(hb->ld.log);
    free(hb);
  }
}

static inline void set_window_values(heartbeat_t* hb,
                                     int64_t latency_change,
                                     uint64_t work,
                                     double accuracy,
                                     double energy_change) {
  // get the index for the data we're going to drop from the window
  // we enforce buffer_size >= window_size for this purpose
  uint64_t idx;
  if (hb->window_size > hb->ld.buffer_index) {
    idx = hb->ld.buffer_depth + hb->ld.buffer_index - hb->window_size;
  } else {
    idx = hb->ld.buffer_index - hb->window_size;
  }
  // now update the running window values
  // if we haven't yet reached window_size heartbeats, the log values are 0
  hb->ld.td.window_time += latency_change - hb->ld.log[idx].latency;
  hb->ld.wd.window_work += work - hb->ld.log[idx].work;
  hb->ld.ad.window_accuracy += accuracy - hb->ld.log[idx].accuracy;
  hb->ld.ed.window_energy += energy_change - hb->ld.log[idx].energy;
}

static inline void process_heartbeat(heartbeat_t* hb,
                                     uint64_t user_tag,
                                     uint64_t work,
                                     double accuracy,
                                     int64_t time,
                                     double energy) {
  int64_t latency_change;
  double energy_change;

  // update shared data
  hb->sd->counter++;
  if (hb->sd->valid == 0) {
    hb->sd->valid = 1;
  } else {
    hb->sd->td.total_time += time - hb->sd->td.last_timestamp;
  }
  hb->sd->td.last_timestamp = time;

  // update local data
  if (hb->ld.valid == 0) {
    hb->ld.valid = 1;
    latency_change = 0;
    energy_change = 0;
    accuracy = 0;
    work = 0;
  } else {
    latency_change = time - hb->ld.td.last_timestamp;
    energy_change = energy - hb->ld.ed.last_energy;
    hb->ld.td.total_time += latency_change;
    hb->ld.wd.total_work += work;
    hb->ld.ad.total_accuracy += accuracy;
    hb->ld.ed.total_energy += energy - hb->ld.ed.last_energy;
  }
  set_window_values(hb, latency_change, work, accuracy, energy_change);
  hb->ld.td.last_timestamp = time;
  hb->ld.ed.last_energy = energy;
  hb->ld.counter++;
  hb->ld.read_index = hb->ld.buffer_index;
  uint64_t index = hb->ld.buffer_index;
  hb->ld.buffer_index++;

  // now store in log
  hb->ld.log[index].id = hb->ld.counter - 1;
  hb->ld.log[index].shared_id = hb->sd->counter - 1;
  hb->ld.log[index].user_tag = user_tag;
  hb->ld.log[index].timestamp = time;
  hb->ld.log[index].work = work;
  hb->ld.log[index].latency = latency_change;
  hb->ld.log[index].accuracy = accuracy;
  hb->ld.log[index].energy = energy_change;
  if (latency_change == 0) {
    hb->ld.log[index].global_perf = 0;
    hb->ld.log[index].window_perf = 0;
    hb->ld.log[index].instant_perf = 0;
    hb->ld.log[index].global_acc = 0;
    hb->ld.log[index].window_acc = 0;
    hb->ld.log[index].instant_acc = 0;
    hb->ld.log[index].global_pwr = 0;
    hb->ld.log[index].window_pwr = 0;
    hb->ld.log[index].instant_pwr = 0;
  } else {
    const double one_billion = 1000000000.0;
    double total_seconds = ((double) hb->ld.td.total_time) / one_billion;
    double window_seconds = ((double) hb->ld.td.window_time) / one_billion;
    double instant_seconds = ((double) latency_change) / one_billion;
    hb->ld.log[index].global_perf = ((double) hb->ld.wd.total_work) / total_seconds;
    hb->ld.log[index].window_perf = ((double) hb->ld.wd.window_work) / window_seconds;
    hb->ld.log[index].instant_perf = ((double) work) / instant_seconds;
    hb->ld.log[index].global_acc = hb->ld.ad.total_accuracy / total_seconds;
    hb->ld.log[index].window_acc = hb->ld.ad.window_accuracy / window_seconds;
    hb->ld.log[index].instant_acc = accuracy / instant_seconds;
    hb->ld.log[index].global_pwr = hb->ld.ed.total_energy / total_seconds;
    hb->ld.log[index].window_pwr = hb->ld.ed.window_energy / window_seconds;
    hb->ld.log[index].instant_pwr = energy_change / instant_seconds;
  }

  // check circular buffer, write to file if full
  if (hb->ld.buffer_index % hb->ld.buffer_depth == 0) {
    hb_flush_buffer(hb);
    hb->ld.buffer_index = 0;
  }
}

static inline int64_t get_time() {
  struct timespec time_info;
  clock_gettime(CLOCK_REALTIME, &time_info);
  return (int64_t) time_info.tv_sec * 1000000000 + (int64_t) time_info.tv_nsec;
}

static inline double get_energy(_heartbeat_energy_resource* er) {
  double total_energy = 0.0;
  double energy_tmp;
  uint64_t i;
  char em_src[100];
  if (er != NULL && er->energy_impls != NULL) {
    for (i = 0; i < er->num_energy_impls; i++) {
      energy_tmp = er->energy_impls[i].fread(&er->energy_impls[i]);
      if (energy_tmp < 0) {
        fprintf(stderr, "heartbeat: Bad energy reading from: %s\n",
                er->energy_impls[i].fsource(em_src));
        continue;
      }
      total_energy += energy_tmp;
    }
  }
  return total_energy;
}

int64_t heartbeat_acc(heartbeat_t* hb,
                      uint64_t user_tag,
                      uint64_t work,
                      double accuracy,
                      const heartbeat_t* hb_prev) {
#ifdef HEARTBEAT_USE_PTHREADS_LOCK
  pthread_mutex_lock(&hb->sd->mutex);
#endif
  int64_t time = get_time();
  if (hb_prev != NULL && hb_prev->ld.valid) {
    // update local data based on previous heartbeat
    hb->ld.td.last_timestamp = hb_prev->ld.td.last_timestamp;
    hb->ld.ed.last_energy = hb_prev->ld.ed.last_energy;
  }
  double energy = get_energy(&hb->sd->er);
  process_heartbeat(hb, user_tag, work, accuracy, time, energy);
#ifdef HEARTBEAT_USE_PTHREADS_LOCK
  pthread_mutex_unlock(&hb->sd->mutex);
#endif
  return time;
}

int64_t heartbeat(heartbeat_t* hb,
                  uint64_t user_tag,
                  uint64_t work,
                  const heartbeat_t* hb_prev) {
  return heartbeat_acc(hb, user_tag, work, HEARTBEAT_ACCURACY_DEFAULT, hb_prev);
}
