/*******************************************************************************
 *
 *
 * Copyright (c) 2018-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * MIT LICENSE:
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 *******************************************************************************/

#include "include/action.h"

#include <string>
#include <map>
#include <vector>
#include <utility>

#include "include/rvs_key_def.h"
#include "include/rvsloglp.h"
#include "include/rvs_module.h"
#include "include/rvs_util.h"
#include "include/gpu_util.h"
#include "include/rsmi_util.h"
#include "include/worker.h"

#define JSON_CREATE_NODE_ERROR          "JSON cannot create node"

#define GM_TEMP                       "temp"
#define GM_CLOCK                      "clock"
#define GM_MEM_CLOCK                  "mem_clock"
#define GM_FAN                        "fan"
#define GM_POWER                      "power"
#define GM_FORCE                      "force"
static constexpr auto MODULE_NAME = "gm";
static constexpr auto MODULE_NAME_CAPS = "GM";

extern Worker* pworker;

/**
 * default class constructor
 */
gm_action::gm_action() {
  bjson = false;
  json_root_node = nullptr;
  module_name  = MODULE_NAME;
  property_bounds.insert(std::pair<string, Metric_bound>
    (GM_TEMP, {false, false, 0, 0}));
  property_bounds.insert(std::pair<string, Metric_bound>
    (GM_CLOCK, {false, false, 0, 0}));
  property_bounds.insert(std::pair<string, Metric_bound>
    (GM_MEM_CLOCK, {false, false, 0, 0}));
  property_bounds.insert(std::pair<string, Metric_bound>
    (GM_FAN, {false, false, 0, 0}));
  property_bounds.insert(std::pair<string, Metric_bound>
    (GM_POWER, {false, false, 0, 0}));
}

/**
 * class destructor
 */
gm_action::~gm_action() {
    property.clear();
}


/**
 * @brief Read configuration 'metric:' key and store it into property_bounds
 * array.
 * @param pMetric Metric name
 * @return 0 - OK
 * @return 1 - syntax error
 */
int gm_action::get_bounds(const char* pMetric) {
  std::string smetric("metrics.");
  smetric += pMetric;

  std::string sval;
  if (!has_property(smetric, &sval)) {
    return 2;
  }

  Metric_bound bound_;
  int error;
  std::vector<string> values = str_split(sval, YAML_DEVICE_PROP_DELIMITER);
  if (values.size() == 3) {
    bound_.mon_metric = true;
    bound_.check_bounds = (values[0] == "true") ? true : false;
    error = rvs_util_parse<uint32_t>(values[1], &bound_.max_val);
    if (error) {
      return 1;
    }
    error = rvs_util_parse<uint32_t>(values[2], &bound_.min_val);
    if (error) {
      return 1;
    }
    property_bounds[std::string(pMetric)] = bound_;
  } else {
    return 1;
  }

  return 0;
}

/**
 * @brief reads all GM specific configuration keys from
 * the module's properties collection
 * @return true if no fatal error occured, false otherwise
 */
bool gm_action::get_all_gm_config_keys(void) {
  string msg;
  bool sts = true;

  if (get_bounds(GM_TEMP) == 1) {
    msg = "Invalid 'metrics." +
            std::string(GM_TEMP) + "' key.";
    rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);
    sts = false;
  }

  if (get_bounds(GM_CLOCK) == 1) {
    msg = "Invalid 'metrics." +
            std::string(GM_CLOCK) + "' key.";
    rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);
    sts = false;
  }

  if (get_bounds(GM_MEM_CLOCK) == 1) {
    msg = "Invalid 'metrics." +
            std::string(GM_MEM_CLOCK) + "' key.";
    rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);
    sts = false;
  }

  if (get_bounds(GM_FAN) == 1) {
    msg = "Invalid 'metrics." +
            std::string(GM_FAN) + "' key.";
    rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);
    sts = false;
  }

  if (get_bounds(GM_POWER) == 1) {
    msg = "Invalid 'metrics." +
            std::string(GM_POWER) + "' key.";
    rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);
    sts = false;
    }

 if (property_get(GM_FORCE, &prop_force, false)) {
      msg = "Invalid '" + std::string(GM_FORCE) + "' key.";
      rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);
      sts = false;
    }
  
 if (property_get(RVS_CONF_TERMINATE_KEY, &prop_terminate, false)) {
      msg = "Invalid 'terminate' key.";
      rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);
      sts = false;
  }
  if (property_get_int<uint64_t>(RVS_CONF_SAMPLE_INTERVAL_KEY,
                                       &sample_interval, 500u)) {
      msg = "Invalid '" +std::string(RVS_CONF_SAMPLE_INTERVAL_KEY) + "' key.";
      rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);
      sts = false;
    }
  if (property_log_interval < sample_interval) {
      msg = "Log interval has the lower value than the sample interval.";
      rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);
      sts = false;
    }

  return sts;
}
/**
 * @brief Implements action functionality
 *
 * Functionality:
 * 
 * @return 0 - success. non-zero otherwise
 *
 * */
int gm_action::run(void) {
  string msg;
  amdsmi_status_t status;
  rvs::action_result_t action_result;

  // if monitoring is already running, stop it
  // (it will be restarted if needed)
  RVSTRACE_
  if (pworker) {
    RVSTRACE_
    // (give thread chance to start)
    sleep(2);
    pworker->set_stop_name(property["name"]);
    pworker->stop();
    delete pworker;
    pworker = nullptr;
  }
  // this action should stop monitoring?
  if (property["monitor"] != "true") {
    RVSTRACE_
    // already done, just return
    action_result.state = rvs::actionstate::ACTION_COMPLETED;
    action_result.status = rvs::actionstatus::ACTION_FAILED;
    action_result.output = "GM Module action " + action_name + " completed";
    action_callback(&action_result);
    return 0;
  }

  RVSTRACE_
  // start new monitoring
  if (!get_all_common_config_keys()) {
    RVSTRACE_
    action_result.state = rvs::actionstate::ACTION_COMPLETED;
    action_result.status = rvs::actionstatus::ACTION_FAILED;
    action_result.output = "Error in common configuration keys.";
    action_callback(&action_result);
    return -1;
  }

  if (!get_all_gm_config_keys()) {
    RVSTRACE_

    action_result.state = rvs::actionstate::ACTION_COMPLETED;
    action_result.status = rvs::actionstatus::ACTION_FAILED;
    action_result.output = "Error in GM configuration keys.";
    action_callback(&action_result);
    return -1;
  }

  RVSTRACE_

  // if 'device: all' get all AMD GPU IDs
  if (property_device_all) {
    gpu_get_all_gpu_id(&property_device);
  }

  // apply device_id filtering if needed
  if (property_device_id > 0) {
    RVSTRACE_
    std::vector<uint16_t> gpu_id_filtered;
    for (auto it = property_device.begin(); it != property_device.end(); it++) {
      RVSTRACE_

      uint16_t _dev_id;
      if (rvs::gpulist::gpu2device(*it, &_dev_id)) {
        RVSTRACE_
        // if not found just continue
        continue;
      }

      if (_dev_id == property_device_id) {
        RVSTRACE_
        gpu_id_filtered.push_back(*it);
      }
    }
    property_device = gpu_id_filtered;
  }

  RVSTRACE_

  // verify that the resulting array is not empty
  if (property_device.size() < 1) {
    msg = "No devices match filtering criteria.";
    rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);

    action_result.state = rvs::actionstate::ACTION_COMPLETED;
    action_result.status = rvs::actionstatus::ACTION_FAILED;
    action_result.output = msg;
    action_callback(&action_result);
    return -1;
  }

  // convert GPU ID into amd_smi_lib device hdl
  std::map<uint32_t, amdsmi_processor_handle> dv_hdl;
  for (auto it = property_device.begin(); it != property_device.end(); it++) {
    RVSTRACE_
    uint16_t location_id;
    if (rvs::gpulist::gpu2location(*it, &location_id)) {
      msg = "Could not obtain BDF for GPU ID: ";
      msg += std::to_string(*it);
      rvs::lp::Err(msg, MODULE_NAME_CAPS, action_name);

      action_result.state = rvs::actionstate::ACTION_COMPLETED;
      action_result.status = rvs::actionstatus::ACTION_FAILED;
      action_result.output = msg;
      action_callback(&action_result);
      return -1;
    }
    amdsmi_processor_handle ix;
    status = rvs::smi_dev_ind_get(location_id, &ix);
    if(status == AMDSMI_STATUS_SUCCESS) {
       dv_hdl.insert(std::pair<uint32_t, amdsmi_processor_handle>(*it, ix));
    }
  }

  pworker = new Worker();
  pworker->set_name(action_name);
  pworker->set_action(*this);
  pworker->json(bjson);
  pworker->set_sample_int(sample_interval);
  pworker->set_log_int(property_log_interval);
  pworker->set_terminate(prop_terminate);
  if (prop_force)
    pworker->set_force(true);

  // set stop name before start
  pworker->set_stop_name(action_name);
  // set array of device indices to monitor
  pworker->set_dv_hdl(dv_hdl);
  // set bounds map
  pworker->set_bound(property_bounds);

  RVSTRACE_
  // start worker thread
  pworker->start();

  // this should be used only for testing purposes
  if (property_duration) {
    RVSTRACE_
    sleep(property_duration);
  }

  RVSTRACE_

  action_result.state = rvs::actionstate::ACTION_COMPLETED;
  action_result.status = rvs::actionstatus::ACTION_SUCCESS;
  action_result.output = "GM Module action " + action_name + " completed";
  action_callback(&action_result);

  return 0;
}

