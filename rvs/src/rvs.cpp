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

//PBQT
//char pbqt_config[1024] = "actions:\n- name: action_1\n  device: all\n  module: pbqt\n  log_interval: 800\n  duration: 5000\n  peers: all\n  test_bandwidth: true\n  bidirectional: true\n  parallel: true\n  block_size: 1000000 2000000 10000000\n  device_id: all";
//GST
//char gst_config[1024]= "{actions: [{name: gpustress-9000-sgemm-false, device: all, module: gst, parallel: false, count: 1, duration: 10000, copy_matrix: false, target_stress: 9000, matrix_size_a: 8640, matrix_size_b: 8640, matrix_size_c: 8640, ops_type: sgemm, lda: 8640, ldb: 8640, ldc: 8640}]}";
//IET
//char iet_config[1024]="{actions: [{name: action_1, device: all, module: iet, parallel: true, count: 1, wait: 100, duration: 50000, ramp_interval: 5000, sample_interval: 700, log_interval: 700, max_violations: 1, target_power: 180, tolerance: 0.06, matrix_size: 8640, ops_type: dgemm}, {name: action_2, device: all, module: iet, parallel: true, count: 1, wait: 100, duration: 50000, ramp_interval: 5000, sample_interval: 1500, log_interval: 2000, max_violations: 1, target_power: 100, tolerance: 0.2, matrix_size: 8640, ops_type: dgemm}]}";
//PEBB
//char pebb_config[1024]="{actions: [{name: h2d-sequential-51MB, device: all, module: pebb, log_interval: 800, duration: 50000, device_to_host: true, host_to_device: true, parallel: true, block_size: 51200000, link_type: 2}]}";
//MEM
//char mem_config[1024]="{actions: [{name: action_1, device: all, module: mem, parallel: true, count: 1, wait: 100, mapped_memory: false, mem_blocks: 128, num_passes: 500, thrds_per_blk: 64, stress: true, num_iter: 50000, exclude: '9 10'}]}";
//PEQT
//char peqt_config[1024]="{actions: [{name: pcie_act_1, device: all, module: peqt, capability: {link_cap_max_speed: null, link_cap_max_width: null, link_stat_cur_speed: null, link_stat_neg_width: null, slot_pwr_limit_value: null, slot_physical_num: null, deviceid: 0, vendor_id: null, kernel_driver: null, dev_serial_num: null, D0_Maximum_Power_12V: null, D0_Maximum_Power_3_3V: null, D0_Sustained_Power_12V: null, D0_Sustained_Power_3_3V: null, atomic_op_routing: null, atomic_op_32_completer: null, atomic_op_64_completer: null, atomic_op_128_CAS_completer: null}}]}";
//GPUP
//char gpup_config[1024]="{actions: [{name: RVS-GPUP-TC1, device: all, module: gpup, properties: {all: null}, io_links-properties: {all: null}}, {name: RVS-GPUP-TC2, device: all, module: gpup, properties: {simd_count: null, mem_banks_count: null, io_links_count: null, vendor_id: null, location_id: null, max_engine_clk_ccompute: null}, io_links-properties: {version_major: null, type: null, version_minor: null, node_from: null, node_to: null, recommended_transfer_size: null, flags: null}}, {name: RVS-GPUP-TC3, device: all, module: gpup, properties: {all: null}, io_links-properties: {all: null}}, {name: RVS-GPUP-TC4, device: all, module: gpup, deviceid: null, properties: {all: null}, io_links-properties: {all: null}}, {name: RVS-GPUP-TC5, device: all, module: gpup, deviceid: null, properties: {all: null}, io_links-properties: {all: null}}, {name: RVS-GPUP-TC6, device: all, module: gpup, deviceid: null, properties: {mem_banks_count: null}, io_links-properties: {version_major: null}}]}";
//RCQT
//char rcqt_config[1024]="{actions: [{name: action_1, device: all, module: rcqt, package: rocm-hip-sdk}, {name: action_2, device: all, module: rcqt, packagelist: 'rocm-hip-libraries rocm-core rocm-dev rocm-hip-runtime-devel rocm-language-runtime rocm-hip-runtime rocm-hip-sdk rocm-utils rocm-smi-lib'}]}";
//PESM
char pesm_config[1024]="{actions: [{name: act1, device: all, deviceid: 0, module: pesm, monitor: true}, {name: act2, device: all, debugwait: 3000, module: pesm, monitor: false}]}";

  session_property.type = RVS_SESSION_TYPE_CUSTOM_ACTION;
  session_property.custom_action.config = pesm_config;

  status = rvs_session_set_property(session_id, &session_property);
  printf("%d status -> %d\n", __LINE__, status);

  status = rvs_session_execute(session_id);
  printf("%d status -> %d\n", __LINE__, status);
}

