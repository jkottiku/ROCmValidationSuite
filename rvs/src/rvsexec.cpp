/********************************************************************************
 *
 * Copyright (c) 2018-2024 Advanced Micro Devices, Inc. All rights reserved.
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

#include "include/rvsexec.h"

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include "yaml-cpp/yaml.h"

#include "include/rvsif0.h"
#include "include/rvsif1.h"
#include "include/rvsaction.h"
#include "include/rvsmodule.h"
#include "include/rvsliblogger.h"
#include "include/rvsoptions.h"
#include "include/rvstrace.h"
#ifdef FETCH_ROCMPATH_FROM_ROCMCORE
#include "rocm-core/rocm_getpath.h"
#endif

#define MODULE_NAME_CAPS "CLI"

using std::string;
using std::cout;
using std::endl;

//! Default constructor
rvs::exec::exec():app_callback(nullptr), user_param(0), num_times(1) {
}

//! Default destructor
rvs::exec::~exec() {
}


/**
 * @brief Main executor method.
 *
 * @return 0 if successful, non-zero otherwise
 *
 */
int rvs::exec::run() {
  int     sts = 0;
  string  val;
  string  path;

  options::has_option("pwd", &path);

  // check -h options
  if (rvs::options::has_option("-h", &val)) {
    do_help();
    return 0;
  }

  // check -v options
  logger::log_level(rvs::logerror);
  if (rvs::options::has_option("-ver", &val)) {
    do_version();
    return 0;
  }

  // check -d options
  if (rvs::options::has_option("-d", &val)) {
    int level;
    try {
      level = std::stoi(val);
    }
    catch(...) {
      char buff[1024];
      snprintf(buff, sizeof(buff),
                "logging level not integer: %s", val.c_str());
      rvs::logger::Err(buff, MODULE_NAME_CAPS);
      return -1;
    }
    if (level < 0 || level > 5) {
      char buff[1024];
      snprintf(buff, sizeof(buff),
                "logging level not in range [0..5]: %s", val.c_str());
      rvs::logger::Err(buff, MODULE_NAME_CAPS);
      return -1;
    }
    logger::log_level(level);
  }

  // if verbose is set, set logging level to the max value (i.e. 5)
  if (rvs::options::has_option("-v")) {
    logger::log_level(5);
  }

  // check -a option
  if (rvs::options::has_option("-a", &val)) {
    logger::append(true);
  }

  // check -l option
  std::string s_log_file;
  if (rvs::options::has_option("-l", &s_log_file)) {
    logger::set_log_file(s_log_file);
  }

  // check -j option
  std::string s_json_log_file;
  if (rvs::options::has_option("-j", &s_json_log_file)) {
    logger::to_json(true);
    logger::set_json_log_file(s_json_log_file);
    
  }

  // check -c option
  string config_file;
  if (rvs::options::has_option("-c", &val)) {
    config_file = val;
  } else {
    config_file = "../share/rocm-validation-suite/conf/rvs.conf";
    // Check if pConfig file exist if not use old path for backward compatibility
    std::ifstream file(path + config_file);
    if (!file.good()) {
      config_file = "conf/rvs.conf";
    }
    file.close();
    config_file = path + config_file;
  }

  // Check if pConfig file exists
  std::ifstream file(config_file);
  if (!file.good()) {
    char buff[1024];
    snprintf(buff, sizeof(buff),
              "%s file is missing.", config_file.c_str());
    rvs::logger::Err(buff, MODULE_NAME_CAPS);
    return -1;
  } else {
    file.close();
  }

  // check -n options
  if (rvs::options::has_option("-n", &val)) {
    try {
      num_times = std::stoi(val);
    }
    catch(...) {
      char buff[1024];
      snprintf(buff, sizeof(buff),
          "number of times test repeat value not an integer: %s", val.c_str());
      rvs::logger::Err(buff, MODULE_NAME_CAPS);
      return -1;
    }
  }

  // construct modules configuration file relative path
  val = path + "../share/rocm-validation-suite/conf/.rvsmodules.config";
  // Check if config file exists if not check the old file location for backward compatibility
  std::ifstream conf_file(val);
  if (!conf_file.good()) {
    val = path + ".rvsmodules.config";
  }
  conf_file.close();

  if (rvs::module::initialize(val.c_str())) {
    return 1;
  }

  if (rvs::options::has_option("-t", &val)) {
        cout << endl << "ROCm Validation Suite (version " <<
        RVS_VERSION_STRING << ")" << endl << endl;
        cout << "Modules available:" << endl;
    rvs::module::do_list_modules();
    return 0;
  }

  if (rvs::options::has_option("-q")) {
    rvs::logger::quiet();
  }

  if (logger::init_log_file()) {
    char buff[1024];
    snprintf(buff, sizeof(buff),
              "could not access log file: %s", s_log_file.c_str());
    rvs::logger::Err(buff, MODULE_NAME_CAPS);
    return -1;
  }

  if (rvs::options::has_option("-g")) {
    int sts = do_gpu_list();
    rvs::module::terminate();
    logger::terminate();
    return sts;
  }

  DTRACE_
  try {
    sts = do_yaml(yaml_data_type_t::YAML_FILE, config_file);
  } catch(std::exception& e) {
    sts = -999;
    char buff[1024];
    snprintf(buff, sizeof(buff),
             "error processing configuration file: %s", config_file.c_str());
    rvs::logger::Err(buff, MODULE_NAME_CAPS);
    snprintf(buff, sizeof(buff),
             "exception: %s", e.what());
    rvs::logger::Err(buff, MODULE_NAME_CAPS);
  }

  rvs::module::terminate();
  logger::terminate();

  DTRACE_
  if (sts) {
    DTRACE_
    return sts;
  }

  // if stop was requested
  if (rvs::logger::Stopping()) {
    DTRACE_
    return -1;
  }

  DTRACE_
  return 0;
}

/**
 * @brief Set application callback function
 * @param callback Application callback function pointer
 * @param user_param User parameter for application callback
 * @return 0 if successful, non-zero otherwise
 *
 */
int rvs::exec::set_callback(void (*callback)(const rvs_results_t * results, int user_param), int user_param) {

  if (nullptr == callback) {
    DTRACE_
    return -1;
  }

  this->app_callback = callback;
  this->user_param = user_param;
  return 0;
}

/**
 * @brief Main executor method.
 *
 * @return 0 if successful, non-zero otherwise
 *
 */
int rvs::exec::run(std::map<std::string, std::string>& opt) {

  int     sts = 0;
  string  path;
  string  module;
  string config;
  yaml_data_type_t data_type;
#ifdef FETCH_ROCMPATH_FROM_ROCMCORE
  char *installPath = nullptr;
  unsigned int installPathLen = 0;
  string rocmPath;
  PathErrors_t retVal = PathSuccess;
  // Get ROCM install path
  retVal = getROCmInstallPath( &installPath, &installPathLen );
  if(retVal == PathSuccess){
    rocmPath = installPath;
  }
  else {
    std::cout << "Failed to get ROCm Install Path: " << retVal <<"\nSet ROCM_PATH in env" << std::endl;
  }
  // free allocated memory
  if(installPath != nullptr) {
    free(installPath);
  }
#endif
  options::has_option("pwd", &path);
  logger::log_level(rvs::logerror);

  if (rvs::options::has_option(opt, "conf", &config)) {
    data_type = yaml_data_type_t::YAML_FILE;
  }
  else if (rvs::options::has_option(opt, "module", &module)) {

#define RVS_MODULE_MAX 11
    std::map <std::string, int> module_map = {
      {"babel", 0},
      {"gpup", 1},
      {"gst", 2},
      {"iet", 3},
      {"mem", 4},
      {"pebb", 5},
      {"peqt", 6},
      {"pesm", 7},
      {"pbqt", 8},
      {"rcqt", 9},
      {"smqt", 10}};

    string module_config_file[RVS_MODULE_MAX] =
    {
      "babel.conf",
      "gpup_single.conf",
      "gst_single.conf",
      "iet_single.conf",
      "mem.conf",
      "pebb_single.conf",
      "peqt_single.conf",
      "pesm_1.conf",
      "pbqt_single.conf",
      "rcqt_single.conf",
      "smqt_single.conf"
    };

    auto itr = module_map.find(module);
    int module_index = itr->second;

    if(RVS_MODULE_MAX <= module_index) {
      return -1;
    }

    config = "../share/rocm-validation-suite/conf/" + module_config_file[module_index];
    // Check if pConfig file exist if not use old path for backward compatibility
    std::ifstream file(path + config);
    if (!file.good()) {
      config = "conf/" + module_config_file[module_index];
      std::ifstream file(path + config);
      if (!file.good()) {
        // configuration file exist in ROCM path ?
#ifdef FETCH_ROCMPATH_FROM_ROCMCORE
        path = rocmPath;
#else
        path = ROCM_PATH;
#endif
        config = "/share/rocm-validation-suite/conf/" + module_config_file[module_index];
      }
      file.close();
    }
    else {
      file.close();
    }
    config = path + config;
    data_type = yaml_data_type_t::YAML_FILE;
  }
  else if (rvs::options::has_option(opt, "yaml", &config)) {
    data_type = yaml_data_type_t::YAML_STRING;
  }

  if(yaml_data_type_t::YAML_FILE == data_type) {
    // Check if pConfig file exists
    std::ifstream file(config);
    if (!file.good()) {
      char buff[1024];
      snprintf(buff, sizeof(buff),
          "%s file is missing.", config.c_str());
      rvs::logger::Err(buff, MODULE_NAME_CAPS);
      return -1;
    } else {
      file.close();
    }
  }

  // construct modules configuration file relative path
  string val = path + "../share/rocm-validation-suite/conf/.rvsmodules.config";
  // Check if config file exists if not check the old file location for backward compatibility
  std::ifstream conf_file(val);
  if (!conf_file.good()) {
    val = path + ".rvsmodules.config";
    std::ifstream conf_file(val);
    if (!conf_file.good()) {
      // Modules config. file exist in ROCM path ?
#ifdef FETCH_ROCMPATH_FROM_ROCMCORE
      path = rocmPath;
#else
      path = ROCM_PATH;
#endif
      val = path + "/share/rocm-validation-suite/conf/.rvsmodules.config";
    }
  }
  conf_file.close();

  if (rvs::module::initialize(val.c_str())) {
    return 1;
  }

  DTRACE_
    try {
      sts = do_yaml(data_type, config);
    } catch(std::exception& e) {
      sts = -999;
      char buff[1024];
      snprintf(buff, sizeof(buff),
          "error processing configuration file: %s", config.c_str());
      rvs::logger::Err(buff, MODULE_NAME_CAPS);
      snprintf(buff, sizeof(buff),
          "exception: %s", e.what());
      rvs::logger::Err(buff, MODULE_NAME_CAPS);
    }

  rvs::module::terminate();
  logger::terminate();

  DTRACE_
    if (sts) {
      DTRACE_
        return sts;
    }

  // if stop was requested
  if (rvs::logger::Stopping()) {
    DTRACE_
      return -1;
  }

  DTRACE_
    return 0;
}

//! Reports version string
void rvs::exec::do_version() {
  cout << RVS_VERSION_STRING << '\n';
}

//! Prints help
void rvs::exec::do_help() {

  cout << "\nUsage: rvs [option]... [file]...\n";
  cout << "\nOptions:\n\n";

  cout << "-a --appendLog     When generating a debug logfile, do not overwrite the content\n";
  cout << "                   of the current log. Use in conjuction with -d and -l options.\n\n";

  cout << "-c --config        Specify the test configuration file to use.\n\n";

  cout << "-d --debugLevel    Specify the debug level for the output log. The range is 0-5 with\n";
  cout << "                   5 being the highest verbose level.\n\n";

  cout << "-g --listGpus      List all the GPUs available in the machine, that RVS supports and\n";
  cout << "                   has visibility.\n\n";

  cout << "-i --indexes       Comma separated list of GPU ids/indexes to run test on. This overrides\n";
  cout << "                   the device/device_index values specified for every actions in the\n";
  cout << "                   configuration file, including the ‘all’ value.\n\n";

  cout << "-j --json          Generate output file in JSON format.\n";
  cout << "                   if a path follows this argument, that will be used as json log file\n";
  cout << "                   else a file created in /var/tmp/ with timestamp in name.\n\n";

  cout << "-l --debugLogFile  Generate log file with output and debug information.\n\n";
 

  cout << "-t --listTests     List the test modules present in RVS.\n\n";

  cout << "-v --verbose       Enable detailed logging. Equivalent to specifying -d 5 option.\n\n";

  cout << "-p --parallel      Enables or Disables parallel execution across multiple GPUs.\n";
  cout << "                   Use this option in conjunction with -c option.\n";
  cout << "                   Accepted Values:\n";
  cout << "                   true – Enables parallel execution.\n";
  cout << "                   false – Disables parallel execution.\n";
  cout << "                   If no value is provided for the option, it defaults to true.\n\n";

  cout << "-n --numTimes      Number of times the test repeatedly executes. Use in conjunction\n";
  cout << "                   with -c option.\n\n";

  cout << "   --quiet         No console output given. See logs and return code for errors.\n\n";

  cout << "   --version       Display version information and exit.\n\n";

  cout << "-h --help          Display usage information and exit.\n\n";
}

//! Reports list of AMD GPUs present in the system
int rvs::exec::do_gpu_list() {
  cout << "\nROCm Validation Suite (version " << RVS_VERSION_STRING << ")\n\n";

  // create action excutor in .so
  rvs::action* pa = module::action_create("pesm");
  if (!pa) {
    rvs::logger::Err("could not list GPUs.", MODULE_NAME_CAPS);
    return 1;
  }

  // obtain interface to set parameters and execute action
  if1* pif1 = static_cast<if1*>(pa->get_interface(1));
  if (!pif1) {
    rvs::logger::Err("could not obtain interface if1.", MODULE_NAME_CAPS);
    module::action_destroy(pa);
    return 1;
  }

  pif1->property_set("name", "(launcher)");

  // specify "list GPUs" action
  pif1->property_set("do_gpu_list", "");

  // set command line options:
  for (auto clit = rvs::options::get().begin();
       clit != rvs::options::get().end(); ++clit) {
    string p(clit->first);
    p = "cli." + p;
    pif1->property_set(p, clit->second);
  }

  // execute action
  int sts = pif1->run();

  // procssing finished, release action object
  module::action_destroy(pa);

  return sts;
}

void rvs::exec::action_callback(const action_result_t * result, void * user_param) {

  if((nullptr == result)||(nullptr == user_param)) {
    return;
  }

  static_cast<rvs::exec *>(user_param)->callback(result);

}

void rvs::exec::callback(const action_result_t * result) {

  rvs_results_t rvs_result;

  rvs_result.status = RVS_STATUS_FAILED;

  switch (result->status) {
    case rvs::actionstatus::ACTION_SUCCESS:
      {
        rvs_result.status = RVS_STATUS_SUCCESS;
      }
      break;
    case rvs::actionstatus::ACTION_FAILED:
      {
        rvs_result.status = RVS_STATUS_FAILED;
      }
      break;
    default:
      return;
  }

  rvs_result.state = RVS_SESSION_STATE_INPROGRESS;
  rvs_result.output_log = result->output.c_str();

  if(nullptr != app_callback) {
    (*app_callback)(&rvs_result, user_param);
  }
}

void rvs::exec::callback(const rvs_results_t * result) {

  if(nullptr != app_callback) {
    (*app_callback)(result, user_param);
  }
}

