/**
 * A performance monitoring API that supports structured parent/child heartbeat
 * relationships. The parent heartbeat owns all global resources and data
 * between all other heartbeats.
 *
 * @author Connor Imes
 */
#ifndef _HEARTBEAT_TREE_H_
#define _HEARTBEAT_TREE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "heartbeat-tree-types.h"
#include <stdint.h>

/**
 * Initialize a heartbeats instance.
 *
 * @param parent
 * @param window_size
 * @param buffer_depth
 * @param log_name
 * @return heartbeat_t or NULL on failure
 */
heartbeat_t* heartbeat_init(heartbeat_t* parent,
                            uint64_t window_size,
                            uint64_t buffer_depth,
                            const char* log_name);

/**
 * Registers a heartbeat.
 *
 * @param hb
 * @param user_tag
 * @param work
 * @param hb_prev
 * @return timestamp
 */
int64_t heartbeat(heartbeat_t* hb,
                  uint64_t user_tag,
                  uint64_t work,
                  const heartbeat_t* hb_prev);

/**
 * Cleanup function for process that wants to register heartbeats
 *
 * @param hb pointer to heartbeat_t
 */
void heartbeat_finish(heartbeat_t* hb);

/**
 * Return the heartbeat's parent, or NULL if it doesn't have one.
 *
 * @param hb pointer to heartbeat_t
 * @return heartbeat_t or NULL
 */
heartbeat_t* hb_get_parent(const heartbeat_t* hb);

/**
 * Returns the size of the sliding window used to compute the current heart
 * rate
 *
 * @param hb pointer to heartbeat_t
 * @return the size of the sliding window (uint64_t)
 */
uint64_t hb_get_window_size(const heartbeat_t* hb);

/**
 * Returns the buffer depth of the log rate
 *
 * @param hb pointer to heartbeat_t
 * @return the buffer depth (uint64_t)
 */
uint64_t hb_get_buffer_depth(const heartbeat_t* hb);

/**
 * Returns the current user tag
 *
 * @param hb pointer to heartbeat_t
 * @return the current user tag (uint64_t)
 */
uint64_t hb_get_user_tag(const heartbeat_t* hb);

/**
 * Get the total time for the life of this heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the total time (int64_t)
 */
int64_t hb_get_global_time(const heartbeat_t* hb);

/**
 * Get the current window time for this heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the window time (int64_t)
 */
int64_t hb_get_window_time(const heartbeat_t* hb);

/**
 * Get the total work for the life of this heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the total work (uint64_t)
 */
uint64_t hb_get_global_work(const heartbeat_t* hb);

/**
 * Get the current window work for this heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the window work (uint64_t)
 */
uint64_t hb_get_window_work(const heartbeat_t* hb);

/**
 * Returns the heart rate over the life of the entire application
 *
 * @param hb pointer to heartbeat_t
 * @return the heart rate (double) over the entire life of the application
 */
double hb_get_global_rate(const heartbeat_t* hb);

/**
 * Returns the heart rate over the last window (as specified to init)
 * heartbeats
 *
 * @param hb pointer to heartbeat_t
 * @return the heart rate (double) over the last window
 */
double hb_get_window_rate(const heartbeat_t* hb);

/**
 * Returns the heart rate for the last heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the heart rate (double) for the last heartbeat
 */
double hb_get_instant_rate(const heartbeat_t* hb);

/**
 * Returns the record for the current heartbeat
 * currently may read old data
 *
 * @param hb pointer to heartbeat_t
 * @param record pointer to record to fill
 */
void hb_get_current(const heartbeat_t* hb,
                    heartbeat_record_t* record);

/**
 * Returns all heartbeat information for the last n heartbeats
 *
 * @param hb pointer to heartbeat_t
 * @param record pointer to heartbeat_record_t
 * @param n uint64_t
 */
uint64_t hb_get_history(const heartbeat_t* hb,
                        heartbeat_record_t* record,
                        uint64_t n);

/**
 * Returns the local heartbeat number for this record.
 *
 * @param hbr
 * @return the local beat number (uint64_t)
 */
uint64_t hbr_get_beat_number(const heartbeat_record_t* hbr);

/**
 * Returns the shared heartbeat number for this record.
 *
 * @param hbr
 * @return the shared beat number (uint64_t)
 */
uint64_t hbr_get_shared_beat_number(const heartbeat_record_t* hbr);

/**
 * Returns the tag number for this record.
 *
 * @param hbr
 * @return the user-specified tag number (uint64_t)
 */
uint64_t hbr_get_user_tag(const heartbeat_record_t* hbr);

/**
 * Returns the timestamp for this record.
 *
 * @param hbr
 * @return the timestamp (int64_t)
 */
int64_t hbr_get_timestamp(const heartbeat_record_t* hbr);

/**
 * Returns the work completed for this record.
 *
 * @param hbr
 * @return the work completed (uint64_t)
 */
uint64_t hbr_get_work(const heartbeat_record_t* hbr);

/**
 * Returns the latency for this record.
 *
 * @param hbr
 * @return the latency (int64_t)
 */
int64_t hbr_get_latency(const heartbeat_record_t* hbr);

/**
 * Returns the global rate recorded in this record.
 *
 * @param hbr
 * @return the global rate (double)
 */
double hbr_get_global_rate(const heartbeat_record_t* hbr);

/**
 * Returns the window rate recorded in this record.
 *
 * @param hbr
 * @return the window rate (double)
 */
double hbr_get_window_rate(const heartbeat_record_t* hbr);

/**
 * Returns the instant rate recorded in this record.
 *
 * @param hbr
 * @return the instant rate (double)
 */
double hbr_get_instant_rate(const heartbeat_record_t* hbr);

#ifdef __cplusplus
 }
#endif

#endif
