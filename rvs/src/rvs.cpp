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

/** \defgroup Launcher Launcher module
 *
 * \brief Launcher module implementation
 *
 * This is the starting point of rvs utility. Launcher will parse the command line
 * as well as the input YAML configuration file and load appropriate test modules.
 * Then, it will invoke tests specified in .conf file and provide logging functionality
 * so that modules can print on screen of in the log file.
 */

#include <iostream>


#include "include/rvscli.h"
#include "include/rvsexec.h"
#include "include/rvsliblogger.h"
#include "include/rvstrace.h"
#include "include/rvs.h"

#define MODULE_NAME_CAPS "CLI"


/**
 *
 * @ingroup Launcher
 * @brief Main method
 *
 * Standard C main() method.
 *
 * @param Argc standard C argc parameter to main()
 * @param Argv standard C argv parameter to main()
 * @return 0 - all OK, non-zero error
 *
 * */
#if 0
int main(int Argc, char**Argv) {
  int sts;
  rvs::cli cli;

  sts =  cli.parse(Argc, Argv);
  if (sts) {
    char buff[1024];
    snprintf(buff, sizeof(buff),
              "error parssing command line: %s", cli.get_error_string());
    rvs::logger::Err(buff, MODULE_NAME_CAPS);
#ifdef RVS_INVERT_RETURN_STATUS
    return 0;
#else
    return -1;
#endif
  }

  rvs::exec executor;
  sts = executor.run();

#ifdef RVS_INVERT_RETURN_STATUS
  DTRACE_
  return sts ? 0 : 1;
#else
//  std::cout << "sts: " << std::to_string(sts);
  return sts;
#endif
}
#endif

void session_callback (rvs_session_id_t session_id, const rvs_results_t *results) {

  printf("%d session id -> %d output -> %s\n", __LINE__, session_id, results->output_log);
  printf("%d session id -> %d status -> %d\n", __LINE__, session_id, results->status);
}

int main(int Argc, char**Argv) {

  rvs_status_t status;
  rvs_session_id_t session_id;
  rvs_session_property_t session_property = {RVS_SESSION_TYPE_DEFAULT_CONF,{{RVS_MODULE_PBQT}}};

  status = rvs_initialize();
  printf("%d status -> %d\n", __LINE__, status);

  status = rvs_session_create(&session_id, session_callback);
  printf("%d status -> %d session_id -> %d\n", __LINE__, status, session_id);

#if 0
  status = rvs_session_set_property(session_id, &session_property);
  printf("%d status -> %d\n", __LINE__, status);

  status = rvs_session_execute(session_id);
  printf("%d status -> %d\n", __LINE__, status);
#endif

#if 0
  char config[1024] =
"actions:\
- name: action_1\
  device: all\ 
  module: pbqt\
  log_interval: 800\
  duration: 5000\
  peers: all\
  test_bandwidth: true\
  bidirectional: true\
  parallel: true\
  block_size: 1000000 2000000 10000000\
  device_id: all";
#else
char config[1024] = "actions:\n- name: action_1\n  device: all\n  module: pbqt\n  log_interval: 800\n  duration: 5000\n  peers: all\n  test_bandwidth: true\n  bidirectional: true\n  parallel: true\n  block_size: 1000000 2000000 10000000\n  device_id: all";
#endif

  session_property.type = RVS_SESSION_TYPE_CUSTOM_ACTION;
  session_property.custom_action.config = config;

  status = rvs_session_set_property(session_id, &session_property);
  printf("%d status -> %d\n", __LINE__, status);

  status = rvs_session_execute(session_id);
  printf("%d status -> %d\n", __LINE__, status);
}

