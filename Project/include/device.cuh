#ifndef __DEVICE_CUH__
#define __DEVICE_CUH__

#include "commondef.h"
#include "util.h"
#include "list.h"
#include "env_var.h"
#include "cupti_lookup.h"

#include <stdlib.h>
#include <time.h>

#include <vector>
#include <unordered_map>

#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>

#include <pthread.h>

#include <cupti.h>
#include <cuda.h>
#include <cuda_runtime.h>

#define EXTC extern "C"
#define DEV __device__
#define HOST __host__
#define GLOBAL __global__
#define NVCD_DEV_EXPORT static inline DEV
#define NVCD_INLINE inline

#define DEV_PRINT_PTR(v) printf("&(%s) = %p, %s = %p\n", #v, &v, #v, v)

#define NVCD_CUDA_EXPORT static inline

#define NVCD_KERNEL_EXEC(kname, grid, block, ...)       \
  do {                                                  \
    while (!nvcd_host_finished()) {                     \
      kname<<<grid, block>>>(__VA_ARGS__);              \
      CUDA_RUNTIME_FN(cudaDeviceSynchronize());         \
    }                                                   \
  } while (0)

//--------------------------------------
// BASE API data
//-------------------------------------

extern "C" {
  typedef struct nvcd {
    CUdevice* devices;
    CUcontext* contexts;

    int num_devices;
  
    bool32_t initialized;
  } nvcd_t;

  NVCD_CUDA_EXPORT void nvcd_init_cuda(nvcd_t* nvcd) {
    if (!nvcd->initialized) {
      CUDA_DRIVER_FN(cuInit(0));
  
      CUDA_RUNTIME_FN(cudaGetDeviceCount(&nvcd->num_devices));

      nvcd->devices = (CUdevice*)zallocNN(sizeof(*(nvcd->devices)) *
                                          nvcd->num_devices);

      nvcd->contexts = (CUcontext*)zallocNN(sizeof(*(nvcd->contexts)) *
                                            nvcd->num_devices);
  
      for (int i = 0; i < nvcd->num_devices; ++i) {
        CUDA_DRIVER_FN(cuDeviceGet(&nvcd->devices[i], i));
        
        CUDA_DRIVER_FN(cuCtxCreate(&nvcd->contexts[i],
                                   0,
                                   nvcd->devices[i]));
      }

      nvcd->initialized = true;
    }
  }

  static nvcd_t g_nvcd = {
    /*.devices =*/ NULL,
    /*.contexts =*/ NULL,
    /*.num_devices =*/ 0,
    /*.initialized =*/ false
  };
}

//--------------------------------------
// CUPTI event data
//-------------------------------------

extern "C" {

    static cupti_event_data_t g_event_data = {
      /*.event_id_buffer =*/ NULL, 
      /*.event_counter_buffer =*/ NULL, 

      /*.num_events_per_group =*/ NULL, 
      /*.num_events_read_per_group =*/ NULL,
      /*.num_instances_per_group =*/ NULL,

      /*.event_counter_buffer_offsets =*/ NULL,
      /*.event_id_buffer_offsets =*/ NULL,
      /*.event_groups_read =*/ NULL,

      /*.kernel_times_nsec_start =*/ NULL,
      /*.kernel_times_nsec_end =*/ NULL,

      /*.event_groups =*/ NULL,

      /*.event_names =*/ NULL,

      /*.stage_time_nsec_start =*/ 0,
      /*.stage_time_nsec_end =*/ 0,

      /*.cuda_context =*/ NULL,
      /*.cuda_device =*/ CU_DEVICE_INVALID,

      /*.subscriber =*/ NULL,
  
      /*.num_event_groups =*/ 0,
      /*.num_kernel_times =*/ 0,

      /*.count_event_groups_read =*/ 0,
  
      /*.event_counter_buffer_length =*/ 0,
      /*.event_id_buffer_length =*/ 0,
      /*.kernel_times_nsec_buffer_length =*/ 10, // default; will increase as necessary at runtime

      /*.event_names_buffer_length =*/ 0,

      /*.initialized =*/ false
  };
  
}

//--------------------------------------
// Device globals
//-------------------------------------

namespace detail {
  extern DEV clock64_t* dev_tstart = nullptr;
  extern DEV clock64_t* dev_ttime = nullptr;
  extern DEV int* dev_num_iter = nullptr;
  extern DEV uint* dev_smids = nullptr;
}

// --------------------------------------------------------------------------

//--------------------------------------
// Host-side device memory management
//-------------------------------------

static size_t dev_tbuf_size = 0;
static size_t dev_num_iter_size = 0;
static size_t dev_smids_size = 0;

static void* d_dev_tstart = nullptr;
static void* d_dev_ttime = nullptr;
static void* d_dev_num_iter = nullptr;
static void* d_dev_smids = nullptr;

static volatile bool test_imbalance_detect = true;

template <class T>
static void cuda_safe_free(T*& ptr) {
  if (ptr != nullptr) {
    CUDA_RUNTIME_FN(cudaFree(static_cast<void*>(ptr)));
    ptr = nullptr;
  }
}

template <class T>
static void cuda_memcpy_host_to_dev(void* dst, std::vector<T> host) {
  size_t size = host.size() * sizeof(T);

  CUDA_RUNTIME_FN(cudaMemcpy(static_cast<void*>(dst),
                             static_cast<void*>(host.data()),
                             size,
                             cudaMemcpyHostToDevice));

}

template <class T>
NVCD_CUDA_EXPORT void* __cuda_zalloc_sym(size_t size, const T& sym, const char* ssym) {
  void* address_of_sym = nullptr;
  CUDA_RUNTIME_FN(cudaGetSymbolAddress(&address_of_sym, sym));
  ASSERT(address_of_sym != nullptr);

  CUDA_RUNTIME_FN(cudaMemset(address_of_sym, 0xFEED, sizeof(address_of_sym)));
  
  void* device_allocated_mem = nullptr;

  CUDA_RUNTIME_FN(cudaMalloc(&device_allocated_mem, size));
  ASSERT(device_allocated_mem != nullptr);

  CUDA_RUNTIME_FN(cudaMemset(device_allocated_mem, 0, size));

  CUDA_RUNTIME_FN(cudaMemcpy(address_of_sym,
                             &device_allocated_mem,
                             sizeof(device_allocated_mem),
                             cudaMemcpyHostToDevice));
  
  ASSERT(address_of_sym != nullptr);

  // sanity check
  {
    void* check = nullptr;
    
    CUDA_RUNTIME_FN(cudaMemcpy(&check,
                               address_of_sym,
                               sizeof(check),
                               cudaMemcpyDeviceToHost));

    printf("HOST-SIDE DEVICE ADDRESS FOR %s: %p. value: %p\n",
           ssym,
           address_of_sym,
           check);
    
    ASSERT(check == device_allocated_mem);
  }
  return device_allocated_mem;
}

#define cuda_zalloc_sym(sz, sym) __cuda_zalloc_sym(sz, sym, #sym)


//-------------------------------------
// NVCD Host-side Device
//-------------------------------------


extern "C" {


  
NVCD_CUDA_EXPORT void nvcd_device_free_mem() {
  cuda_safe_free(d_dev_tstart);
  cuda_safe_free(d_dev_ttime);
  cuda_safe_free(d_dev_num_iter);
  cuda_safe_free(d_dev_smids);
}

NVCD_CUDA_EXPORT void nvcd_device_init_mem(int num_threads) {
  {       
    dev_tbuf_size = sizeof(clock64_t) * static_cast<size_t>(num_threads);

    d_dev_tstart = cuda_zalloc_sym(dev_tbuf_size, detail::dev_tstart);
    d_dev_ttime = cuda_zalloc_sym(dev_tbuf_size, detail::dev_ttime);
  }

  {
    dev_smids_size = sizeof(uint) * static_cast<size_t>(num_threads);

    d_dev_smids = cuda_zalloc_sym(dev_smids_size, detail::dev_smids);
  }

  if (test_imbalance_detect) {
    dev_num_iter_size = sizeof(int) * static_cast<size_t>(num_threads);

    d_dev_num_iter = cuda_zalloc_sym(dev_num_iter_size, detail::dev_num_iter);

    std::vector<int> host_num_iter(num_threads, 0);

    int iter_min = 100;
    int iter_max = iter_min * 100;

    for (size_t i = 0; i < host_num_iter.size(); ++i) {
      srand(time(nullptr));

      if (i > host_num_iter.size() - 100) {
        iter_min = 1000;
        iter_max = iter_min * 100;
      }

      host_num_iter[i] = iter_min + (rand() % (iter_max - iter_min));
    }

    cuda_memcpy_host_to_dev<int>(d_dev_num_iter, std::move(host_num_iter));
  }
}

NVCD_CUDA_EXPORT void nvcd_device_get_ttime(clock64_t* out) {
  CUDA_RUNTIME_FN(cudaMemcpy(out,
                             d_dev_ttime,
                             dev_tbuf_size,
                             cudaMemcpyDeviceToHost));
}

NVCD_CUDA_EXPORT void nvcd_device_get_smids(unsigned* out) {
  CUDA_RUNTIME_FN(cudaMemcpy(out,
                             d_dev_smids,
                             dev_smids_size,
                             cudaMemcpyDeviceToHost));
}

}

//-------------------------------------
// NVCD Device
//-------------------------------------

NVCD_DEV_EXPORT uint get_smid() {
  uint ret;
  asm("mov.u32 %0, %smid;" : "=r"(ret) );
  return ret;
}

NVCD_DEV_EXPORT void nvcd_device_begin(int thread) {
  DEV_PRINT_PTR(detail::dev_tstart);
  detail::dev_tstart[thread] = clock64();
}

NVCD_DEV_EXPORT void nvcd_device_end(int thread) {
  detail::dev_ttime[thread] = clock64() - detail::dev_tstart[thread];
  detail::dev_smids[thread] = get_smid();

  DEV_PRINT_PTR(detail::dev_ttime);
  DEV_PRINT_PTR(detail::dev_smids);
}


//------------------------------------------------------
// Kernel thread data
//------------------------------------------------------

extern "C" {

  typedef struct kernel_thread_data {
    clock64_t* times;

    uint32_t* smids;
  
    size_t num_cuda_threads;

    bool32_t initialized;
  
  } kernel_thread_data_t;

  NVCD_CUDA_EXPORT void kernel_thread_data_init(kernel_thread_data_t* k, int num_cuda_threads) {
    ASSERT(k != NULL);

    if (!k->initialized) {
      k->num_cuda_threads = (size_t)num_cuda_threads;

      k->times = (clock64_t*) zallocNN(sizeof(k->times[0]) *
                                       k->num_cuda_threads);
      
      k->smids = (uint32_t*) zallocNN(sizeof(k->smids[0]) *
                                       k->num_cuda_threads);

      k->initialized = true;
    }
  }

  NVCD_CUDA_EXPORT void kernel_thread_data_free(kernel_thread_data_t* k) {
    ASSERT(k != NULL);

    safe_free_v(k->times);
    safe_free_v(k->smids);

    k->initialized = false;
  }

  NVCD_CUDA_EXPORT void kernel_thread_data_report(kernel_thread_data_t* k) {
    ASSERT(k != NULL);
  
    puts("============PER-THREAD KERNEL DATA============");

    for (size_t i = 0; i < k->num_cuda_threads; ++i) {
      printf("---\nThread %" PRIu64 ":\n"
             "\tTime\t= %" PRId64 " nanoseconds\n"
             "\tSM ID\t= %" PRIu32 "\n",
             i,
             k->times[i],
             k->smids[i]);
    }
    puts("==============================================");
  }

  static kernel_thread_data_t g_kernel_thread_data = {
    /*.times =*/ NULL,
    /*.smids =*/ NULL,
  
    /*.num_cuda_threads =*/ 0,

    /*.initialized =*/ false
  };
}

//------------------------------------------------------
// NVCD host
//------------------------------------------------------

extern "C" {

NVCD_CUDA_EXPORT void cleanup() {
  cupti_event_data_free(&g_event_data);
  
  cupti_name_map_free();

  for (int i = 0; i < g_nvcd.num_devices; ++i) {
    ASSERT(g_nvcd.contexts[i] != NULL);
    CUDA_DRIVER_FN(cuCtxDestroy(g_nvcd.contexts[i]));
  }
}

NVCD_CUDA_EXPORT void nvcd_report() {
  ASSERT(g_event_data.initialized == true);
  
  cupti_report_event_data(&g_event_data);
}

NVCD_CUDA_EXPORT void nvcd_init() {
  nvcd_init_cuda(&g_nvcd);
}

NVCD_CUDA_EXPORT void nvcd_host_begin(int num_cuda_threads) {  
  ASSERT(g_nvcd.initialized == true);

  nvcd_device_init_mem(num_cuda_threads);

  kernel_thread_data_init(&g_kernel_thread_data, num_cuda_threads);
  
  g_event_data.cuda_context = g_nvcd.contexts[0];
  g_event_data.cuda_device = g_nvcd.devices[0];
  g_event_data.thread_host_begin = pthread_self();
  
  cupti_event_data_init(&g_event_data);
  cupti_event_data_begin(&g_event_data);
}

NVCD_CUDA_EXPORT bool nvcd_host_finished() {  
  ASSERT(g_event_data.count_event_groups_read
         <= g_event_data.num_event_groups /* serious problem if this fails */);
  
  return g_event_data.count_event_groups_read
    == g_event_data.num_event_groups; 
}

NVCD_CUDA_EXPORT void nvcd_host_end() {
  ASSERT(g_kernel_thread_data.initialized == true);
  ASSERT(g_nvcd.initialized == true);
  
  nvcd_device_get_ttime(g_kernel_thread_data.times);
  nvcd_device_get_smids(g_kernel_thread_data.smids);

  kernel_thread_data_report(&g_kernel_thread_data);
  kernel_thread_data_free(&g_kernel_thread_data);
  
  cupti_event_data_end(&g_event_data);
  nvcd_device_free_mem();
}

NVCD_CUDA_EXPORT void nvcd_terminate() {
  cleanup();
}

}

//------------------------------------------------------
// Dead code
//------------------------------------------------------


#if 0
EXTC NVCD_EXPORT GLOBAL void nvcd_kernel_test() {
  int thread = blockIdx.x * blockDim.x + threadIdx.x;

  int num_threads = blockDim.x * gridDim.x;

  if (thread == 0) {
    
  }

  if (thread < num_threads) {
    nvcd_device_begin(thread);

    volatile int number = 0;

    for (int i = 0; i < detail::dev_num_iter[thread]; ++i) {
      number += i;
    }

    nvcd_device_end(thread);
  }
}

NVCD_CUDA_EXPORT void nvcd_kernel_test_call(int num_threads) {
  int nblock = 4;
  int threads = num_threads / nblock;
  nvcd_kernel_test<<<nblock, threads>>>();
}
#endif

//-----------------------------------------------------------------------
#if 0
NVCD_CUDA_EXPORT void nvcd_device_init_mem(int num_threads);
NVCD_CUDA_EXPORT void nvcd_device_free_mem();

NVCD_CUDA_EXPORT void nvcd_device_get_ttime(clock64_t* out);
NVCD_CUDA_EXPORT void nvcd_device_get_smids(unsigned* out);

EXTC NVCD_EXPORT GLOBAL void nvcd_kernel_test();
NVCD_CUDA_EXPORT void nvcd_kernel_test_call(int num_threads);
#endif

#endif // __DEVICE_CUH__

