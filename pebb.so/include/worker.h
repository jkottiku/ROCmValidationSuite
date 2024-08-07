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
#ifndef PEBB_SO_INCLUDE_WORKER_H_
#define PEBB_SO_INCLUDE_WORKER_H_

#include <string>
#include <vector>
#include <mutex>

#include "include/rvsthreadbase.h"


/**
 * @class pebbworker
 * @ingroup PEBB
 *
 * @brief Bandwidth test implementation class
 *
 * Derives from rvs::ThreadBase and implements actual test functionality
 * in its run() method.
 *
 */

namespace rvs {
class hsa;
}

class pebbworker : public rvs::ThreadBase {
 public:
  //! default constructor
  pebbworker();
  //! default destructor
  virtual ~pebbworker();

  //! stop thread loop and exit thread
  void stop();
  //! Sets initiating action name
  void set_name(const std::string& name) { action_name = name; }
  //! sets stopping action name
  void set_stop_name(const std::string& name) { stop_action_name = name; }
  //! Sets JSON flag
  void json(const bool flag) { bjson = flag; }
  //! Returns initiating action name
  const std::string& get_name(void) { return action_name; }

  int initialize(uint16_t iSrc, uint16_t iDst, bool h2d, bool d2h);
  virtual int do_transfer();
  void get_running_data(uint16_t* Src, uint16_t* Dst, bool* Bidirect,
                        size_t* Size, double* Duration);
  void get_final_data(uint16_t* Src, uint16_t* Dst, bool* Bidirect,
                      size_t* Size, double* Duration, bool bReset = true);

  //! Set transfer index
  void set_transfer_ix(uint16_t val) { transfer_ix = val; }
  //! Get transfer index
  uint16_t get_transfer_ix() { return transfer_ix; }
  //! Set total number of transfers
  void set_transfer_num(uint16_t val) { transfer_num = val; }
  //! Get total number of transfers
  uint16_t get_transfer_num() { return transfer_num; }
  //! Set list of test sizes
  void set_block_sizes(const std::vector<uint32_t>& val) { block_size = val; }
  //! Set logging level
  void set_loglevel(const int level) { loglevel = level; }

  //! Set hot calls
  void set_hot_calls(uint32_t _hot_calls) { hot_calls = _hot_calls; }
  //! Set warm calls
  void set_warm_calls(uint32_t _warm_calls) { warm_calls = _warm_calls; }
  //! Set b2b
  void set_b2b(bool _b2b) { b2b = _b2b; }

 protected:
  virtual void run(void);

 protected:
  //! TRUE if JSON output is required
  bool    bjson;
  //! Loops while TRUE
  bool    brun;
  //! Name of the action which initiated thread
  std::string  action_name;
  //! Name of the action which stops thread
  std::string  stop_action_name;

  //! ptr to RVS HSA singleton wrapper
  rvs::hsa* pHsa;
  //! source NUMA node
  uint16_t src_node;
  //! destination NUMA node
  uint16_t dst_node;
  //! 'true' for bidirectional transfer
  bool bidirect;
  //! 'true' if host to device transfer is required
  bool prop_h2d;
  //! 'true' if device to host transfer is required
  bool prop_d2h;

  //! Current size of transfer data
  size_t current_size;

  //! running total for size (bytes)
  size_t running_size;
  //! running total for duration (sec)
  double running_duration;

  //! final total size (bytes)
  size_t total_size;
  //! final total duration (sec)
  double total_duration;

  //! transfer index
  uint16_t transfer_ix;
  //! total number of transfers
  uint16_t transfer_num;
  //! logging level
  int loglevel;

  //! hot calls
  uint32_t hot_calls;
  //! warm calls
  uint32_t warm_calls;
  //! 'true' if back-to-back transfers enabled
  bool b2b;

  //! list of test block sizes
  std::vector<uint32_t> block_size;

  //! synchronization mutex
  std::mutex cntmutex;
};

#endif  // PEBB_SO_INCLUDE_WORKER_H_
