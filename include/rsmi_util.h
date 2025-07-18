/********************************************************************************
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
#ifndef INCLUDE_RSMI_UTIL_H_
#define INCLUDE_RSMI_UTIL_H_

#include <stdint.h>
#include <cstddef>
#include <map>
#include <vector>

#include "amd_smi/amdsmi.h"

namespace rvs {
extern std::map<uint64_t, amdsmi_processor_handle> smipci_to_hdl_map;
amdsmi_status_t smi_pci_hdl_mapping();
amdsmi_status_t smi_dev_ind_get(uint64_t bdfid, amdsmi_processor_handle* pdv_hdl);
std::map<uint64_t, amdsmi_processor_handle> get_smi_pci_map();
}

#endif  // INCLUDE_RSMI_UTIL_H_
