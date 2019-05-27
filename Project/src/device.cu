#include "device.cuh"

#include "util.h"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <unordered_map>

//--------------------------------------
// internal
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
static void* __cuda_zalloc_sym(size_t size, const T& sym, const char* ssym) {
  void* address_of_sym = nullptr;
  CUDA_RUNTIME_FN(cudaGetSymbolAddress(&address_of_sym, sym));

  void* device_allocated_mem = nullptr;

  CUDA_RUNTIME_FN(cudaMalloc(&device_allocated_mem, size));
  CUDA_RUNTIME_FN(cudaMemset(device_allocated_mem, 0, size));

  CUDA_RUNTIME_FN(cudaMemcpy(address_of_sym,
                             &device_allocated_mem,
                             sizeof(device_allocated_mem),
                             cudaMemcpyHostToDevice));

  return device_allocated_mem;
}

#define cuda_zalloc_sym(sz, sym) __cuda_zalloc_sym(sz, sym, #sym)

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


//-------------------------------------
// public
//-------------------------------------

EXTC NVCD_EXPORT HOST void nvcd_device_free_mem() {
  cuda_safe_free(d_dev_tstart);
  cuda_safe_free(d_dev_ttime);
  cuda_safe_free(d_dev_num_iter);
  cuda_safe_free(d_dev_smids);
}

EXTC NVCD_EXPORT HOST void nvcd_device_init_mem(int num_threads) {
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

EXTC NVCD_EXPORT HOST void nvcd_device_get_ttime(clock64_t* out) {
  CUDA_RUNTIME_FN(cudaMemcpy(out,
                             d_dev_ttime,
                             dev_tbuf_size,
                             cudaMemcpyDeviceToHost));
}

EXTC NVCD_EXPORT HOST void nvcd_device_get_smids(unsigned* out) {
  CUDA_RUNTIME_FN(cudaMemcpy(out,
                             d_dev_smids,
                             dev_smids_size,
                             cudaMemcpyDeviceToHost));

}


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

EXTC NVCD_EXPORT HOST void nvcd_kernel_test_call(int num_threads) {
  int nblock = 4;
  int threads = num_threads / nblock;
  nvcd_kernel_test<<<nblock, threads>>>();
}
