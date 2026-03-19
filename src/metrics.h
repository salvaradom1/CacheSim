
#ifndef METRICS_H
#define METRICS_H

#include "arguments.h"
#include <stdint.h>

typedef struct {
  uint64_t loads;
  uint64_t stores;
  uint64_t rmiss;
  uint64_t wmiss;
  uint64_t drmiss;
  uint64_t dwmiss;
  uint64_t bytes_read;
  uint64_t bytes_written;
  uint64_t read_time;
  uint64_t write_time;
} metrics_t;

metrics_t *metrics_create();
void metrics_miss_update(metrics_t *metrics, char operation_type,
                         bool was_dirty);
void metrics_hit_update(metrics_t *metrics, char operation_type);
void show_metrics(const metrics_t *metrics, const config_t *config);
void metrics_destroy(metrics_t *metrics);

#endif