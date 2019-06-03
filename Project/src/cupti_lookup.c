#include "nvcd/cupti_lookup.h"
#include <string.h>
#include <inttypes.h>
#include "nvcd/util.h"
#include "nvcd/list.h"
#include "nvcd/env_var.h"

/*
 * List of event strings as listed in the CUPTI event documentation
 * for CUDA toolkit v9.2
 */

char* g_cupti_event_names_2x[] = {
  // events from 1.x

  "tex_cache_hit",
  "tex_cache_miss",
  
  "branch",
  "divergent_branch",
  "instructions",
  "warp_serialize",
  
  "gld_incoherent",
  "gld_coherent",
  "gld_32b",
  "gld_64b",
  "gld_128b",

  "gst_incoherent",
  "gst_coherent",
  "gst_32b",
  "gst_64b",
  "gst_128b",

  "local_load",
  "local_store",

  "cta_launched",
  "sm_cta_launched",

  "prof_trigger_00",
  "prof_trigger_01",
  "prof_trigger_02",
  "prof_trigger_03",
  "prof_trigger_04",
  "prof_trigger_05",
  "prof_trigger_06",
  "prof_trigger_07",
  
  // events from 2.x
  /* 
   * domain_a 
   */

  "sm_cta_launched",
  
  // 6
  "l1_local_load_hit",
  "l1_local_load_miss",
  "l1_local_store_hit",
  "l1_local_store_miss",
  "l1_global_load_hit",
  "l1_global_load_miss",

  // 1
  "uncached_global_load_transaction",

  // 1
  "global_store_transaction",

  // 1
  "l1_shared_bank_conflict",

  // 2
  "tex0_cache_sector_queries",
  "tex0_cache_sector_misses",

  // 2
  "tex1_cache_sector_queries",
  "tex1_cache_sector_misses",

  /* 
   * domain_b
   */
  
  // 14
  "l2_subp0_write_sector_misses",
  "l2_subp1_write_sector_misses",
  "l2_subp0_read_sector_misses",
  "l2_subp1_read_sector_misses",
  "l2_subp0_write_sector_queries",
  "l2_subp1_write_sector_queries",
  "l2_subp0_read_sector_queries",
  "l2_subp1_read_sector_queries",
  "l2_subp0_read_hit_sectors",
  "l2_subp1_read_hit_sectors",
  "l2_subp0_read_tex_sector_queries",
  "l2_subp1_read_tex_sector_queries",
  "l2_subp0_read_tex_hit_sectors",
  "l2_subp1_read_tex_hit_sectors",

  // 4
  "fb_subp0_read_sectors",
  "fb_subp1_read_sectors",
  "fb_subp0_write_sectors",
  "fb_subp1_write_sectors",

  // 4
  "fb0_subp0_read_sectors",
  "fb0_subp1_read_sectors",
  "fb0_subp0_write_sectors",
  "fb0_subp1_write_sectors",

  // 4
  "fb1_subp0_read_sectors",
  "fb1_subp1_read_sectors",
  "fb1_subp0_write_sectors",
  "fb1_subp1_write_sectors",

  /*
   * domain_c
   */

  "gld_inst_8bit",
  "gld_inst_16bit",
  "gld_inst_32bit",
  "gld_inst_64bit",
  "gld_inst_128bit",

  "gst_inst_8bit",
  "gst_inst_16bit",
  "gst_inst_32bit",
  "gst_inst_64bit",
  "gst_inst_128bit",

  /*
   * domain_d
   */

  "branch",
  "divergent_branch",
  "warps_launched",
  "threads_launched",
  "active_warps",
  "active_cycles",

  "local_load",
  "local_store",
  "gld_request",
  "gst_request",
  "shared_load",
  "shared_store",
  "prof_trigger_XX",

  "inst_issued",
  "inst_issued1_0",
  "inst_issued2_0",
  "inst_issued1_1",
  "inst_issued2_1",
  "inst_executed",
  
  "thread_inst_executed_0",
  "thread_inst_executed_1"
};

const uint32_t g_cupti_event_names_2x_length = ARRAY_LENGTH(g_cupti_event_names_2x);

NVCD_EXPORT char** cupti_get_event_names() {
  return g_cupti_event_names_2x;
}

NVCD_EXPORT uint32_t cupti_get_num_event_names() {
  return g_cupti_event_names_2x_length;
}

/*
 * List of event metrics as listed in the CUPTI event documentation
 * for CUDA toolkit v9.2
 */

const char* g_cupti_metrics_3x[NUM_CUPTI_METRICS_3X] = {

  // 6
  "achieved_occupancy",
  "alu_fu_utilization",
  "atomic_replay_overhead",
  "atomic_throughput",
  "atomic_transactions",
  "atomic_transactions_per_request",

  // 1
  "branch_efficiency",

  // 3
  "cf_executed",
  "cf_fu_utilization",
  "cf_issued",

  // 5
  "dram_read_throughput", 
  "dram_read_transactions",
  "dram_utilization",   
  "dram_write_throughput",
  "dram_write_transactions",

  // 3
  "ecc_throughput",
  "ecc_transactions",
  "eligible_warps_per_cycle",

  // 11
  "flop_count_dp", 
  "flop_count_dp_add",
  "flop_count_dp_fma",
  "flop_count_dp_mul",
  "flop_count_sp", 
  "flop_count_sp_add",
  "flop_count_sp_fma",
  "flop_count_sp_mul",
  "flop_count_sp_special",
  "flop_dp_efficiency",
  "flop_sp_efficiency",

  // 12
  "gld_efficiency",
  "gld_requested_throughput",
  "gld_throughput",
  "gld_transactions",
  "gld_transactions_per_request",
  "global_cache_replay_overhead",
  "global_replay_overhead",
  "gst_efficiency",
  "gst_requested_throughput",
  "gst_throughput",
  "gst_transactions",
  "gst_transactions_per_request",

  // 17
  "inst_bit_convert",
  "inst_compute_ld_st",
  "inst_control",
  "inst_executed",
  "inst_fp_32",
  "inst_fp_64",
  "inst_integer",
  "inst_inter_thread_communication",
  "inst_issued",
  "inst_misc",
  "inst_per_warp",
  "inst_replay_overhead",
  "ipc",
  "ipc_instance",
  "issue_slot_utilization",
  "issue_slots",
  "issued_ipc",

  // 29
  "l1_cache_global_hit_rate",
  "l1_cache_local_hit_rate",
  "l1_shared_utilization",
  "l2_atomic_throughput",
  "l2_atomic_transactions",
  "l2_l1_read_hit_rate",
  "l2_l1_read_throughput",
  "l2_l1_read_transactions",
  "l2_l1_write_throughput",
  "l2_l1_write_transactions",
  "l2_read_throughput",
  "l2_read_transactions",
  "l2_tex_read_transactions",
  "l2_tex_read_hit_rate",
  "l2_tex_read_throughput",
  "l2_utilization",   
  "l2_write_throughput",
  "l2_write_transactions",
  "ldst_executed",
  "ldst_fu_utilization",
  "ldst_issued",
  "local_load_throughput",
  "local_load_transactions",
  "local_load_transactions_per_request",
  "local_memory_overhead",
  "local_replay_overhead",
  "local_store_throughput",
  "local_store_transactions",
  "local_store_transactions_per_request",

  // 6
  "nc_cache_global_hit_rate",
  "nc_gld_efficiency",
  "nc_gld_requested_throughput",
  "nc_gld_throughput",
  "nc_l2_read_throughput",
  "nc_l2_read_transactions",

  // 27 
  "shared_efficiency",
  "shared_load_throughput",
  "shared_load_transactions",
  "shared_load_transactions_per_request",
  "shared_replay_overhead",
  "shared_store_throughput",
  "shared_store_transactions",
  "shared_store_transactions_per_request",
  "sm_efficiency",
  "sm_efficiency_instance",
  "stall_constant_memory_dependency",
  "stall_exec_dependency",
  "stall_inst_fetch",
  "stall_memory_dependency",
  "stall_memory_throttle",
  "stall_not_selected",
  "stall_other",
  "stall_pipe_busy",
  "stall_sync",
  "stall_texture",
  "sysmem_read_throughput",
  "sysmem_read_transactions",
  "sysmem_read_utilization",
  "sysmem_utilization",
  "sysmem_write_throughput",
  "sysmem_write_transactions",
  "sysmem_write_utilization",

  // 5
  "tex_cache_hit_rate",
  "tex_cache_throughput",
  "tex_cache_transactions",
  "tex_fu_utilization",
  "tex_utilization",

  // 2
  "warp_execution_efficiency",
  "warp_nonpred_execution_efficiency"
};

typedef struct cupti_name_map {
  list_t self;
  char* name;
  CUpti_EventID id;
} cupti_name_map_t;

static cupti_name_map_t* g_name_map_list = NULL;

static CUpti_runtime_api_trace_cbid g_cupti_runtime_cbids[] = {
  CUPTI_RUNTIME_TRACE_CBID_cudaLaunch_v3020,
  CUPTI_RUNTIME_TRACE_CBID_cudaLaunchKernel_v7000
};

#define NUM_CUPTI_RUNTIME_CBIDS (sizeof(g_cupti_runtime_cbids) / sizeof(g_cupti_runtime_cbids[0]))

static void init_cupti_event_buffers(cupti_event_data_t* e);

static void cupti_name_map_push(cupti_name_map_t* node) {
  list_push_fn_impl(&g_name_map_list,
                    node,
                    cupti_name_map_t,
                    self);
}

static void cupti_name_map_free_node(cupti_name_map_t* n) {
  free(n->name);
  n->name = NULL;
}

static void fill_event_groups(cupti_event_data_t* e,
                              CUpti_EventGroup* local_eg_assign,
                              uint32_t num_egs) {
  e->num_event_groups = num_egs;
  e->event_groups = zallocNN(sizeof(e->event_groups[0]) * e->num_event_groups);
  e->event_groups_read = zallocNN(sizeof(e->event_groups_read[0]) * e->num_event_groups);
    
  for (uint32_t i = 0; i < e->num_event_groups; ++i) {
    ASSERT(local_eg_assign[i] != NULL);
      
    e->event_groups[i] = local_eg_assign[i];

    // enabling this (on the GTX 980M, at least, will cause a SIGBUS error for event groups
    // that are tied to events which are used by metrics)
    #if 0
    uint32_t profile_all = 1;
    CUPTI_FN(cuptiEventGroupSetAttribute(e->event_groups[i],
                                         CUPTI_EVENT_GROUP_ATTR_PROFILE_ALL_DOMAIN_INSTANCES,
                                         sizeof(profile_all), &profile_all));
    #endif
  }
}

static bool find_event_group(cupti_event_data_t* e,
                             CUpti_EventGroup* local_eg_assign,
                             CUpti_EventID event_id,
                             uint32_t max_egs,
                             uint32_t* num_egs) {
  
  uint32_t j = 0;
  CUptiResult err = CUPTI_ERROR_NOT_COMPATIBLE;

  //
  // find a suitable group
  // for this event
  //
  bool iterating = j < max_egs;
  bool error_valid = false;
  bool found = false;

  while (iterating) {
    if (local_eg_assign[j] == NULL) {
      CUPTI_FN(cuptiEventGroupCreate(e->cuda_context,
                                     &local_eg_assign[j],
                                     0));
      *num_egs = *num_egs + 1;
    }

    err = cuptiEventGroupAddEvent(local_eg_assign[j],
                                  event_id);
        
    j++;

    // event groups cannot have
    // events from different domains;
    // in these cases we just find another group.
    error_valid =
      !(err == CUPTI_ERROR_MAX_LIMIT_REACHED
        || err == CUPTI_ERROR_NOT_COMPATIBLE);

    if (error_valid) {
      error_valid = err == CUPTI_SUCCESS;
      CUPTI_FN(err);
    }
        
    if (j == max_egs || error_valid) {
      iterating = false;
      found = err == CUPTI_SUCCESS;
    }
  }

  ASSERT(j <= max_egs);
      
  // trigger exit if we still error out:
  // something not taken into account
  // needs to be looked at
  CUPTI_FN(err);

  return found;
}

static void init_cupti_metric_data(cupti_event_data_t* e) {
  cupti_metric_data_t* metric_buffer = zallocNN(sizeof(*metric_buffer));
  

  CUPTI_FN(cuptiDeviceGetNumMetrics(e->cuda_device,
                                    &metric_buffer->num_metrics));

  size_t metric_array_size = sizeof(CUpti_MetricID) * metric_buffer->num_metrics;

  metric_buffer->metric_ids = zallocNN(metric_array_size);

  CUPTI_FN(cuptiDeviceEnumMetrics(e->cuda_device,
                                  &metric_array_size,
                                  &metric_buffer->metric_ids[0]));

  metric_buffer->metric_values = zallocNN(sizeof(metric_buffer->metric_values[0]) *
                                          metric_buffer->num_metrics);
  
  metric_buffer->event_data = zallocNN(sizeof(metric_buffer->event_data[0]) *
                                       metric_buffer->num_metrics);

  metric_buffer->computed = zallocNN(sizeof(metric_buffer->computed[0]) *
                                     metric_buffer->num_metrics);

  metric_buffer->metric_get_value_results =
    zallocNN(sizeof(metric_buffer->metric_get_value_results[0]) *
             metric_buffer->num_metrics);
                                                     
  
#define _index_ "[%" PRIu32 "] "
  
  for (uint32_t i = 0; i < metric_buffer->num_metrics; ++i) {
    char* name = cupti_metric_get_name(metric_buffer->metric_ids[i]);
    printf( _index_ "Processing metric %s...\n", i, name);
    
    uint32_t num_events = 0;
    CUPTI_FN(cuptiMetricGetNumEvents(metric_buffer->metric_ids[i], &num_events));
    printf(_index_ "event count is %" PRIu32 ":\n", i, num_events);

    size_t event_array_size = sizeof(CUpti_EventID) * num_events;
    
    CUpti_EventID* events = zallocNN(event_array_size);
    
    CUPTI_FN(cuptiMetricEnumEvents(metric_buffer->metric_ids[i],
                                   &event_array_size,
                                   &events[0]));
    
    cupti_event_data_set_null(&metric_buffer->event_data[i]);

    metric_buffer->event_data[i].cuda_device = e->cuda_device;
    metric_buffer->event_data[i].cuda_context = e->cuda_context;
    
    cupti_event_data_init_from_ids(&metric_buffer->event_data[i],
                                   &events[0],
                                   num_events);
    
    // TODO: take each group and compute the needed values...
    
    free(name);

    puts("---");
  }

  metric_buffer->initialized = true;

  e->metric_data = metric_buffer;
}


static uint32_t derive_event_count(cupti_event_data_t* e) {
  uint32_t ret = 0;

  for (uint32_t i = 0; i < e->num_event_groups; ++i) {
    ret += e->num_events_per_group[i];
  }

  return ret;
}

//
// see https://docs.nvidia.com/cuda/archive/9.2/cupti/group__CUPTI__METRIC__API.html#group__CUPTI__METRIC__API_1gf42dcb1d349f91265e7809bbba2fc01e
// for more information on why this is needed.
//
static void normalize_counters(cupti_event_data_t* e, uint64_t* normalized) {
  for (uint32_t j = 0; j < e->num_event_groups; ++j) {
    CUpti_EventDomainID domain_id;
    size_t domain_id_sz = sizeof(domain_id);
    CUPTI_FN(cuptiEventGroupGetAttribute(e->event_groups[j],
                                         CUPTI_EVENT_GROUP_ATTR_EVENT_DOMAIN_ID,
                                         &domain_id_sz,
                                         (void*) &domain_id));
    
    uint32_t total_instance_count = 1;

    // Querying for this value will throw
    // an error that isn't consistent with what's reported
    // in the documentation. Currently, only speculation
    // as to "why" is possible - for now, we're forgoing
    // anything involving multiple domain instances
    // since it appears to cause problems with specific configurations
    // (at least, for the metrics - event counters that are recorded without
    // interfacing with teh metrics appear to be fine)
    #if 0    
    size_t total_instance_count_sz = sizeof(total_instance_count);
    CUPTI_FN(cuptiEventDomainGetAttribute(domain_id,
                                          CUPTI_EVENT_DOMAIN_ATTR_TOTAL_INSTANCE_COUNT,
                                          &total_instance_count_sz,
                                          (void*) &total_instance_count));
    #endif
    
    uint32_t cb_offset = e->event_counter_buffer_offsets[j];
    uint32_t ib_offset = e->event_id_buffer_offsets[j];
    uint32_t nepg = e->num_events_per_group[j];
    uint32_t nipg = e->num_instances_per_group[j];
      
    for (uint32_t row = 0; row < nipg; ++row) {
      for (uint32_t col = 0; col < nepg; ++col) {
        normalized[ib_offset + col] +=
          e->event_counter_buffer[cb_offset + row * nepg + col];
      }
    }

    for (uint32_t k = 0; k < nepg; ++k) {
      normalized[ib_offset + k] *= total_instance_count;
      normalized[ib_offset + k] /= nipg;
    }
  }
}

static void print_cupti_metric(cupti_metric_data_t* metric_data, uint32_t index) {
  //ASSERT(metric_data->computed[index] == true);

  CUpti_MetricID m = metric_data->metric_ids[index];
  CUpti_MetricValue v = metric_data->metric_values[index];

  char* name = cupti_metric_get_name(m);
    
  printf("[%" PRIu32 "] %s = ", index, name);
  
  if (metric_data->computed[index] == true) {
  
    CUpti_MetricValueKind kind;
  
    size_t kind_sz = sizeof(kind);
  
    CUPTI_FN(cuptiMetricGetAttribute(m,
                                     CUPTI_METRIC_ATTR_VALUE_KIND,
                                     &kind_sz,
                                     (void*) &kind));
  
    switch (kind) {
    case CUPTI_METRIC_VALUE_KIND_DOUBLE: {
      printf("(double) %f", v.metricValueDouble);
    } break;
    
    case CUPTI_METRIC_VALUE_KIND_UINT64: {
      printf("(uint64) %" PRIu64, v.metricValueUint64);
    } break;
    
    case CUPTI_METRIC_VALUE_KIND_INT64: {
      printf("(int64) %" PRId64, v.metricValueInt64);
    } break;
    
    case CUPTI_METRIC_VALUE_KIND_PERCENT: {
      printf("(percent) %f", v.metricValuePercent);
    } break;
    
    case CUPTI_METRIC_VALUE_KIND_THROUGHPUT: {
      printf("(bytes/second) %" PRId64, v.metricValueThroughput);
    } break;
    
    case CUPTI_METRIC_VALUE_KIND_UTILIZATION_LEVEL: {
      const char* level = NULL;
    
      switch (v.metricValueUtilizationLevel) {
      case CUPTI_METRIC_VALUE_UTILIZATION_IDLE: { 
        level = "IDLE";
      } break;
      
      case CUPTI_METRIC_VALUE_UTILIZATION_LOW: { 
        level = "LOW";
      } break;

      case CUPTI_METRIC_VALUE_UTILIZATION_MID: { 
        level = "MID";
      } break;

      case CUPTI_METRIC_VALUE_UTILIZATION_HIGH: { 
        level = "HIGH";
      } break;

      case CUPTI_METRIC_VALUE_UTILIZATION_MAX: { 
        level = "MAX";
      } break;
      
      default:
        //        ASSERT(false /* bad utilization level received */);

        level = "UNKNOWN (bad value received)";
        break;
      }
    
      printf("(utilization level) %" PRIu32 " =  %s ",
             v.metricValueUtilizationLevel,
             level);
    } break;

    default:
      //ASSERT(false /* bad metric value kind received */);
      printf("[WARNING: bad metric value kind received: 0x%" PRIx32 "] ", kind);
      break;
    }
  } else {
    const char* result_string = NULL;

    // TODO: docs do not specify whether or not this string should be freed,
    // so the assumption is that it shouldn't (for now - should test with valgrind
    // when time allows)
    //
    // reference (see function listing given below the enum values):
    //
    // https://docs.nvidia.com/cuda/archive/9.2/cupti/group__CUPTI__RESULT__API.html#group__CUPTI__RESULT__API_1g8c54bf95108e67d858f37fcf76c88714
    //
    //
    CUPTI_FN(cuptiGetResultString(metric_data->metric_get_value_results[index],
                                 &result_string));
    
    printf("[NOT computed - Error code received: %" PRIu32 " = %s]",
           metric_data->metric_get_value_results[index],
           result_string);
  }
  
  printf("%s", "\n");

  free(name);
}

static void calc_cupti_metrics(cupti_metric_data_t* m) {
  ASSERT(m->initialized == true);
  ASSERT(m->num_metrics < 2000);
  
  for (uint32_t i = 0; i < m->num_metrics; ++i) {
    cupti_event_data_t* e = &m->event_data[i];
    
    ASSERT(e->event_id_buffer_length ==
           derive_event_count(&m->event_data[i]));

    ASSERT(e->num_kernel_times > 0 && e->num_kernel_times < 100);
    
    uint64_t* normalized = zallocNN(e->event_id_buffer_length *
                                    sizeof(normalized[0]));
    
    normalize_counters(e, normalized);
    
    CUptiResult err = cuptiMetricGetValue(e->cuda_device,
                                 m->metric_ids[i],
                                 sizeof(e->event_id_buffer[0]) * e->event_id_buffer_length,
                                 &e->event_id_buffer[0],
                                 sizeof(normalized[0]) * e->event_id_buffer_length,
                                 &normalized[0],
                                 e->kernel_times_nsec[0],
                                 &m->metric_values[i]);

    ASSERT(m->computed[i] == false);

    if (err == CUPTI_SUCCESS) {
      m->computed[i] = true;
    } else if (err != CUPTI_ERROR_INVALID_METRIC_VALUE) {
      CUPTI_FN(err);
    }

    m->metric_get_value_results[i] = err;
  }
}


typedef struct group_info {
  uint32_t num_events;
  uint32_t num_instances;
  CUpti_EventID* events;
  uint64_t* counters;
  CUpti_EventGroup group;
} group_info_t; 

static volatile bool g_process_group_aos = false;

static group_info_t* g_group_info_buffer = NULL;
static uint32_t g_group_info_count = 0;
static uint32_t g_group_info_size = 0;

static void group_info_append(group_info_t* info, uint32_t group) {
  if (g_group_info_buffer == NULL) {
    g_group_info_size = 50;
    g_group_info_buffer = zallocNN(g_group_info_size * sizeof(*g_group_info_buffer));
  }

  ASSERT(group < g_group_info_size /* TODO: use double_buffer_size() in util.h */); 
  
  memcpy(&g_group_info_buffer[group], info, sizeof(*info));
  g_group_info_count++;
}

static void group_info_free(group_info_t* info) {
  safe_free_v(info->events);
  safe_free_v(info->counters);
}

static void group_info_validate(cupti_event_data_t* e,
                                group_info_t* info,
                                uint32_t group) {
  printf("Validating group %" PRIu32 "\n", group);

  ASSERT(info->group == e->event_groups[group]);
  ASSERT(info->num_events == e->num_events_per_group[group]);
  ASSERT(info->num_instances == e->num_instances_per_group[group]);

  volatile CUpti_EventID* base =
    &e->event_id_buffer[e->event_id_buffer_offsets[group]];
  
  for (uint32_t i = 0; i < info->num_events; ++i) {
    ASSERT(base[i] == info->events[i]);

    char* name = cupti_event_get_name(info->events[i]);
    char* name2 = cupti_event_get_name(base[i]);

    ASSERT(strcmp(name, name2) == 0);

#if 0
    uint64_t* soa_counters = &e->event_counter_buffer[e->event_counter_buffer_offsets[group]];
    
    // counter comparison
    for (uint32_t j = 0; j < info->num_instances; ++j) {
      ASSERT(soa_counters[i * info->num_instances + j] ==
             info->counters[i * info->num_instances + j]);
    }
#endif
    
    printf("\t[%" PRIu32 "] %s|%s is good...\n", i, name, name2);
    free(name);
    free(name2);
  }

  ASSERT(info->num_instances == 1);
}

static void group_info_make(cupti_event_data_t* e,
                        group_info_t* info,
                        uint32_t group) {

  CUpti_EventGroup g = e->event_groups[group];
  
  {
    size_t sz = sizeof(info->num_events);

    CUPTI_FN(cuptiEventGroupGetAttribute(g,
                                         CUPTI_EVENT_GROUP_ATTR_NUM_EVENTS,
                                         &sz,
                                         &info->num_events));
  }

  {
    size_t sz = sizeof(info->num_instances);

    CUPTI_FN(cuptiEventGroupGetAttribute(g,
                                         CUPTI_EVENT_GROUP_ATTR_INSTANCE_COUNT,
                                         &sz,
                                         &info->num_instances));
  }

  {
    size_t sz = sizeof(info->events[0]) * info->num_events;

    info->events = zallocNN(sz);
    
    CUPTI_FN(cuptiEventGroupGetAttribute(g,
                                         CUPTI_EVENT_GROUP_ATTR_EVENTS,
                                         &sz,
                                         &info->events[0]));
  }
  
  {
    size_t sz = sizeof(info->counters) * info->num_instances * info->num_events;
    info->counters = zallocNN(sz);
  }

  info->group = g;
}

static group_info_t* group_info_new(cupti_event_data_t* e, uint32_t group) {
  group_info_t* ret = zallocNN(sizeof(*ret));
  group_info_make(e, ret, group);
  return ret;
}

static void read_group_all_events(cupti_event_data_t* e, uint32_t group) {
  size_t cb_size =
    e->num_events_per_group[group] *
    e->num_instances_per_group[group] *
    sizeof(uint64_t);

  uint64_t* tmp = zallocNN(cb_size);
  
  {
    size_t ib_size = e->num_events_per_group[group] * sizeof(CUpti_EventID);
    size_t ib_offset = e->event_id_buffer_offsets[group];

    size_t ids_read = 0;
    
    CUPTI_FN(cuptiEventGroupReadAllEvents(e->event_groups[group],
                                          CUPTI_EVENT_READ_FLAG_NONE,
                                          &cb_size,
                                          //&e->event_counter_buffer[cb_offset],
                                          tmp,
                                          &ib_size,
                                          &e->event_id_buffer[ib_offset],
                                          &ids_read));

    printf("[%i] ids read: %" PRId64 "/ %" PRId64 "\n",
           group,
           ids_read,
           (size_t) e->num_events_per_group[group]);
  }
  
  {
    size_t num_counters = cb_size / sizeof(tmp[0]);
    size_t stride = e->num_instances_per_group[group];
    size_t cb_offset = e->event_counter_buffer_offsets[group];
    
    for (size_t i = 0; i < num_counters; ++i) {
      size_t base = cb_offset + i * stride;
      for (size_t j = 0; j < stride; ++j) {
        e->event_counter_buffer[base + j] += tmp[base + j];
      }
    }
  }

}

// if called, it must be called after read_group_all_events
static void read_group_per_event(cupti_event_data_t* e, uint32_t group) {  
  group_info_t* info = group_info_new(e, group);

  size_t bytes_read_per_event = sizeof(info->counters[0]) * info->num_instances;
  
  for (uint32_t i = 0; i < info->num_events; ++i) {
    CUPTI_FN(cuptiEventGroupReadEvent(info->group,
                                      CUPTI_EVENT_READ_FLAG_NONE,
                                      info->events[i],
                                      &bytes_read_per_event,
                                      &info->counters[i * info->num_instances]));

    ASSERT(bytes_read_per_event == sizeof(info->counters[0]) * info->num_instances);
  }

  for (uint32_t i = 0; i < g_group_info_count; ++i) {
    ASSERT(info->group != g_group_info_buffer[i].group);
  }
  
  group_info_append(info, group);
}

static void collect_group_events(cupti_event_data_t* e) {
  for (uint32_t i = 0; i < e->num_event_groups; ++i) {
    if (e->event_groups_read[i] == CED_EVENT_GROUP_UNREAD) {
      read_group_all_events(e, i);

      if (g_process_group_aos) {
        read_group_per_event(e, i);
        group_info_validate(e,
                            &g_group_info_buffer[i],
                            i);
      }
    }
  }

  for (uint32_t i = 0; i < e->num_event_groups; ++i) {
    if (e->event_groups_read[i] == CED_EVENT_GROUP_UNREAD) {
      e->event_groups_read[i] = CED_EVENT_GROUP_READ;
      e->count_event_groups_read++;
      
      CUPTI_FN(cuptiEventGroupDisable(e->event_groups[i]));
    }
  }

  for (uint32_t i = 0; i < e->num_event_groups; ++i) {
    if (e->event_groups_read[i] == CED_EVENT_GROUP_DONT_READ) {
      e->event_groups_read[i] = CED_EVENT_GROUP_UNREAD;
    }
  }
}

static void init_cupti_event_groups(cupti_event_data_t* e) {
#define MAX_EGS 30
  // static default; increase if more groups become necessary
  uint32_t max_egs = MAX_EGS; 
  uint32_t num_egs = 0;

  // we use a local buffer with an estimate,
  // so when we store the memory we aren't using
  // more than we need
  CUpti_EventGroup local_eg_assign[MAX_EGS];

  // CUpti_EventGroup is just a typedef for a pointer
  for (uint32_t i = 0; i < max_egs; ++i)
    local_eg_assign[i] = NULL;
    
#undef MAX_EGS
  
  for (uint32_t i = 0; i < e->event_names_buffer_length; ++i) {
    CUpti_EventID event_id = V_UNSET;
    
    CUptiResult err = cuptiEventGetIdFromName(e->cuda_device,
                                              e->event_names[i],
                                              &event_id);

    // even if the compute capability being targeted
    // is technically larger than the capability of the
    // set of events queried against, there is still variation between
    // cards. Some events simply won't be available for that
    // card.
    bool available = true;
    
    if (err != CUPTI_SUCCESS) {
      if (err == CUPTI_ERROR_INVALID_EVENT_NAME) {
        available = false;
      } else {
        // force an exit, since
        // something else needs to be
        // looked at
        CUPTI_FN(err);
      }
    } else {
      // in the future we'll only have ids,
      // so we may as well map them now for output.
      cupti_map_event_name_to_id(e->event_names[i], event_id);
    }
    
    if (available) {
      bool found = find_event_group(e,
                                    &local_eg_assign[0],
                                    event_id,
                                    max_egs,
                                    &num_egs);
      ASSERT(found);
    }

    printf("(%s) group found for index %u => %s:0x%x\n",
           available ? "available" : "unavailable",
           i,
           e->event_names[i],
           event_id);
  }

  ASSERT(num_egs <= max_egs /* see the declaration of max_egs if this fails */);

  if (num_egs == 0) {
    exit_msg(stderr,
             EUNSUPPORTED_EVENTS,
             "%s",
             "No supported events found within given list. "
             "Support can vary between device and compute capability.");
  }

  fill_event_groups(e, &local_eg_assign[0], num_egs);
}

static void init_cupti_event_names(cupti_event_data_t* e) {
  char* env_string = getenv(ENV_EVENTS);

  FILE* stream = stderr;
  
  if (env_string != NULL) { 
    size_t count = 0;
    char** list = env_var_list_read(env_string, &count);

    // Sanity check
    ASSERT(count < 0x1FF);

    if (list != NULL) {
      size_t i = 0;

      bool scanning = i < count;
      bool using_all = false;
    
      while (scanning) {
        if (strcmp(list[i], ENV_ALL_EVENTS) == 0) {
          fprintf(stream,
                  "(%s) Found %s in list. All event counters will be used.\n",
                  ENV_EVENTS,
                  ENV_ALL_EVENTS);
          
          e->event_names = &g_cupti_event_names_2x[0];
          e->event_names_buffer_length = g_cupti_event_names_2x_length;

          scanning = false;
          using_all = true;
        } else {
          fprintf(stream, "(%s) [%" PRIu64 "] Found %s\n", ENV_EVENTS, i, list[i]);
          i++;
          scanning = i < count;
        }
      }

      if (!using_all) {
        e->event_names = list;
        e->event_names_buffer_length = (uint32_t)count;
      }

      free_strlist(list, count);
    } else {
      exit_msg(stream,
               EBAD_INPUT,
               "(%s) %s",
               ENV_EVENTS,
               "Could not parse the environment list.");
    }
  } else {
    fprintf(stream,
            "%s undefined; defaulting to all event counters.\n",
            ENV_EVENTS);
    
    e->event_names = &g_cupti_event_names_2x[0];
    e->event_names_buffer_length = g_cupti_event_names_2x_length;
  }
}

static void init_cupti_event_buffers(cupti_event_data_t* e) {
  // get instance and event counts for each group
  {
    e->num_events_per_group = zallocNN(sizeof(e->num_events_per_group[0]) *
                                       e->num_event_groups);

    e->num_events_read_per_group = zallocNN(sizeof(e->num_events_read_per_group[0]) *
                                            e->num_event_groups);
    
    e->num_instances_per_group = zallocNN(sizeof(e->num_instances_per_group[0]) *
                                          e->num_event_groups);

    for (uint32_t i = 0; i < e->num_event_groups; ++i) {
      // instance count
      {
        uint32_t inst = 0;
        size_t instsz = sizeof(inst);
        CUPTI_FN(cuptiEventGroupGetAttribute(e->event_groups[i],
                                             CUPTI_EVENT_GROUP_ATTR_INSTANCE_COUNT,
                                             &instsz,
                                             &inst));

        e->num_instances_per_group[i] = inst;
      }

      // event count
      {
        uint32_t event = 0;
        size_t eventsz = sizeof(event);
        CUPTI_FN(cuptiEventGroupGetAttribute(e->event_groups[i],
                                             CUPTI_EVENT_GROUP_ATTR_NUM_EVENTS,
                                             &eventsz,
                                             &event));
        e->num_events_per_group[i] = event;
      }
    }                                          
  }
  
  // compute offsets for the event id buffer,
  // and allocate the memory.
  // for all groups
  {
    e->event_id_buffer_offsets = mallocNN(sizeof(e->event_id_buffer_offsets[0]) *
                                          e->num_event_groups);
    
    e->event_id_buffer_length = 0;
    
    for (uint32_t i = 0; i < e->num_event_groups; ++i) {
      e->event_id_buffer_offsets[i] = e->event_id_buffer_length;
      e->event_id_buffer_length += e->num_events_per_group[i];
    }

    e->event_id_buffer = zallocNN(sizeof(e->event_id_buffer[0]) *
                                  e->event_id_buffer_length);
    
  }
  
  // compute offset indices for the event counter buffer,
  // and allocate the memory.
  // for all groups
  {
    e->event_counter_buffer_offsets =
      mallocNN(sizeof(e->event_counter_buffer_offsets[0]) * e->num_event_groups);
    
    e->event_counter_buffer_length = 0;
    
    for (uint32_t i = 0; i < e->num_event_groups; ++i) {
      uint32_t accum = 0;
      for (uint32_t j = 0; j < i; ++j) {
        accum += e->num_events_per_group[j] * e->num_instances_per_group[j];
      }
      
      e->event_counter_buffer_offsets[i] = accum;
    }
    
    for (uint32_t i = 0; i < e->num_event_groups; ++i) {
      e->event_counter_buffer_length +=
        e->num_events_per_group[i] * e->num_instances_per_group[i];
    }
    
    e->event_counter_buffer =
      zallocNN(sizeof(e->event_counter_buffer[0]) * e->event_counter_buffer_length);  
  }
}

NVCD_EXPORT void cupti_name_map_free() {
  list_free_fn_impl(g_name_map_list,
                    cupti_name_map_t,
                    cupti_name_map_free_node,
                    self);
}

NVCD_EXPORT void cupti_map_event_name_to_id(const char* event_name, CUpti_EventID event_id) {
  if (cupti_find_event_name_from_id(event_id) == NULL) {
    cupti_name_map_t* node = mallocNN(sizeof(cupti_name_map_t));

    node->name = strdup(event_name);

    NOT_NULL(node->name);
    
    node->id = event_id;
    node->self.next = NULL;

    cupti_name_map_push(node);
  }
}

NVCD_EXPORT const char* cupti_find_event_name_from_id(CUpti_EventID id) {
  const char* ret = NULL;

  list_t* n = list_node(g_name_map_list,
                        cupti_name_map_t,
                        self);

  while (n != NULL && ret == NULL) {
    cupti_name_map_t* nm = list_base(n,
                                     cupti_name_map_t,
                                     self);
    if (nm->id == id) {
      ret = nm->name;
    }
    
    n = n->next;
  }

  return ret;
}

static char _peg_buffer[1 << 13] = { 0 };

static void print_event_group_soa(cupti_event_data_t* e, uint32_t group) {
  // used for iterative bounds checking
#define peg_buffer_length ARRAY_LENGTH(_peg_buffer) - 1
  
  memset(&_peg_buffer[0], 0, sizeof(_peg_buffer));
  
  uint64_t* pcounters = &e->event_counter_buffer[0];
  
  uint32_t ib_offset = e->event_id_buffer_offsets[group];
  uint32_t cb_offset = e->event_counter_buffer_offsets[group];
  
  uint32_t nepg = e->num_events_per_group[group];
  uint32_t nipg = e->num_instances_per_group[group];

  uint32_t next_cb_offset = 0;
  uint32_t next_ib_offset = 0;

  int ptr = 0;
  
  // bounds check ordering for
  // event_counter_buffer_offsets
  {
    uint32_t prev_cb_offset = (group > 0) ?
      
      e->event_counter_buffer_offsets[group - 1] :
      0;

    uint32_t prev_cb_offset_add = (group > 0) ?

      (e->num_events_per_group[group - 1] *
       e->num_instances_per_group[group - 1]) :
      0;

    ASSERT(prev_cb_offset + prev_cb_offset_add == cb_offset);
  }

  // bounds check ordering for
  // event_id_buffer_offsets
  {
    uint32_t prev_ib_offset = (group > 0) ?

      e->event_id_buffer_offsets[group - 1] :
      0;

    uint32_t prev_ib_offset_add = (group > 0) ?
      
      e->num_events_per_group[group - 1] :
      0;

    ASSERT(prev_ib_offset + prev_ib_offset_add == ib_offset);
  }

  // used for iterative bounds checking
  {
    next_cb_offset =
      group < (e->num_event_groups - 1) ?
      e->event_counter_buffer_offsets[group + 1] :
      e->event_counter_buffer_length;
  }
  
  // used for iterative bounds checking
  {
    next_ib_offset =
      group < (e->num_event_groups - 1) ?
      e->event_id_buffer_offsets[group + 1] :
      e->event_id_buffer_length;
  }
  
  for (uint32_t i = 0; i < nepg; ++i) {
    ASSERT(ib_offset + i < next_ib_offset);
    ASSERT(ptr < peg_buffer_length);

    CUpti_EventID eid = e->event_id_buffer[ib_offset + i];
    
    {
      const char* name = cupti_find_event_name_from_id(eid);
      
      ptr += sprintf(&_peg_buffer[ptr],
                     "[%" PRIu32 " (eid: 0x%" PRIx32 ")] %s:\n",
                     i,
                     eid,
                     name);
    }
    
    for (uint32_t j = 0; j < nipg; ++j) {
      uint32_t k = cb_offset + j * nepg + i;

      ASSERT(k < next_cb_offset);
      
      ptr += sprintf(&_peg_buffer[ptr],
                     "\t[%" PRIu32 "] %" PRIu64 " | 0x%" PRIx64 "\n",
                     j,
                     pcounters[k],
                     pcounters[k]);
    }
  }

  ASSERT(ptr <= peg_buffer_length);
  
  printf("======(SOA)GROUP %" PRIu32  "=======\n"
         "%s"
         "===\n",
         group,
         &_peg_buffer[0]);

#undef peg_buffer_length
}

static void print_event_group_aos(cupti_event_data_t* e, uint32_t group) {
  ASSERT(group < g_group_info_count);
  
  group_info_t* info = &g_group_info_buffer[group];
  
  printf("======(AOS) GROUP %" PRIu32  "=======\n", group);
  
  for (uint32_t i = 0; i < info->num_events; ++i){
    char* name = cupti_event_get_name(info->events[i]);

    printf("[%" PRIu32 "] %s\n", i, name);
    
    for (uint32_t j = 0; j < info->num_instances; ++j) {
      printf("\t[%" PRIu32 "] %" PRIu64 "\n",
             j,
             info->counters[i * info->num_instances + j]);
    }

    free(name);
  }

  printf("===\n");
}

NVCD_EXPORT void cupti_report_event_data(cupti_event_data_t* e) {
  if (g_process_group_aos) {
    for (uint32_t i = 0; i < e->num_event_groups; ++i) {
      group_info_t* info = &g_group_info_buffer[i];
      group_info_validate(e, info, i);
    }
  }
  
  for (uint32_t i = 0; i < e->num_event_groups; ++i) {
    print_event_group_soa(e, i);
    if (g_process_group_aos) {
      print_event_group_aos(e, i);
    }
  }

  if (e->is_root == true) {
    ASSERT(e->metric_data != NULL);
    
    for (uint32_t i = 0; i < e->metric_data->num_metrics; ++i) {
      print_cupti_metric(e->metric_data, i);
    }    
  }
}

static bool _message_reported = false;

NVCD_EXPORT void CUPTIAPI cupti_event_callback(void* userdata,
                                               CUpti_CallbackDomain domain,
                                               CUpti_CallbackId callback_id,
                                               CUpti_CallbackData* callback_info) {
  {
    bool found = false;
    size_t i = 0;

    while (i < NUM_CUPTI_RUNTIME_CBIDS && !found) {
      found = callback_id == g_cupti_runtime_cbids[i];
      i++;
    }

    ASSERT(found);
  }

  cupti_event_data_t* event_data = (cupti_event_data_t*) userdata;

  puts("-----------------");
  printf("event callback hit for event_data = %p\n", event_data);
  
  // For now it appears that the threads are the same between the main thread
  // and the thread this callback is installed in. The check is important though
  // since this could technically change. Some might consider this pedantic, but non-thread-safe
  // event handlers with user pointer data are a thing, and device synchronization waits
  // can obviously happen across multiple threads.  
  {
    event_data->thread_event_callback = pthread_self();
    
    volatile int threads_eq = pthread_equal(event_data->thread_event_callback,
                                            event_data->thread_event_data_init);

    if (threads_eq != 0) {
      if (!_message_reported) {
        fprintf(stderr, "%s is launched on the same thread as the main thread (this is good)\n", __FUNC__);
        _message_reported = true;
      }
    } else {
      exit_msg(stderr,
               ERACE_CONDITION,
               "Race condition detected in %s. "
               "Synchronization primitives will be needed for "
               "nvci_host_begin() caller's thread wait loop and "
               "the thread for this callback.\n", __FUNC__);
    }
  }

  // actual event handling
  {
    switch (callback_info->callbackSite) {
    case CUPTI_API_ENTER: {
      CUDA_RUNTIME_FN(cudaDeviceSynchronize());

      CUPTI_FN(cuptiSetEventCollectionMode(callback_info->context,
                                           CUPTI_EVENT_COLLECTION_MODE_KERNEL));

      //
      // We can get all of the event groups we wish to read,
      // but not necessarily at the same time.
      // In this case, it's necessary to repeatedly call the same kernel
      // until
      //           event_data->count_event_groups_read == event_data->num_event_groups
      // is true.
      // The state tracking is handled in this loop,
      // as well as in collect_group_events()
      //
      for (uint32_t i = 0; i < event_data->num_event_groups; ++i) {
        if (event_data->event_groups_read[i] == CED_EVENT_GROUP_UNREAD) {
          CUptiResult err = cuptiEventGroupEnable(event_data->event_groups[i]);

          if (err != CUPTI_SUCCESS) {
            if (err == CUPTI_ERROR_NOT_COMPATIBLE) {
              printf("Group %" PRIu32 " out of "
                     "%" PRIu32 " considered not compatible with the current set of enabled groups\n",
                     i,
                     event_data->num_event_groups);

              event_data->event_groups_read[i] = CED_EVENT_GROUP_DONT_READ;
            } else {
              CUPTI_FN(err);
            }
          } else {
            printf("Group %" PRIu32 " enabled.\n", i);
          }
        }
      }

      CUPTI_FN(cuptiDeviceGetTimestamp(callback_info->context,
                                       &event_data->stage_time_nsec_start));
    } break;

    case CUPTI_API_EXIT: {
      size_t finish_time = 0;
      
      CUDA_RUNTIME_FN(cudaDeviceSynchronize());
      CUPTI_FN(cuptiDeviceGetTimestamp(callback_info->context,
                                       &finish_time));

      collect_group_events(event_data);

      MAYBE_GROW_BUFFER_U32_NN(event_data->kernel_times_nsec,
                               event_data->num_kernel_times,
                               event_data->kernel_times_nsec_buffer_length);

      event_data->kernel_times_nsec[event_data->num_kernel_times] =
        finish_time - event_data->stage_time_nsec_start;

      event_data->num_kernel_times++;
    } break;

    default:
      ASSERT(false);
      break;
    }
  }
}

NVCD_EXPORT void cupti_event_data_subscribe(cupti_event_data_t* e) {
  ASSERT(e != NULL && e->subscriber == NULL && e->initialized);
  
  CUPTI_FN(cuptiSubscribe(&e->subscriber,
                          (CUpti_CallbackFunc)cupti_event_callback,
                          (void*) e));

  for (uint32_t i = 0; i < NUM_CUPTI_RUNTIME_CBIDS; ++i) {
    CUPTI_FN(cuptiEnableCallback(1,
                                 e->subscriber,
                                 CUPTI_CB_DOMAIN_RUNTIME_API,
                                 g_cupti_runtime_cbids[i]));
  }
}

NVCD_EXPORT void cupti_event_data_unsubscribe(cupti_event_data_t* e) {
  ASSERT(e != NULL && e->initialized && e->subscriber != NULL);
  CUPTI_FN(cuptiUnsubscribe(e->subscriber));
  //
  e->subscriber = NULL;
}

static inline void __cupti_event_data_init_base(cupti_event_data_t* e) {
  e->thread_event_data_init = pthread_self();

  e->kernel_times_nsec = zallocNN(sizeof(e->kernel_times_nsec[0]) *
                                  e->kernel_times_nsec_buffer_length);

  init_cupti_event_buffers(e);  
}

NVCD_EXPORT void cupti_event_data_init_from_ids(cupti_event_data_t* e,
                                                CUpti_EventID* event_ids,
                                                uint32_t num_event_ids) {
  ASSERT(e != NULL);
  ASSERT(e->cuda_context != NULL);
  ASSERT(e->cuda_device >= 0);
  ASSERT(!e->is_root);

  if (!e->initialized) {
    
    
    // event group initialization
    {
      CUpti_EventGroup eg_buf[50] = { 0 };
      uint32_t num_egs = 0;
      uint32_t max_egs = 50;
      
      for (uint32_t i = 0; i < num_event_ids; ++i) {
        volatile bool found = find_event_group(e,
                                               &eg_buf[0],
                                               event_ids[i],
                                               max_egs,
                                               &num_egs);

        ASSERT(found);
      }

      ASSERT(num_egs <= max_egs);
      
      fill_event_groups(e,
                        &eg_buf[0],
                        num_egs);
    }

    __cupti_event_data_init_base(e); 
    
    e->initialized = true;
  }
}

NVCD_EXPORT void cupti_event_data_init(cupti_event_data_t* e) {
  ASSERT(e != NULL);
  ASSERT(e->cuda_context != NULL);
  ASSERT(e->cuda_device >= 0);

  if (!e->initialized) {
    init_cupti_event_names(e);
    init_cupti_event_groups(e);

    __cupti_event_data_init_base(e); 
    
    if (e->is_root == true) {
      init_cupti_metric_data(e);
    }
    
    e->initialized = true;
  }
}


NVCD_EXPORT void cupti_event_data_set_null(cupti_event_data_t* e) {
  cupti_event_data_t tmp = CUPTI_EVENT_DATA_NULL;
  memcpy(e, &tmp, sizeof(tmp));
}

NVCD_EXPORT void cupti_event_data_free(cupti_event_data_t* e) {
  ASSERT(e != NULL);
  
  safe_free_v(e->event_id_buffer);
  safe_free_v(e->event_counter_buffer);
  
  safe_free_v(e->num_events_per_group);
  safe_free_v(e->num_instances_per_group);
  safe_free_v(e->num_events_read_per_group);
  
  safe_free_v(e->event_counter_buffer_offsets);
  safe_free_v(e->event_id_buffer_offsets);
  safe_free_v(e->event_groups_read);

  safe_free_v(e->kernel_times_nsec);
  
  for (size_t i = 0; i < e->num_event_groups; ++i) { 
    if (e->event_groups[i] != NULL) {
      CUPTI_FN(cuptiEventGroupDisable(e->event_groups[i]));
      CUPTI_FN(cuptiEventGroupRemoveAllEvents(e->event_groups[i]));
      CUPTI_FN(cuptiEventGroupDestroy(e->event_groups[i]));
    }
  }
  
  safe_free_v(e->event_groups);
  
  // TODO: event names may be either a subset of a static buffer
  // initialized in the .data section,
  // or a subset. Should add a flag to determine
  // whether or not the data needs to be freed.
  memset(e, 0, sizeof(*e));
}

NVCD_EXPORT void cupti_event_data_begin(cupti_event_data_t* e) {
  ASSERT(e != NULL);

  cupti_event_data_subscribe(e);
}

NVCD_EXPORT void cupti_event_data_end(cupti_event_data_t* e) {
  ASSERT(e != NULL);
  
  cupti_event_data_unsubscribe(e);
}

typedef char name_str_t[256];

NVCD_EXPORT char* cupti_event_get_name(CUpti_EventID eid) {
  name_str_t name = {0};

  size_t sz = sizeof(name);
  
  CUPTI_FN(cuptiEventGetAttribute(eid, CUPTI_EVENT_ATTR_NAME, &sz, &name[0]));

  return strdup(name);
}

NVCD_EXPORT CUpti_EventID* cupti_metric_get_event_ids(CUpti_MetricID metric, uint32_t* num_events) {
  ASSERT(num_events != NULL);
  
  CUPTI_FN(cuptiMetricGetNumEvents(metric, num_events));

  size_t sz = sizeof(CUpti_EventID) * (*num_events);

  CUpti_EventID* event_ids = zallocNN(sz);

  CUPTI_FN(cuptiMetricEnumEvents(metric, &sz, &event_ids[0]));

  return event_ids;
}

NVCD_EXPORT CUpti_MetricID* cupti_metric_get_ids(CUdevice dev, uint32_t* num_metrics) {
  CUPTI_FN(cuptiDeviceGetNumMetrics(dev, num_metrics));

  size_t array_size_bytes = sizeof(CUpti_MetricID) * (*num_metrics);
  
  CUpti_MetricID* metric_ids = mallocNN(array_size_bytes); 

  CUPTI_FN(cuptiDeviceEnumMetrics(dev,
                                  &array_size_bytes,
                                  &metric_ids[0]));

  return metric_ids;
}

NVCD_EXPORT char* cupti_metric_get_name(CUpti_MetricID metric) {
  name_str_t name = {0};

  size_t sz = sizeof(name);
          
  CUPTI_FN(cuptiMetricGetAttribute(metric,
                                   CUPTI_METRIC_ATTR_NAME,
                                   &sz,
                                   (void*) &name[0]));

  return strdup(name);
}

NVCD_EXPORT void cupti_event_data_calc_metrics(cupti_event_data_t* e) {
  ASSERT(e != NULL);
  ASSERT(e->is_root == true);
  ASSERT(e->metric_data != NULL);
  
  calc_cupti_metrics(e->metric_data);
}

NVCD_EXPORT bool cupti_event_data_callback_finished(cupti_event_data_t* e) {
  ASSERT(e->count_event_groups_read
         <= e->num_event_groups /* serious problem if this fails */);
  
  return e->count_event_groups_read
    == e->num_event_groups;
}

