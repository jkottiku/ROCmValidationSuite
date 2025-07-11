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
#ifndef GST_SO_INCLUDE_GST_WORKER_H_
#define GST_SO_INCLUDE_GST_WORKER_H_

#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "include/rvsthreadbase.h"
#include "include/rvs_blas.h"
#include "include/rvs_util.h"
#include "include/rvsactionbase.h"
#include "include/action.h"

#define GST_RESULT_PASS_MESSAGE         "true"
#define GST_RESULT_FAIL_MESSAGE         "false"


/**
 * @class GSTWorker
 * @ingroup GST
 *
 * @brief GSTWorker action implementation class
 *
 * Derives from rvs::ThreadBase and implements actual action functionality
 * in its run() method.
 *
 */
class GSTWorker : public rvs::ThreadBase {
 public:
    GSTWorker();
    virtual ~GSTWorker();

    //! sets action name
    void set_name(const std::string& name) { action_name = name; }
    //! sets action
    void set_action(const gst_action& _action) { action = _action; }
    //! returns action name
    const std::string& get_name(void) { return action_name; }

    //! sets GPU ID
    void set_gpu_id(uint16_t _gpu_id) { gpu_id = _gpu_id; }
    //! returns GPU ID
    uint16_t get_gpu_id(void) { return gpu_id; }

    //! sets the GPU index
    void set_gpu_device_index(int _gpu_device_index) {
        gpu_device_index = _gpu_device_index;
    }
    //! returns the GPU index
    int get_gpu_device_index(void) { return gpu_device_index; }

    //! sets the run delay
    void set_run_wait_ms(uint64_t _run_wait_ms) { run_wait_ms = _run_wait_ms; }
    //! returns the run delay
    uint64_t get_run_wait_ms(void) { return run_wait_ms; }

    //! sets the total stress test run duration
    void set_run_duration_ms(uint64_t _run_duration_ms) {
        run_duration_ms = _run_duration_ms;
    }
    //! returns the total stress test run duration
    uint64_t get_run_duration_ms(void) { return run_duration_ms; }

    //! sets the stress test ramp duration
    void set_ramp_interval(uint64_t _ramp_interval) {
        ramp_interval = _ramp_interval;
    }
    //! returns the stress test ramp duration
    uint64_t get_ramp_interval(void) { return ramp_interval; }

    //! sets the time interval at which the module reports the average GFlops
    void set_log_interval(uint64_t _log_interval) {
        log_interval = _log_interval;
    }
    //! returns the time interval at which the module reports the average GFlops
    uint64_t get_log_interval(void) { return log_interval; }

    //! sets the maximum allowed number of target_stress violations
    void set_max_violations(uint64_t _max_violations) {
        max_violations = _max_violations;
    }
    //! returns the maximum allowed number of target_stress violations
    uint64_t get_max_violations(void) { return max_violations; }

    //! sets the copy_matrix (true = the matrix will be copied to GPU each
    //! time a new SGEMM will run, false = the matrix will be copied only once)
    void set_copy_matrix(bool _copy_matrix) { copy_matrix = _copy_matrix; }

    //! returns the copy_matrix value
    bool get_copy_matrix(void) { return copy_matrix; }

    //! sets the target stress (in GFlops) that the GPU will try to achieve
    void set_target_stress(float _target_stress) {
        target_stress = _target_stress;
    }
    //! returns the target stress (in GFlops) that the GPU will try to achieve
    float get_target_stress(void) { return target_stress; }

    //! sets hot calls
    void set_gst_hot_calls(uint64_t _hot_calls) {
        gst_hot_calls = _hot_calls;
    }
 
    //! sets hot calls
    uint64_t get_gst_hot_calls(void) {
        return gst_hot_calls;
    }

    //! sets the matrix size
    void set_matrix_size_a(uint64_t _matrix_size_a) {
        matrix_size_a = _matrix_size_a;
    }
    //! sets the matrix size
    void set_matrix_size_b(uint64_t _matrix_size_b) {
        matrix_size_b = _matrix_size_b;
    }
    //! sets the matrix size
    void set_matrix_size_c(uint64_t _matrix_size_c) {
        matrix_size_c = _matrix_size_c;
    }
    //! sets the matrix init
    void set_matrix_init(std::string _matrix_init) {
        matrix_init = _matrix_init;
    }
    //! sets the transpose matrix a
    void set_matrix_transpose_a(int transa) {
        gst_trans_a = transa;
    }
    //! sets the transpose matrix b
    void set_matrix_transpose_b(int transb) {
        gst_trans_b = transb;
    }
    //! sets alpha val
    void set_alpha_val(float alpha_val) {
        gst_alpha_val = alpha_val;
    }
    //! sets beta val
    void set_beta_val(float beta_val) {
        gst_beta_val = beta_val;
    }

    //! sets offsets
    void set_lda_offset(int lda) {
        gst_lda_offset = lda;
    }
    //! sets offsets
    void set_ldb_offset(int ldb) {
        gst_ldb_offset = ldb;
    }
    //! sets offsets
    void set_ldc_offset(int ldc) {
        gst_ldc_offset = ldc;
    }
    //! sets offsets
    void set_ldd_offset(int ldd) {
        gst_ldd_offset = ldd;
    }

    //! returns the matrix size
    uint64_t get_matrix_size_a(void) { return matrix_size_a; }

    //! returns the matrix size
    uint64_t get_matrix_size_b(void) { return matrix_size_b; }

    //! returns the matrix size
    uint64_t get_matrix_size_c(void) { return matrix_size_c; }

    //! sets the GFlops tolerance
    void set_tolerance(float _tolerance) { tolerance = _tolerance; }

    //! returns the GFlops tolerance
    float get_tolerance(void) { return tolerance; }


    //! returns the difference (in milliseconds) between 2 points in time
    uint64_t time_diff(
                std::chrono::time_point<std::chrono::system_clock> t_end,
                    std::chrono::time_point<std::chrono::system_clock> t_start);

    //! sets the JSON flag
    static void set_use_json(bool _bjson) { bjson = _bjson; }
    //! returns the JSON flag
    static bool get_use_json(void) { return bjson; }

    //! set gemm operation type - sgemm, dgemm, hgemm
    void set_gst_ops_type(std::string _ops_type) { gst_ops_type = _ops_type; }

    //! set data format based gemm operation  - fp8, fp16, bf16
    void set_gst_data_type(std::string _data_type) { gst_data_type = _data_type; }

    //! sets gemm output self check enable/disable
    void set_self_check(bool _self_check) { self_check = _self_check; }

    //! returns the self check value
    bool get_self_check(void) { return self_check; }

    //! sets gemm output accuracy check enable/disable
    void set_accu_check(bool _accu_check) { accu_check = _accu_check; }

    //! returns the accuracy check value
    bool get_accu_check(void) { return accu_check; }

    //! sets gemm output error inject enable/disable
    void set_error_inject(bool _error_inject) { error_inject = _error_inject; }

    //! returns the error inject value
    bool get_error_inject(void) { return error_inject; }

    //! sets gemm error inject frequency value
    void set_error_frequency(uint64_t _error_freq) { error_freq = _error_freq; }

    //! returns gemm error inject frequency value
    uint64_t get_error_frequency(void) { return error_freq; }

    //! sets gemm error inject count value
    void set_error_count(uint64_t _error_count) { error_count = _error_count; }

    //! returns gemm error inject count value
    uint64_t get_error_count(void) { return error_count; }

    //! sets the gemm mode
    void set_gemm_mode(std::string _gemm_mode) { gemm_mode = _gemm_mode; }

    //! returns gemm mode
    std::string get_gemm_mode(void) { return gemm_mode; }

    //! sets the batch size
    void set_batch_size(int _batch_size) { batch_size = _batch_size; }

    //! returns the batch size
    int get_batch_size(void) { return batch_size; }

    //! sets the matrix a stride
    void set_stride_a(uint64_t _stride_a) { stride_a = _stride_a; }

    //! returns the matrix a stride
    uint64_t get_stride_a(void) { return stride_a; }

    //! sets the matrix b stride
    void set_stride_b(uint64_t _stride_b) { stride_b = _stride_b; }

    //! returns the matrix b stride
    uint64_t get_stride_b(void) { return stride_b; }

    //! sets the matrix c stride
    void set_stride_c(uint64_t _stride_c) { stride_c = _stride_c; }

    //! returns the matrix c stride
    uint64_t get_stride_c(void) { return stride_c; }

    //! sets the matrix d stride
    void set_stride_d(uint64_t _stride_d) { stride_d = _stride_d; }

    //! returns the matrix d stride
    uint64_t get_stride_d(void) { return stride_d; }

    //! set blas backend source library  - rocblas, hipblaslt
    void set_blas_source(std::string _blas_source) { blas_source = _blas_source; }

    //! set gemm compute type
    void set_compute_type(std::string _compute_type) { compute_type = _compute_type; }

    //! set output data format based gemm operation
    void set_gst_out_data_type(std::string _out_data_type) { gst_out_data_type = _out_data_type; }

    //! set scale matrix a
    void set_gst_scale_a(std::string _scale_a) { gst_scale_a = _scale_a; }
    //! set scale matrix b
    void set_gst_scale_b(std::string _scale_b) { gst_scale_b = _scale_b; }
    //! set rotating buffer size
    void set_gst_rotating(uint32_t _rotating) { gst_rotating = _rotating; }

 protected:
    void setup_blas(int *error, std::string *err_description);
    void hit_max_gflops(int *error, std::string *err_description);
    bool do_gst_ramp(int *error, std::string *err_description);
    bool do_gst_stress_test(int *error, std::string *err_description);
    void log_gst_test_result(bool gst_test_passed);
    virtual void run(void);
    void log_interval_gflops(double gflops_interval);
    bool check_gflops_violation(double gflops_interval);
    void check_target_stress(double gflops_interval);
    void usleep_ex(uint64_t microseconds);

 protected:
    //! name of the action
    std::string action_name;
    //! action instance
    gst_action action;
    //! index of the GPU that will run the stress test
    int gpu_device_index;
    //Matrix transpose A
    int gst_trans_a;
    //Matrix transpose B
    int gst_trans_b;
    //! ID of the GPU that will run the stress test
    uint16_t gpu_id;
    //GST aplha value 
    float gst_alpha_val;
    //GST beta value
    float gst_beta_val;
    //leading offsets
    int gst_lda_offset;
    int gst_ldb_offset;
    int gst_ldc_offset;
    int gst_ldd_offset;
    //! stress test run delay
    uint64_t run_wait_ms;
    //! stress test run duration
    uint64_t run_duration_ms;
    //! stress test ramp duration
    uint64_t ramp_interval;
    //! time interval at which the module reports the average GFlops
    uint64_t log_interval;
    //! maximum allowed number of target_stress violations
    uint64_t max_violations;
    //! specifies whether to copy the matrix to the GPU for each SGEMM operation
    bool copy_matrix;
    //! target stress (in GFlops) that the GPU will try to achieve
    float target_stress;
    //! GFlops tolerance (how much the GFlops can fluctuare after
    //! the ramp period for the test to succeed)
    float tolerance;
    //! matrix sizes
    uint64_t matrix_size_a;
    uint64_t matrix_size_b;
    uint64_t matrix_size_c;
    //! matrix init
    std::string matrix_init;
    //num of hot calls
    uint64_t gst_hot_calls;
    //! actual ramp time in case the GPU achieves the given target_stress Gflops
    uint64_t ramp_actual_time;
    //! rvs_blas pointer
    std::unique_ptr<rvs_blas> gpu_blas;
    //! max gflops achieved during the stress test
    double max_gflops;
    //! delay used to reduce SGEMM frequency
    double delay_target_stress;
    //! TRUE if JSON output is required
    static bool bjson;
    //! Type of operation
    std::string gst_ops_type;
    //! Type of data
    std::string gst_data_type;
    //! GEMM operations synchronization mutex
    std::mutex mutex;
    //! GEMM operations synchronization condition variable
    std::condition_variable cv;
    //! blas gemm operations status
    bool blas_status;
    //! gemm output self-check
    bool self_check;
    //! gemm output accuracy-check
    bool accu_check;
    //! Inject error in gemm output
    bool error_inject;
    //! error injection frequency (number of gemm calls per error injection)
    uint64_t error_freq;
    //! number of errors injected in gemm output
    uint64_t error_count;
    //! gemm mode : basic (single), batched or strided batched
    std::string gemm_mode;
    //! Matrix batch count
    int batch_size;
    //! Stride from the start of matrix a(i)
    //! to next matrix a(i+1) in the strided batch
    uint64_t stride_a;
    //! Stride from the start of matrix b(i)
    //! to next matrix b(i+1) in the strided batch
    uint64_t stride_b;
    //! Stride from the start of matrix c(i)
    //! to next matrix c(i+1) in the strided batch
    uint64_t stride_c;
    //! Stride from the start of matrix d(i)
    //! to next matrix d(i+1) in the strided batch
    uint64_t stride_d;
    //! blas backend source library - rocblas,hipblaslt
    std::string blas_source;
    //! gemm compute type
    std::string compute_type;
    //! Type of output data
    std::string gst_out_data_type;
    //! Scale matrix a
    std::string gst_scale_a;
    //! Scale matrix b
    std::string gst_scale_b;
    //! Rotating buffer size
    uint32_t gst_rotating;
};

#endif  // GST_SO_INCLUDE_GST_WORKER_H_
