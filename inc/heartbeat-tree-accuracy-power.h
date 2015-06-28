/**
 * An extension of the capabilities in "heartbeat-tree-accuracy.h" to include
 * monitoring system power/energy usage.
 *
 * @author Connor Imes
 */
#ifndef _HEARTBEAT_TREE_ACCURACY_POWER_H_
#define _HEARTBEAT_TREE_ACCURACY_POWER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "heartbeat-tree-accuracy-power-types.h"
#include "heartbeat-tree-accuracy.h"
#include <energymon/energymon.h>
#include <stdint.h>

/**
 * Initialize a heartbeats instance.
 *
 * @param parent
 * @param window_size
 * @param buffer_depth
 * @param log_name
 * @param num_energy_impls
 * @param energy_impls
 * @return heartbeat_t or NULL on failure
 */
heartbeat_t* heartbeat_acc_pow_init(heartbeat_t* parent,
                                    uint64_t window_size,
                                    uint64_t buffer_depth,
                                    const char* log_name,
                                    uint64_t num_energy_impls,
                                    em_impl* energy_impls);

/**
 * Get the total energy for the life of this heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the total energy (double)
 */
double hb_get_global_energy(const heartbeat_t* hb);

/**
 * Get the current window energy for this heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the window energy (double)
 */
double hb_get_window_energy(const heartbeat_t* hb);

/**
 * Returns the power over the life of the entire application
 *
 * @param hb pointer to heartbeat_t
 * @return the power (double) over the entire life of the application
 */
double hb_get_global_power(const heartbeat_t* hb);

/**
 * Returns the power over the last window (as specified to init)
 * heartbeats
 *
 * @param hb pointer to heartbeat_t
 * @return the power (double) over the last window
 */
double hb_get_window_power(const heartbeat_t* hb);

/**
 * Returns the power for the last heartbeat.
 *
 * @param hb pointer to heartbeat_t
 * @return the power (double) for the last heartbeat
 */
double hb_get_instant_power(const heartbeat_t* hb);

/**
 * Returns the energy recorded in this record.
 *
 * @param hbr
 * @return the energy (double)
 */
double hbr_get_energy(const heartbeat_record_t* hbr);

/**
 * Returns the global power recorded in this record.
 *
 * @param hbr
 * @return the global power (double)
 */
double hbr_get_global_power(const heartbeat_record_t* hbr);

/**
 * Returns the window power recorded in this record.
 *
 * @param hbr
 * @return the window power (double)
 */
double hbr_get_window_power(const heartbeat_record_t* hbr);

/**
 * Returns the instant power recorded in this record.
 *
 * @param hbr
 * @return the instant power (double)
 */
double hbr_get_instant_power(const heartbeat_record_t* hbr);

#ifdef __cplusplus
}
#endif

#endif
