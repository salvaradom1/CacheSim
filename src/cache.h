#ifndef CACHE_H
#define CACHE_H

#include "arguments.h"
#include "metrics.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint64_t tag;
  bool valid;
  bool dirty;
  size_t last_used;
} way_t;

typedef struct {
  way_t *ways;
} set_t;

typedef struct {
  set_t *sets;
  metrics_t *metrics;
  config_t *arguments;
} cache_t;

typedef struct {
  uint32_t instruction_pointer;
  char operation;
  uint32_t memory_address;
  uint8_t bytes;
  uint64_t data;
  size_t i;
} operation_t;

typedef struct {
  uint64_t old_tag;
  bool is_valid;
} old_tag_t;

cache_t *cache_create(config_t *arguments);
void cache_destroy(cache_t *cache);
void cache_memory_access(cache_t *cache, operation_t *operation);
operation_t *operation_create();
void operation_destroy(operation_t *operation);
#endif // CACHE_H
