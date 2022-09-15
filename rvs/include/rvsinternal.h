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

#include <include/rvs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RVS_MAX_SESSIONS 1

typedef enum {
  RVS_STATE_INITIALIZED,
  RVS_STATE_UNINITIALIZED
} rvs_state_t;

typedef enum {
  RVS_SESSION_STATE_IDLE = 0,
  RVS_SESSION_STATE_CREATED,
  RVS_SESSION_STATE_READY,
  RVS_SESSION_STATE_INPROGRESS,
  RVS_SESSION_STATE_COMPLETED
} rvs_session_state_t;

typedef struct rvs_session_ {
  rvs_session_id_t id;
  rvs_session_state_t state;
  rvs_session_callback callback;
  rvs_session_property_t property;
} rvs_session_t;

#ifdef __cplusplus
}
#endif
