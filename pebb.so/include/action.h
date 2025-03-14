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
#ifndef PEBB_SO_INCLUDE_ACTION_H_
#define PEBB_SO_INCLUDE_ACTION_H_

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <limits>
#include <string>
#include <vector>

#include "include/rvsactionbase.h"
#include "include/worker.h"
#include "include/rvshsa.h"


/**
 * @class pebb_action
 * @ingroup PEBB
 *
 * @brief PEBB action implementation class
 *
 * Derives from rvs::actionbase and implements actual action functionality
 * in its run() method.
 *
 */
class pebb_action : public rvs::actionbase {
 public:
  pebb_action();
  virtual ~pebb_action();
  virtual int run(void);

  typedef struct bandwidth{
     string         finalBandwith;
     uint16_t       GPUId;
     uint16_t       CPUId;
  }bandwidth;

  vector<bandwidth>   resultBandwidth;
 protected:
  bool get_all_pebb_config_keys(void);
  //! 'true' if "all" is found under "peer" key for this action
  bool      prop_peer_device_all_selected;

  //! array of peer GPU IDs to be used in data trasfers
  std::vector<std::string> prop_peers;
  //! deviceid of peer GPUs
  int  prop_peer_deviceid;
  //! 'true' if bandwidth test is to be executed for verified peers
  bool prop_test_bandwidth;
  //! 'true' if bidirectional data transfer is required
  bool prop_bidirectional;

  //! 'true' if host to device transfer is required
  bool prop_h2d;
  //! 'true' if device to host transfer is required
  bool prop_d2h;

  //! list of test block sizes
  std::vector<uint32_t> block_size;
  //! set to 'true' if the default block sizes are to be used
  bool b_block_size_all;
  //! test block size for back-to-back transfers
  uint32_t b2b_block_size;
  //! link type
  int link_type;
  std::string link_type_string;

 //! Number of warm calls (transfer iterations) before bandwidth calculation (hot calls)
 //! to ignore few intial transfers for the bandwidth to settle
 uint32_t warm_calls;
 //! Number of hot calls (transfer iterations) for bandwidth calculation after warm calls
 uint32_t hot_calls;
 //! set to true for back-to-back transfers (resource allocation only once for entire transfer iterations)
 bool b2b;

 protected:
  int create_threads();
  int destroy_threads();

  int run_single();
  int run_parallel();

  int print_link_info(int SrcNode, int DstNode, int DstGpuID,
                      uint32_t Distance,
                      const std::vector<rvs::linkinfo_t>& arrLinkInfo,
                      bool bReverse);
  void* json_base_node(int log_level);
  void json_add_kv(void *json_node, const std::string &key, const std::string &value);
  void json_to_file(void *json_node,int log_level);
  void log_json_bandwidth(std::string srcnode, std::string dstnode,
                 int log_level, std::string bandwidth = "");
  int print_running_average();
  int print_running_average(pebbworker* pWorker);
  int print_final_average();

  //! 'true' for the duration of test
  bool brun;

 private:
  void do_running_average(void);
  void do_final_average(void);

  std::vector<pebbworker*> test_array;
};

#endif  // PEBB_SO_INCLUDE_ACTION_H_
