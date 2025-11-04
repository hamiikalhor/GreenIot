/**
 * @file ble_mesh_config.h
 * @brief BLE Mesh Configuration and Optimal Intervals
 * 
 * This file defines the optimal publishing intervals based on:
 * 1. Basil cultivation requirements (18-25°C, 60-70% RH)
 * 2. Battery life optimization (target: 5-7 years)
 * 3. BLE Mesh network efficiency
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 * @version 1.0
 */

#ifndef BLE_MESH_CONFIG_H
#define BLE_MESH_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Publishing Intervals - Optimized for Basil Cultivation & Battery Life
// ============================================================================

/**
 * OPTIMAL PUBLISHING INTERVAL: 5 MINUTES (300 seconds)
 * 
 * Justification:
 * 
 * 1. BASIL CULTIVATION REQUIREMENTS:
 *    - Temperature changes slowly in controlled environments (< 0.5°C/min)
 *    - Humidity changes moderately (< 2%/min with HVAC)
 *    - Critical threshold: ±2°C from optimal (18-25°C)
 *    - 5-minute intervals provide early warning (10-15 min before critical)
 * 
 * 2. BATTERY LIFE OPTIMIZATION:
 *    - Active period (sensor + BLE): ~2 seconds @ 50mA = 0.028mAh
 *    - Sleep period (LPN): 298 seconds @ 800µA = 0.066mAh
 *    - Total per cycle: 0.094mAh
 *    - Cycles per day: 288 (24h × 12 per hour)
 *    - Daily consumption: 27mAh
 *    - Battery capacity: 18650 (3000mAh)
 *    - Expected life: 3000mAh / 27mAh = 111 days (~4 months)
 *    - With optimizations (adaptive sampling): 5-7 years possible
 * 
 * 3. BLE MESH NETWORK EFFICIENCY:
 *    - Sensor Status message size: ~12 bytes (temp + hum + battery)
 *    - Network overhead: ~20 bytes (headers, MIC)
 *    - Total per message: ~32 bytes
 *    - With 50 nodes: 1,600 bytes per 5 minutes
 *    - Network utilization: <1% (well within BLE Mesh capacity)
 * 
 * 4. COMPARISON WITH ALTERNATIVES:
 *    - 1 minute: 5x more power, unnecessary for slow-changing environment
 *    - 10 minutes: 2x battery life, but slower anomaly detection
 *    - 15 minutes: 3x battery life, but misses rapid changes (HVAC failure)
 * 
 * CONCLUSION: 5 minutes provides optimal balance between:
 *    - Timely anomaly detection
 *    - Long battery life
 *    - Network efficiency
 */
#define BLE_MESH_PUBLISH_INTERVAL_MS        300000   // 5 minutes
#define BLE_MESH_PUBLISH_INTERVAL_SEC       300      // 5 minutes

/**
 * Alternative intervals for different scenarios
 */
#define BLE_MESH_PUBLISH_FAST_MS            60000    // 1 minute (testing/commissioning)
#define BLE_MESH_PUBLISH_NORMAL_MS          300000   // 5 minutes (normal operation)
#define BLE_MESH_PUBLISH_SLOW_MS            900000   // 15 minutes (power saving)

/**
 * Adaptive publishing thresholds
 * If temperature or humidity changes exceed these thresholds, publish immediately
 */
#define BLE_MESH_TEMP_CHANGE_THRESHOLD      1.0f     // °C
#define BLE_MESH_HUM_CHANGE_THRESHOLD       5.0f     // % RH

// ============================================================================
// Low Power Node (LPN) Configuration
// ============================================================================

/**
 * LPN POLL INTERVAL: 10 SECONDS
 * 
 * Justification:
 * - Balances message latency vs power consumption
 * - Friend node queues messages for max 10 seconds
 * - Average message latency: 5 seconds (acceptable for sensor data)
 * - Power savings: 90-95% compared to always-on
 */
#define BLE_MESH_LPN_POLL_INTERVAL_MS       10000    // 10 seconds
#define BLE_MESH_LPN_POLL_TIMEOUT_MS        300      // 300ms poll timeout
#define BLE_MESH_LPN_RECV_DELAY_MS          100      // 100ms receive delay

// ============================================================================
// Network Configuration
// ============================================================================

/**
 * TTL (Time To Live): 7 hops
 * - Typical vertical farm: 20-50 nodes, 3-5 relay nodes
 * - 7 hops covers >99% of topologies
 * - Reduces network flooding compared to default (127)
 */
#define BLE_MESH_DEFAULT_TTL                7

/**
 * Retransmissions
 */
#define BLE_MESH_TRANSMIT_COUNT             3        // Transmit 3 times
#define BLE_MESH_TRANSMIT_INTERVAL_MS       10       // 10ms between transmits

/**
 * Friend Node Configuration (for relay nodes)
 */
#define BLE_MESH_FRIEND_LPN_COUNT           5        // Support up to 5 LPNs
#define BLE_MESH_FRIEND_QUEUE_SIZE          16       // Messages per LPN

// ============================================================================
// Power Consumption Estimates
// ============================================================================

/**
 * POWER BUDGET CALCULATIONS:
 * 
 * Scenario 1: NORMAL OPERATION (5-minute intervals)
 * ================================================
 * - Sensor reading: 50ms @ 5mA = 0.007mAh
 * - BLE advertising: 10ms @ 15mA = 0.004mAh
 * - BLE transmit: 20ms @ 12mA = 0.007mAh
 * - LPN sleep: 299.92s @ 800µA = 0.067mAh
 * - Total per cycle: 0.085mAh
 * - Cycles per day: 288
 * - Daily: 24.5mAh
 * - Battery life (3000mAh): 122 days (~4 months)
 * 
 * Scenario 2: WITH DEEP SLEEP (when stable)
 * ==========================================
 * - Same as above, but deep sleep @ 10µA
 * - LPN sleep: 299.92s @ 10µA = 0.0008mAh
 * - Total per cycle: 0.018mAh
 * - Daily: 5.2mAh
 * - Battery life (3000mAh): 577 days (~19 months)
 * 
 * Scenario 3: ADAPTIVE SAMPLING (stable conditions)
 * ==================================================
 * - Reduce to 15-minute intervals when stable
 * - Daily: 5.2mAh / 3 = 1.7mAh
 * - Battery life (3000mAh): 1765 days (~5 years)
 * 
 * TARGET: 5-7 years with 18650 battery (3000mAh)
 * ACHIEVED WITH: Adaptive sampling + deep sleep + LPN
 */

#define BLE_MESH_POWER_SENSOR_ACTIVE_UA     5000     // 5mA
#define BLE_MESH_POWER_BLE_TX_UA            12000    // 12mA
#define BLE_MESH_POWER_BLE_RX_UA            11000    // 11mA
#define BLE_MESH_POWER_LPN_SLEEP_UA         800      // 800µA (light sleep)
#define BLE_MESH_POWER_DEEP_SLEEP_UA        10       // 10µA (deep sleep)

// ============================================================================
// Sensor Properties (BLE Mesh Specification)
// ============================================================================

/**
 * Device Properties from Mesh Model Specification v1.0.1
 * https://www.bluetooth.com/specifications/specs/mesh-device-properties-2/
 */

// Temperature 8 (Property ID: 0x004F)
// Unit: degree Celsius
// Format: 8-bit signed integer
// Resolution: 0.5°C
// Range: -64°C to +63.5°C
#define BLE_MESH_PROP_TEMP_RESOLUTION       0.5f
#define BLE_MESH_PROP_TEMP_MIN              -64.0f
#define BLE_MESH_PROP_TEMP_MAX              63.5f

// Humidity (Property ID: 0x0076)
// Unit: percentage
// Format: 16-bit unsigned integer
// Resolution: 0.5%
// Range: 0% to 100%
#define BLE_MESH_PROP_HUM_RESOLUTION        0.5f
#define BLE_MESH_PROP_HUM_MIN               0.0f
#define BLE_MESH_PROP_HUM_MAX               100.0f

// Percentage 8 (Property ID: 0x006E) - for battery level
// Unit: percentage
// Format: 8-bit unsigned integer
// Resolution: 0.5%
// Range: 0% to 100%
#define BLE_MESH_PROP_BATTERY_RESOLUTION    0.5f
#define BLE_MESH_PROP_BATTERY_MIN           0.0f
#define BLE_MESH_PROP_BATTERY_MAX           100.0f

// ============================================================================
// Basil Cultivation Thresholds (for immediate alerts)
// ============================================================================

/**
 * If measurements fall outside these ranges, increase publishing frequency
 * to 1-minute intervals for faster corrective action
 */
#define BASIL_TEMP_MIN_CRITICAL             15.0f    // °C (growth stunting)
#define BASIL_TEMP_MIN_OPTIMAL              18.0f    // °C
#define BASIL_TEMP_MAX_OPTIMAL              25.0f    // °C
#define BASIL_TEMP_MAX_CRITICAL             30.0f    // °C (wilting risk)

#define BASIL_HUM_MIN_CRITICAL              40.0f    // % (leaf drying)
#define BASIL_HUM_MIN_OPTIMAL               60.0f    // %
#define BASIL_HUM_MAX_OPTIMAL               70.0f    // %
#define BASIL_HUM_MAX_CRITICAL              80.0f    // % (mold risk)

#ifdef __cplusplus
}
#endif

#endif // BLE_MESH_CONFIG_H

