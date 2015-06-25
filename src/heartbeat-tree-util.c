/**
 * Functions defined in heartbeat header files that are reusable across
 * implementations due to common structure in heartbeat_t structs.
 *
 * To disable function definitions for a particular heartbeat interface and
 * implement them elsewhere, define the following macros as needed:
 *   HEARTBEAT_UTIL_OVERRIDE
 *   HEARTBEAT_ACCURACY_UTIL_OVERRIDE
 *   HEARTBEAT_ACCURACY_POWER_UTIL_OVERRIDE
 *
 * @author Connor Imes
 * @author Hank Hoffmann
 */

#include <string.h>
#include <inttypes.h>

/* Determine which heartbeat implementation to use */
#if defined(HEARTBEAT_MODE_ACC_POW)
#include "heartbeat-tree-accuracy-power.h"
#include "heartbeat-tree-accuracy-power-types.h"
#elif defined(HEARTBEAT_MODE_ACC)
#include "heartbeat-tree-accuracy.h"
#include "heartbeat-tree-accuracy-types.h"
#else
#include "heartbeat-tree.h"
#include "heartbeat-tree-types.h"
#endif

/*
 * Functions from heartbeat-tree.h
 */
#if !defined(HEARTBEAT_UTIL_OVERRIDE)

heartbeat_t* hb_get_parent(const heartbeat_t* hb) {
  return hb->parent;
}

uint64_t hb_get_window_size(const heartbeat_t* hb) {
  return hb->window_size;
}

uint64_t hb_get_buffer_depth(const heartbeat_t* hb) {
  return hb->ld.buffer_depth;
}

void hb_get_current(const heartbeat_t* hb,
                    heartbeat_record_t* record) {
  hb_get_history(hb, record, 1);
}

int64_t hb_get_global_time(const heartbeat_t* hb) {
  return hb->ld.td.total_time;
}

int64_t hb_get_window_time(const heartbeat_t* hb) {
  return hb->ld.td.window_time;
}

uint64_t hb_get_global_work(const heartbeat_t* hb) {
  return hb->ld.wd.total_work;
}

uint64_t hb_get_window_work(const heartbeat_t* hb) {
  return hb->ld.wd.window_work;
}

double hb_get_global_rate(const heartbeat_t* hb) {
  return hb->ld.log[hb->ld.read_index].global_perf;
}

double hb_get_window_rate(const heartbeat_t* hb) {
  return hb->ld.log[hb->ld.read_index].window_perf;
}

double hb_get_instant_rate(const heartbeat_t* hb) {
  return hb->ld.log[hb->ld.read_index].instant_perf;
}

uint64_t hb_get_history(const heartbeat_t* hb,
                        heartbeat_record_t* record,
                        uint64_t n) {
  if (n == 0) {
    return 0;
  }

  if (n > hb->ld.counter) {
    // more records were requested than have been created
    memcpy(record,
           &hb->ld.log[0],
           hb->ld.buffer_index * sizeof(heartbeat_record_t));
    return hb->ld.buffer_index;
  }

  if (hb->ld.buffer_index >= n) {
    // the number of records requested do not overflow the circular buffer
    memcpy(record,
           &hb->ld.log[hb->ld.buffer_index - n],
           n * sizeof(heartbeat_record_t));
    return n;
  }

  // the number of records requested could overflow the circular buffer
  if (n >= hb->ld.buffer_depth) {
    // more records were requested than we can support, return what we have
    memcpy(record,
         &hb->ld.log[hb->ld.buffer_index],
         (hb->ld.buffer_depth - hb->ld.buffer_index) * sizeof(heartbeat_record_t));
    memcpy(record + hb->ld.buffer_depth - hb->ld.buffer_index,
           &hb->ld.log[0],
           hb->ld.buffer_index * sizeof(heartbeat_record_t));
    return hb->ld.buffer_depth;
  }

  // buffer_index < n < buffer_depth
  // still overflows circular buffer, but we don't want all records
  memcpy(record,
         &hb->ld.log[hb->ld.buffer_depth + hb->ld.buffer_index - n],
         (n - hb->ld.buffer_index) * sizeof(heartbeat_record_t));
  memcpy(record + n - hb->ld.buffer_index,
         &hb->ld.log[0],
         hb->ld.buffer_index * sizeof(heartbeat_record_t));
  return n;
}

uint64_t hbr_get_beat_number(const heartbeat_record_t* hbr) {
  return hbr->id;
}

uint64_t hbr_get_shared_beat_number(const heartbeat_record_t* hbr) {
  return hbr->shared_id;
}

uint64_t hbr_get_user_tag(const heartbeat_record_t* hbr) {
  return hbr->user_tag;
}

int64_t hbr_get_timestamp(const heartbeat_record_t* hbr) {
  return hbr->timestamp;
}

uint64_t hbr_get_work(const heartbeat_record_t* hbr) {
  return hbr->work;
}

int64_t hbr_get_latency(const heartbeat_record_t* hbr) {
  return hbr->latency;
}

double hbr_get_global_rate(const heartbeat_record_t* hbr) {
  return hbr->global_perf;
}

double hbr_get_window_rate(const heartbeat_record_t* hbr) {
  return hbr->window_perf;
}

double hbr_get_instant_rate(const heartbeat_record_t* hbr) {
  return hbr->instant_perf;
}

#endif

/*
 * Functions from heartbeat-tree-accuracy.h
 */
#if defined(HEARTBEAT_MODE_ACC) && !defined(HEARTBEAT_ACCURACY_UTIL_OVERRIDE)

double hb_get_global_accuracy(const heartbeat_t* hb) {
  return hb->ld.ad.total_accuracy;
}

double hb_get_window_accuracy(const heartbeat_t* hb) {
  return hb->ld.ad.window_accuracy;
}

double hb_get_global_accuracy(const heartbeat_t* hb) {
  return hb->ld.log[hb->ld.read_index].global_acc;
}

double hb_get_window_accuracy(const heartbeat_t* hb) {
  return hb->ld.log[hb->ld.read_index].window_acc;
}

double hb_get_instant_accuracy(const heartbeat_t* hb) {
  return hb->ld.log[hb->ld.read_index].instant_acc;
}

double hbr_get_accuracy(const heartbeat_record_t* hbr) {
  return hbr->accuracy;
}

double hbr_get_global_accuracy(const heartbeat_record_t* hbr) {
  return hbr->global_acc;
}

double hbr_get_window_accuracy(const heartbeat_record_t* hbr) {
  return hbr->window_acc;
}

double hbr_get_instant_accuracy(const heartbeat_record_t* hbr) {
  return hbr->instant_acc;
}

#endif

/*
 * Functions from heartbeat-tree-accuracy-power.h
 */
#if defined(HEARTBEAT_MODE_ACC_POW) && !defined(HEARTBEAT_ACCURACY_POWER_UTIL_OVERRIDE)

double hb_get_global_energy(const heartbeat_t* hb) {
  return hb->ld.ed.total_energy;
}

double hb_get_window_energy(const heartbeat_t* hb) {
  return hb->ld.ed.window_energy;
}

double hb_get_global_power(const heartbeat_t* hb) {
  return hb->ld.log[hb->ld.read_index].global_pwr;
}

double hb_get_window_power(const heartbeat_t* hb) {
  return hb->ld.log[hb->ld.read_index].window_pwr;
}

double hb_get_instant_power(const heartbeat_t* hb) {
  return hb->ld.log[hb->ld.read_index].instant_pwr;
}

double hbr_get_energy(const heartbeat_record_t* hbr) {
  return hbr->energy;
}

double hbr_get_global_power(const heartbeat_record_t* hbr) {
  return hbr->global_pwr;
}

double hbr_get_window_power(const heartbeat_record_t* hbr) {
  return hbr->window_pwr;
}

double hbr_get_instant_power(const heartbeat_record_t* hbr) {
  return hbr->instant_pwr;
}

#endif
