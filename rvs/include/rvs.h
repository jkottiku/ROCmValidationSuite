/********************************************************************************
 *
 * Copyright (c) 2018-2022 ROCm Developer Tools
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
#ifndef INCLUDE_RVS_H
#define INCLUDE_RVS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  RVS_STATUS_SUCCESS = 0,
  RVS_STATUS_FAILED = -1,
  RVS_STATUS_INVALID_ARGUMENT = -2,
  RVS_STATUS_INVALID_STATE = -3,
  RVS_STATUS_INVALID_SESSION = -4,
  RVS_STATUS_INVALID_SESSION_STATE = -5
/*
 * Not Supported,
 * Module not found
 * */
} rvs_status_t;

typedef enum {
  RVS_SESSION_STATE_FAILED = -1,
  RVS_SESSION_STATE_IDLE = 0,
  RVS_SESSION_STATE_CREATED,
  RVS_SESSION_STATE_READY,
  RVS_SESSION_STATE_STARTED,
  RVS_SESSION_STATE_INPROGRESS,
  RVS_SESSION_STATE_COMPLETED
} rvs_session_state_t;

typedef unsigned int rvs_session_id_t;

typedef enum rvs_session_type_t {

  RVS_SESSION_TYPE_DEFAULT_CONF,
  RVS_SESSION_TYPE_CUSTOM_CONF,
  RVS_SESSION_TYPE_CUSTOM_ACTION
};

typedef enum rvs_module_t {

  RVS_MODULE_BABEL = 0,
  RVS_MODULE_GPUP,
  RVS_MODULE_GST,
  RVS_MODULE_IET,
  RVS_MODULE_MEM,
  RVS_MODULE_PEBB,
  RVS_MODULE_PEQT,
  RVS_MODULE_PESM,
  RVS_MODULE_PBQT,
  RVS_MODULE_RCQT,
  RVS_MODULE_MAX
};

typedef struct rvs_session_default_conf_t {

 rvs_module_t module;
};

typedef struct rvs_session_custom_conf_t {

 
};

typedef struct rvs_session_custom_action_t {
  const char * config;
};

typedef struct rvs_session_property_t {

  rvs_session_type_t type;

  union {
    rvs_session_default_conf_t default_conf; 
    rvs_session_custom_conf_t custom_conf; 
    rvs_session_custom_action_t custom_action;
  };
};

typedef struct rvs_results_t {
  rvs_status_t status;
  rvs_session_state_t state;
  const char *output_log;
};

typedef void (*rvs_session_callback) (rvs_session_id_t session_id, const rvs_results_t *results);

rvs_status_t rvs_initialize();
rvs_status_t rvs_session_create(rvs_session_id_t *session_id, rvs_session_callback session_cb);
rvs_status_t rvs_session_set_property(rvs_session_id_t session_id, rvs_session_property_t *session_property);
rvs_status_t rvs_session_execute(rvs_session_id_t session_id);
rvs_status_t rvs_session_destroy(rvs_session_id_t session_id);
rvs_status_t rvs_terminate();

#ifdef __cplusplus
}
#endif

#endif
