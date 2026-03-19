#include "metrics.h"
#include <stdio.h>
#include <stdlib.h>

metrics_t *metrics_create() { return calloc(1, sizeof(metrics_t)); }

void metrics_miss_update(metrics_t *metrics, char operation_type,
                         bool was_dirty) {
  if (!metrics)
    return;
  if (operation_type == 'R') {
    metrics->rmiss++;
    if (was_dirty)
      metrics->drmiss++;
  } else if (operation_type == 'W') {
    metrics->wmiss++;
    if (was_dirty)
      metrics->dwmiss++;
  }
}

void metrics_hit_update(metrics_t *metrics, char operation_type) {
  if (!metrics)
    return;
  if (operation_type == 'R') {
    metrics->read_time += 1;
  } else if (operation_type == 'W') {
    metrics->write_time += 1;
  }
}

void show_metrics(const metrics_t *metrics, const config_t *config) {
  if (!metrics || !config)
    return;

  uint64_t total_accesses = metrics->loads + metrics->stores;
  uint64_t total_misses = metrics->rmiss + metrics->wmiss;

  if (config->associativity > 1) {
    fprintf(stdout, "%lu-way, %lu sets, size = %luKB\n", config->associativity,
            config->set_amount, config->cache_size / 1024);
  } else {
    fprintf(stdout, "direct-mapped, %lu sets, size = %luKB\n",
            config->set_amount, config->cache_size / 1024);
  }
  fprintf(stdout, "loads %lu stores %lu total %lu\n", metrics->loads,
          metrics->stores, total_accesses);
  fprintf(stdout, "rmiss %lu wmiss %lu total %lu\n", metrics->rmiss,
          metrics->wmiss, total_misses);
  fprintf(stdout, "dirty rmiss %lu dirty wmiss %lu\n", metrics->drmiss,
          metrics->dwmiss);
  fprintf(stdout, "bytes read %lu bytes written %lu\n", metrics->bytes_read,
          metrics->bytes_written);
  fprintf(stdout, "read time %lu write time %lu\n", metrics->read_time,
          metrics->write_time);
  fprintf(stdout, "miss rate %f\n",
          total_accesses ? (float)total_misses / total_accesses : 0.0);
}

void metrics_destroy(metrics_t *metrics) { free(metrics); }