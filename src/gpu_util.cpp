/********************************************************************************
 *
 * Copyright (c) 2018-2025 Advanced Micro Devices, Inc. All rights reserved.
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
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "amd_smi/amdsmi.h"
#include "include/gpu_util.h"
#include "include/rsmi_util.h"
#define __HIP_PLATFORM_HCC__
#include "hip/hip_runtime.h"
#include "hip/hip_runtime_api.h"

std::vector<uint16_t> rvs::gpulist::location_id;
std::vector<uint16_t> rvs::gpulist::gpu_id;
std::vector<uint16_t> rvs::gpulist::gpu_idx;
std::vector<uint16_t> rvs::gpulist::device_id;
std::vector<uint16_t> rvs::gpulist::node_id;
std::vector<uint16_t> rvs::gpulist::domain_id;
std::map<std::pair<uint16_t, uint16_t> , uint16_t> rvs::gpulist::domain_loc_map;
std::vector<std::string> rvs::gpulist::pci_bdf;

using std::vector;
using std::string;
using std::ifstream;

int gpu_num_subdirs(const char* dirpath, const char* prefix) {
  int count = 0;
  DIR *dirp;
  struct dirent *dir;
  int prefix_len = strlen(prefix);

  dirp = opendir(dirpath);
  if (dirp) {
    while ((dir = readdir(dirp)) != 0) {
      if ((strcmp(dir->d_name, ".") == 0) ||
        (strcmp(dir->d_name, "..") == 0))
        continue;
      if (prefix_len &&
        strncmp(dir->d_name, prefix, prefix_len))
        continue;
      count++;
    }
    closedir(dirp);
  }
  return count;
}

/**
 * gets all GPUS location_id
 * @param pgpus_location_id ptr to vector that will store all the GPU location_id
 * @return
 */
void gpu_get_all_location_id(std::vector<uint16_t>* pgpus_location_id) {
  ifstream f_id, f_prop;
  char path[KFD_PATH_MAX_LENGTH];

  std::string prop_name;
  int gpu_id;
  uint32_t prop_val;


  // Discover the number of nodes: Inside nodes folder there are only folders
  // that represent the node number
  int num_nodes = gpu_num_subdirs(KFD_SYS_PATH_NODES, "");

  // get all GPUs device id
  for (int node_id = 0; node_id < num_nodes; node_id++) {
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/gpu_id", KFD_SYS_PATH_NODES,
           node_id);
    f_id.open(path);
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/properties",
             KFD_SYS_PATH_NODES, node_id);
    f_prop.open(path);

    f_id >> gpu_id;
    if (gpu_id != 0) {
      while (f_prop >> prop_name) {
        if (prop_name == "location_id") {
          f_prop >> prop_val;
          (*pgpus_location_id).push_back(prop_val);
          break;
        }
      }
    }
    f_id.close();
    f_prop.close();
  }
}

/**
 * gets all GPUS gpu_id
 * @param pgpus_id ptr to vector that will store all the GPU gpu_id
 * @return
 */
void gpu_get_all_gpu_id(std::vector<uint16_t>* pgpus_id) {
  ifstream f_id, f_prop;
  char path[KFD_PATH_MAX_LENGTH];

  int gpu_id;

  // Discover the number of nodes: Inside nodes folder there are only folders
  // that represent the node number
  int num_nodes = gpu_num_subdirs(KFD_SYS_PATH_NODES, "");

  // get all GPUs device id
  for (int node_id = 0; node_id < num_nodes; node_id++) {
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/gpu_id", KFD_SYS_PATH_NODES,
             node_id);
    f_id.open(path);

    f_id >> gpu_id;

    if (gpu_id != 0)
      (*pgpus_id).push_back(gpu_id);

    f_id.close();
  }
}

/**
 * gets all GPUS device_id
 * @param pgpus_device_id ptr to vector that will store all the GPU location_id
 * @return
 */
void gpu_get_all_device_id(std::vector<uint16_t>* pgpus_device_id) {
  ifstream f_id, f_prop;
  char path[KFD_PATH_MAX_LENGTH];

  std::string prop_name;
  int gpu_id;
  uint32_t prop_val;

  // Discover the number of nodes: Inside nodes folder there are only folders
  // that represent the node number
  int num_nodes = gpu_num_subdirs(KFD_SYS_PATH_NODES, "");

  // get all GPUs device id
  for (int node_id = 0; node_id < num_nodes; node_id++) {
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/gpu_id", KFD_SYS_PATH_NODES,
             node_id);
    f_id.open(path);
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/properties",
             KFD_SYS_PATH_NODES, node_id);
    f_prop.open(path);

    f_id >> gpu_id;

    if (gpu_id != 0) {
      while (f_prop >> prop_name) {
        if (prop_name == "device_id") {
          f_prop >> prop_val;
          (*pgpus_device_id).push_back(prop_val);
          break;
        }
      }
    }

    f_id.close();
    f_prop.close();
  }
}

vector<amdsmi_processor_handle>
get_smi_processors(){
  std::vector<amdsmi_processor_handle> proc_handles;
  uint32_t socket_num = 0;
  auto ret = amdsmi_get_socket_handles(&socket_num, nullptr);
  std::vector<amdsmi_socket_handle> sockets(socket_num);
  ret = amdsmi_get_socket_handles(&socket_num, &sockets[0]);
  for(auto socket : sockets){
    uint32_t dev_cnt = 0;
    ret = amdsmi_get_processor_handles(socket, &dev_cnt, nullptr);
    std::vector<amdsmi_processor_handle> dev_handles(dev_cnt);
    ret = amdsmi_get_processor_handles(socket,
        &dev_cnt, &dev_handles[0]);
    if (ret == AMDSMI_STATUS_SUCCESS){
      for (auto dev : dev_handles){
        processor_type_t processor_type;
	amdsmi_get_processor_type(dev, &processor_type);
	if (processor_type == AMDSMI_PROCESSOR_TYPE_AMD_GPU) {
          proc_handles.push_back(dev);
        }
      }
    }
  }
  return proc_handles;
}

/**
 * gets all GPU device indexes
 * @param pgpus_device_idx ptr to vector that will store all the GPU indexes
 * @return
 */
void gpu_get_all_gpu_idx(std::vector<uint16_t>* pgpus_gpu_idx) {

  std::map<uint64_t, uint16_t> smi_map;
  //uint32_t smi_num_devices = 0;
  uint64_t gpuid;
  amdsmi_status_t smi_ret;
  auto proc_handles = get_smi_processors();
  if (proc_handles.empty()){
    return;
  }
  for(auto i=0; i<proc_handles.size();++i ){
    amdsmi_kfd_info_t kfd_info;
    smi_ret = amdsmi_get_gpu_kfd_info(proc_handles[i], &kfd_info);
    if(AMDSMI_STATUS_SUCCESS == smi_ret){
      smi_map.insert({kfd_info.kfd_id, i});
    }
  }
  ifstream f_id, f_prop;
  char path[KFD_PATH_MAX_LENGTH];

  std::string prop_name;
  int gpu_id;
  uint32_t prop_val;

  // Discover the number of nodes: Inside nodes folder there are only folders
  // that represent the node number
  int num_nodes = gpu_num_subdirs(KFD_SYS_PATH_NODES, "");

  // get all GPUs device id
  for (int node_id = 0; node_id < num_nodes; node_id++) {
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/gpu_id", KFD_SYS_PATH_NODES,
        node_id);
    f_id.open(path);
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/properties",
        KFD_SYS_PATH_NODES, node_id);
    f_prop.open(path);

    f_id >> gpu_id;

    if (gpu_id != 0) {

      if(smi_map.find(gpu_id) != smi_map.end()) {
        (*pgpus_gpu_idx).push_back(smi_map[gpu_id]);
      }

    }

    f_id.close();
    f_prop.close();
  }
}

/**
 * gets all GPUS nodes
 * @param pgpus_node_id ptr to vector that will store all the GPU nodes
 * @return
 */
void gpu_get_all_node_id(std::vector<uint16_t>* pgpus_node_id) {
  ifstream f_id;
  char path[KFD_PATH_MAX_LENGTH];
  int gpu_id;


  // Discover the number of nodes: Inside nodes folder there are only folders
  // that represent the node number
  int num_nodes = gpu_num_subdirs(KFD_SYS_PATH_NODES, "");

  // get all GPUs device id
  for (int node_id = 0; node_id < num_nodes; node_id++) {
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/gpu_id", KFD_SYS_PATH_NODES,
           node_id);
    f_id.open(path);
    f_id >> gpu_id;

    if (gpu_id != 0) {
      (*pgpus_node_id).push_back(node_id);
    }
    f_id.close();
  }
}

/**
 * gets all GPUS' domain id which along BDF forms whole address
 * @param pgpus_domain_id ptr to vector that will store all the GPU domain_id
 * @return
 */
void gpu_get_all_domain_id(std::vector<uint16_t>* pgpus_domain_id,
    std::map<std::pair<uint16_t, uint16_t> , uint16_t>& pgpus_dom_loc_map) {

  ifstream f_id, f_prop;
  char path[KFD_PATH_MAX_LENGTH];
  std::string prop_name;
  int gpu_id;
  uint32_t domain_val;
  uint32_t loc_val;

  // Discover the number of nodes: Inside nodes folder there are only folders
  // that represent the node number
  int num_nodes = gpu_num_subdirs(KFD_SYS_PATH_NODES, "");

  // get all GPUs device id
  for (int node_id = 0; node_id < num_nodes; node_id++) {
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/gpu_id", KFD_SYS_PATH_NODES,
        node_id);
    f_id.open(path);
    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/properties",
        KFD_SYS_PATH_NODES, node_id);
    f_prop.open(path);

    f_id >> gpu_id;

    if (gpu_id != 0) {
      while (f_prop >> prop_name) {
        if (prop_name == "domain") {
          f_prop >> domain_val;
          (*pgpus_domain_id).push_back(domain_val);
          continue;
        }
        else if(prop_name == "location_id"){
          f_prop >> loc_val;
          continue;
        }
      }
      pgpus_dom_loc_map[std::make_pair(domain_val, loc_val)] = gpu_id;
    }

    f_id.close();
    f_prop.close();
  }
}

/**
 * Get PCI BDF (Bus, Device, Function) for all GPUs.
 * @param pgpus_domain_id ptr to vector that will store PCI BDF
 * @return void
 */
void gpu_get_all_pci_bdf(std::vector<std::string>& ppci_bdf) {

  ifstream f_id, f_prop;
  char path[KFD_PATH_MAX_LENGTH];
  std::string prop_name;
  int gpu_id;
  uint32_t domain_val;
  uint32_t location_val;
  std::string domain;
  std::string bus;
  std::string device;

  // Discover the number of nodes: Inside nodes folder there are only folders
  // that represent the node number
  int num_nodes = gpu_num_subdirs(KFD_SYS_PATH_NODES, "");

  // get all GPUs device id
  for (int node_id = 0; node_id < num_nodes; node_id++) {

    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/gpu_id", KFD_SYS_PATH_NODES,
        node_id);
    f_id.open(path);

    snprintf(path, KFD_PATH_MAX_LENGTH, "%s/%d/properties",
        KFD_SYS_PATH_NODES, node_id);
    f_prop.open(path);

    f_id >> gpu_id;

    if (gpu_id != 0) {

      while (f_prop >> prop_name) {

        if (prop_name == "domain") {

          // Get domain string
          f_prop >> domain_val;

          std::stringstream ss;
          ss << std::hex << domain_val;
          domain =  ss.str();

          domain = std::string(4 - std::min(4, (int)domain.length()), '0') + domain;

          continue;
        }
        else if(prop_name == "location_id") {

          // Get bus and device string
          f_prop >> location_val;

          std::stringstream ss;
          ss << std::hex << location_val;
          std::string location (ss.str());

          location = std::string(4 - std::min(4, (int)location.length()), '0') + location;

          bus = location.substr(0,2);
          device = location.substr(2,2);

          continue;
        }
      }

      /* Form PCI BDF */
      ppci_bdf.push_back(domain + ":" + bus + ":" + device + ".0" );
    }

    f_id.close();
    f_prop.close();
  }
}

/**
 * @brief Check if the GPU is die (chiplet) in Multi-Chip Module (MCM) GPU.
 * @param device_id GPU Device ID
 * @return true if GPU is die in MCM GPU, false if GPU is single die GPU.
 **/
bool gpu_check_if_mcm_die (int idx) {
  amdsmi_status_t ret;
  uint64_t val =0 , time_stamp;
  float cntr_resolution;
  amdsmi_processor_handle smi_hdl = 0;

  if (gpu_hip_to_smi_hdl(idx, &smi_hdl)) {
    return false;
  }

  // in case of secondary die, energy accumulator will return zero. 
  ret = amdsmi_get_energy_count(smi_hdl, &val, &cntr_resolution, &time_stamp);
  if (!((AMDSMI_STATUS_SUCCESS == ret) && val == 0))
    return false;
  return true;
}

/**
 * @brief Get GPU smi index from hip index.
 * @param hip_index GPU hip index
 * @param smi_index GPU smi index
 * @return 0 if successful, -1 otherwise
 **/
int gpu_hip_to_smi_hdl(int hip_index, amdsmi_processor_handle* smi_hdl) {

  int hip_num_gpu_devices = 0;
  uint32_t smi_num_devices = 0;
  uint64_t smi_bdf_id = 0;

  // map this to smi as only these are visible
  hipGetDeviceCount(&hip_num_gpu_devices);
  if(hip_index >= hip_num_gpu_devices) {
    return -1;
  }
  auto smi_map = rvs::get_smi_pci_map();
  /* Fetch Domain, Bus, Device and Function numbers from HIP PCIe id */
  uint64_t pDom = 0, pBus = 0, pDev = 0, pFun = 0;
  char pciString[256] = {0};
  auto hipret = hipDeviceGetPCIBusId(pciString, 256, hip_index);
  if (sscanf(pciString, "%04x:%02x:%02x.%01x", reinterpret_cast<uint64_t *>(&pDom),
        reinterpret_cast<uint64_t *>(&pBus),
        reinterpret_cast<uint64_t *>(&pDev),
        reinterpret_cast<uint64_t *>(&pFun)) != 4) {
    std::cout << "parsing error in BDF:" << pciString << std::endl;
  }

  /* Form bdfid as per rsmi_dev_pci_id_get() logic */
  uint64_t hip_bdf_id =
	  ((((pDom) & 0x00000000ffffffff) << 32) |
	   (((pBus) & 0x00000000000000ff) << 8) |
	   (((pDev) & 0x000000000000001f) << 3) |
	    ((pFun) & 0x0000000000000007));

  /* Check for matching bdf id in smi list */
  if(smi_map.find(hip_bdf_id) != smi_map.end()) {
    *smi_hdl = smi_map[hip_bdf_id];
    return 0;
  }
  return -1;
}

/**
 * @brief Initialize gpulist helper class
 * @return 0 if successful, -1 otherwise
 **/
int rvs::gpulist::Initialize() {
  amdsmi_init(AMDSMI_INIT_AMD_GPUS);
  gpu_get_all_location_id(&location_id);
  gpu_get_all_gpu_id(&gpu_id);
  gpu_get_all_gpu_idx(&gpu_idx);
  gpu_get_all_device_id(&device_id);
  gpu_get_all_node_id(&node_id);
  gpu_get_all_domain_id(&domain_id, domain_loc_map);
  gpu_get_all_pci_bdf(pci_bdf);

  return 0;
}


/**
 * @brief Given Gpu ID return Location ID
 * @param GpuID Gpu ID
 * @param pLocationID Location ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::gpu2location(const uint16_t GpuID,
                               uint16_t* pLocationID) {
  const auto it = std::find(gpu_id.cbegin(),
                            gpu_id.cend(), GpuID);
  if (it == gpu_id.cend()) {
    return -1;
  }
  size_t pos = std::distance(gpu_id.cbegin(), it);
  *pLocationID = location_id[pos];
  return 0;
}


/**
 * @brief Given Location ID return GPU ID
 * @param LocationID Location ID of a GPU
 * @param pGpuID GPU ID of the GPU on Location ID
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::location2gpu(const uint16_t LocationID, uint16_t* pGpuID) {
  const auto it = std::find(location_id.cbegin(),
                            location_id.cend(), LocationID);
  if (it == location_id.cend()) {
    return -1;
  }
  size_t pos = std::distance(location_id.cbegin(), it);
  *pGpuID = gpu_id[pos];
  return 0;
}


/**
 * @brief Given Node ID return GPU ID
 * @param NodeID Location ID of a GPU
 * @param pGpuID device ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::node2gpu(const uint16_t NodeID, uint16_t* pGpuID) {
  const auto it = std::find(node_id.cbegin(),
                            node_id.cend(), NodeID);
  if (it == node_id.cend()) {
    return -1;
  }
  size_t pos = std::distance(node_id.cbegin(), it);
  *pGpuID = gpu_id[pos];
  return 0;
}

/**
 * @brief Given Node ID return PCI BDF
 * @param NodeID GPU Node ID
 * @param pPciBDF GPU PCI BDF
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::node2bdf(const uint16_t NodeID, std::string& pPciBDF) {

  const auto it = std::find(node_id.cbegin(),
                            node_id.cend(), NodeID);
  if (it == node_id.cend()) {
    return -1;
  }

  size_t pos = std::distance(node_id.cbegin(), it);
  pPciBDF = pci_bdf[pos];
  return 0;
}

/**
 * @brief Given Location ID return GPU device ID
 * @param LocationID Location ID of a GPU
 * @param pDeviceID device ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::location2device(const uint16_t LocationID,
                                  uint16_t* pDeviceID) {
  const auto it = std::find(location_id.cbegin(),
                            location_id.cend(), LocationID);
  if (it == location_id.cend()) {
    return -1;
  }
  size_t pos = std::distance(location_id.cbegin(), it);
  *pDeviceID = device_id[pos];
  return 0;
}


/**
 * @brief Given Gpu ID return GPU device ID
 * @param GpuID Gpu ID of a GPU
 * @param pDeviceID device ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::gpu2device(const uint16_t GpuID, uint16_t* pDeviceID) {
  const auto it = std::find(gpu_id.cbegin(),
                            gpu_id.cend(), GpuID);
  if (it == gpu_id.cend()) {
    return -1;
  }

  size_t pos = std::distance(gpu_id.cbegin(), it);
  *pDeviceID = device_id[pos];
  return 0;
}

/**
 * @brief Given Gpu ID return GPU device index
 * @param GpuID Gpu ID of a GPU
 * @param pDeviceID device ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::gpu2gpuindex(const uint16_t GpuID, uint16_t* pGpuIdx) {
  const auto it = std::find(gpu_id.cbegin(),
                            gpu_id.cend(), GpuID);
  if (it == gpu_id.cend()) {
    return -1;
  }
  size_t pos = std::distance(gpu_id.cbegin(), it);
  *pGpuIdx = gpu_idx[pos];
  return 0;
}


/**
 * @brief Given Gpu ID return GPU HSA Node ID
 * @param GpuID Gpu ID of a GPU
 * @param pNodeID Node ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::gpu2node(const uint16_t GpuID, uint16_t* pNodeID) {
  const auto it = std::find(gpu_id.cbegin(),
                            gpu_id.cend(), GpuID);
  if (it == gpu_id.cend()) {
    return -1;
  }

  size_t pos = std::distance(gpu_id.cbegin(), it);
  *pNodeID = node_id[pos];
  return 0;
}


/**
 * @brief Given Location ID return GPU node ID
 * @param LocationID Location ID of a GPU
 * @param pNodeID Node ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::location2node(const uint16_t LocationID,
                                    uint16_t* pNodeID) {
  const auto it = std::find(location_id.cbegin(),
                            location_id.cend(), LocationID);
  if (it == location_id.cend()) {
    return -1;
  }

  size_t pos = std::distance(location_id.cbegin(), it);
  *pNodeID = node_id[pos];
  return 0;
}

/**
 * @brief Given domain id and Location ID return GPU node ID
 * @param LocationID Location ID of a GPU
 * @param pNodeID Node ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::domlocation2node(const uint16_t domainID, const uint16_t LocationID,
                                    uint16_t* pNodeID) {
  auto it = domain_loc_map.find(std::make_pair(domainID, LocationID));
  if (it == domain_loc_map.end()) {
    return -1;
  }

  return gpu2node(it->second, pNodeID);
}

/**
 * @brief Given domain id and Location ID return GPU node ID
 * @param LocationID Location ID of a GPU
 * @param pNodeID Node ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::domlocation2gpu(const uint16_t domainID, const uint16_t LocationID,
                                    uint16_t* pGPUID) {
  auto it = domain_loc_map.find(std::make_pair(domainID, LocationID));
  if (it == domain_loc_map.end()) {
    return -1;
  }
  *pGPUID = it->second;
  return 0;
}

/**
 * @brief Given Gpu ID return GPU domain ID
 * @param GpuID Gpu ID of a GPU
 * @param pDomain domain ID of the GPU
 * @return 0 if found, -1 otherwise
 **/
int rvs::gpulist::gpu2domain(const uint16_t GpuID, uint16_t* pDomain) {
  const auto it = std::find(gpu_id.cbegin(),
                            gpu_id.cend(), GpuID);
  if (it == gpu_id.cend()) {
    return -1;
  }
  size_t pos = std::distance(gpu_id.cbegin(), it);
  *pDomain = domain_id[pos];
  return 0;
}

/**
 * @brief Check if the indexes list is gpu indexes
 * @param 
 * @return true if indexes are GPU indexes
 **/
bool gpu_check_if_gpu_indexes (const std::vector <uint16_t> &index) {

  uint32_t smi_num_devices = 0;
  smi_num_devices = rvs::get_smi_pci_map().size();
  for(auto i = 0; i < index.size(); i++) {
    if(index[i] >= smi_num_devices) {
      return false;
    }
  }
  return true;
}

