/**
 * An extension of the capabilities in "heartbeat-tree.h" to include monitoring
 * program accuracy.
 *
 * @author Connor Imes
 */
#ifndef _HEARTBEAT_TREE_ACCURACY_H_
#define _HEARTBEAT_TREE_ACCURACY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "heartbeat-tree-accuracy-types.h"
#include "heartbeat-tree.h"

/**
 * Initialize a heartbeats instance.
 *
 * @param parent
 * @param window_size
 * @param buffer_depth
 * @param log_name
 * @return heartbeat_t or NULL on failure
 */
heartbeat_t* heartbeat_acc_init(heartbeat_t* parent,
                                uint64_t window_size,
                                uint64_t buffer_depth,
                                const char* log_name);

/**
 * Registers a heartbeat
 *
 * @param hb
 * @param user_tag
 * @param work
 * @param accuracy
 * @param hb_prev
 * @return timestamp
 */
int64_t heartbeat_acc(heartbeat_t* hb,
                      uint64_t user_tag,
                      uint64_t work,
                      double accuracy,
                      const heartbeat_t* hb_prev);

/**
 * Get the total accuracy for the life of this heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the global accuracy (double)
 */
double hb_get_global_accuracy(const heartbeat_t* hb);

/**
 * Get the current window accuracy for this heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the window accuracy (double)
 */
double hb_get_window_accuracy(const heartbeat_t* hb);

/**
 * Returns the accuracy over the life of the entire application
 *
 * @param hb pointer to heartbeat_t
 * @return the accuracy (double) over the entire life of the application
 */
double hb_get_global_accuracy(const heartbeat_t* hb);

/**
 * Returns the accuracy over the last window (as specified to init)
 * heartbeats
 *
 * @param hb pointer to heartbeat_t
 * @return the accuracy (double) over the last window
 */
double hb_get_window_accuracy(const heartbeat_t* hb);

/**
 * Returns the accuracy for the last heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the accuracy (double) for the last heartbeat
 */
double hb_get_instant_accuracy(const heartbeat_t* hb);

/**
 * Returns the accuracy recorded in this record.
 *
 * @param hbr
 * @return the accuracy (double)
 */
double hbr_get_accuracy(const heartbeat_record_t* hbr);

/**
 * Returns the global accuracy recorded in this record.
 *
 * @param hbr
 * @return the global accuracy (double)
 */
double hbr_get_global_accuracy(const heartbeat_record_t* hbr);

/**
 * Returns the window accuracy recorded in this record.
 *
 * @param hbr
 * @return the window accuracy (double)
 */
double hbr_get_window_accuracy(const heartbeat_record_t* hbr);

/**
 * Returns the instant accuracy recorded in this record.
 *
 * @param hbr
 * @return the instant accuracy (double)
 */
double hbr_get_instant_accuracy(const heartbeat_record_t* hbr);

#ifdef __cplusplus
}
#endif

#endif
