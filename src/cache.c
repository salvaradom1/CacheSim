#include "cache.h"
#include "arguments.h"
#include <stdio.h>
#define PENALTY 100

set_t *sets_create(size_t num_sets, size_t num_ways);
void sets_destroy(set_t *sets, size_t num_sets);
void handle_cache_miss(cache_t *cache, operation_t *operation, uint64_t index,
                       uint64_t tag);
void handle_cache_hit(cache_t *cache, operation_t *operation, int hit_index,
                      uint64_t index, uint64_t tag);
int find_replacement(set_t *set, uint64_t num_ways);
int find_block(set_t *set, uint64_t tag, uint64_t num_ways);
void print_verbose(config_t *args, size_t i, char *case_id, uint64_t index,
                   uint64_t tag, int line_used, int64_t old_tag, bool was_valid,
                   bool was_dirty, size_t last_used);

cache_t *cache_create(config_t *arguments) {
  if (!arguments) {
    return NULL;
  }
  cache_t *cache = calloc(1, sizeof(cache_t));
  if (!cache) {
    return NULL;
  }
  cache->metrics = metrics_create();
  if (!cache->metrics) {
    free(cache);
    return NULL;
  }
  cache->sets = sets_create(arguments->set_amount, arguments->associativity);
  if (!cache->sets) {
    free(cache->metrics);
    free(cache);
    return NULL;
  }
  cache->arguments = arguments;
  return cache;
}

void cache_memory_access(cache_t *cache, operation_t *operation) {
  if (!cache || !operation || !cache->arguments || !cache->metrics ||
      !cache->sets) {
    return;
  }
  config_t *args = cache->arguments;
  metrics_t *metrics = cache->metrics;

  uint64_t index =
      (operation->memory_address / args->block_size) % args->set_amount;
  uint64_t tag =
      operation->memory_address / (args->block_size * args->set_amount);

  if (operation->operation == 'R') {
    metrics->loads++;
  } else {
    metrics->stores++;
  }
  set_t *set = &cache->sets[index];
  int hit_index = find_block(set, tag, args->associativity);

  if (hit_index != -1) {
    handle_cache_hit(cache, operation, hit_index, index, tag);
  } else {
    handle_cache_miss(cache, operation, index, tag);
  }
}

void cache_destroy(cache_t *cache) {
  if (!cache) {
    return;
  }
  sets_destroy(cache->sets, cache->arguments->set_amount);
  metrics_destroy(cache->metrics);
  free(cache);
}

operation_t *operation_create() {
  operation_t *operation = calloc(1, sizeof(operation_t));
  return operation;
}

void operation_destroy(operation_t *operation) { free(operation); }

/////////////////////////// AUX ////////////////////////////

set_t *sets_create(size_t num_sets, size_t num_ways) {
  set_t *sets = calloc(num_sets, sizeof(set_t));
  if (!sets)
    return NULL;
  for (size_t i = 0; i < num_sets; i++) {
    sets[i].ways = calloc(num_ways, sizeof(way_t));
    if (!sets[i].ways) {
      for (size_t j = 0; j < i; j++) {
        free(sets[j].ways);
      }
      free(sets);
      return NULL;
    }
  }
  return sets;
}

void sets_destroy(set_t *sets, size_t num_sets) {
  if (!sets)
    return;
  for (size_t i = 0; i < num_sets; i++) {
    free(sets[i].ways);
  }
  free(sets);
}

int find_replacement(set_t *set, uint64_t num_ways) {
  if (!set || num_ways == 0)
    return -1;
  for (size_t i = 0; i < num_ways; i++) {
    if (!set->ways[i].valid) {
      return (int)i;
    }
  }
  int lru_index = 0;
  size_t oldest = set->ways[0].last_used;
  for (size_t i = 1; i < num_ways; i++) {
    if (set->ways[i].last_used < oldest) {
      oldest = set->ways[i].last_used;
      lru_index = (int)i;
    }
  }
  return lru_index;
}

void handle_cache_hit(cache_t *cache, operation_t *operation, int hit_index,
                      uint64_t index, uint64_t tag) {
  way_t *way = &cache->sets[index].ways[hit_index];
  metrics_t *metrics = cache->metrics;

  size_t last_used_prev = way->last_used;
  bool was_dirty = way->dirty;
  way->last_used = operation->i;
  if (operation->operation == 'W') {
    way->dirty = true;
  }
  metrics_hit_update(metrics, operation->operation);
  print_verbose(cache->arguments, operation->i, "1", index, tag, hit_index,
                way->tag, true, was_dirty, last_used_prev);
}

void handle_cache_miss(cache_t *cache, operation_t *operation, uint64_t index,
                       uint64_t tag) {
  config_t *args = cache->arguments;
  metrics_t *metrics = cache->metrics;
  set_t *set = &cache->sets[index];

  int replace_index = find_replacement(set, args->associativity);
  way_t *replacement = &set->ways[replace_index];

  uint64_t old_tag = replacement->valid ? replacement->tag : 0;
  bool was_valid = replacement->valid;
  bool was_dirty = replacement->valid && replacement->dirty;
  size_t last_used_prev = replacement->last_used;

  int penalty_cycles = 1 + (was_dirty ? 2 * PENALTY : PENALTY);
  if (operation->operation == 'R') {
    metrics->read_time += penalty_cycles;
  } else if (operation->operation == 'W') {
    metrics->write_time += penalty_cycles;
  }

  metrics_miss_update(metrics, operation->operation, was_dirty);
  if (was_dirty) {
    metrics->bytes_written += args->block_size;
  }
  replacement->tag = tag;
  replacement->valid = true;
  replacement->last_used = operation->i;
  replacement->dirty = (operation->operation == 'W');
  metrics->bytes_read += args->block_size;

  print_verbose(args, operation->i, was_dirty ? "2b" : "2a", index, tag,
                replace_index, old_tag, was_valid, was_dirty, last_used_prev);
}

int find_block(set_t *set, uint64_t tag, uint64_t num_ways) {
  for (size_t i = 0; i < num_ways; i++) {
    if (set->ways[i].valid && set->ways[i].tag == tag) {
      return (int)i;
    }
  }
  return -1;
}

void print_verbose(config_t *args, size_t i, char *case_id, uint64_t index,
                   uint64_t tag, int line_used, int64_t old_tag, bool was_valid,
                   bool was_dirty, size_t last_used) {
  if (!args->verbose_mode || i < args->first_line || i > args->last_line) {
    return;
  }
  int dirty_bit = was_dirty ? 1 : 0;
  if (was_valid) {
    if (args->associativity > 1) {
      printf("%zu %s %lx %lx %d %lx 1 %d %zu\n", i, case_id, index, tag,
             line_used, (uint64_t)old_tag, dirty_bit, last_used);
    } else {
      printf("%zu %s %lx %lx %d %lx 1 %d\n", i, case_id, index, tag, line_used,
             (uint64_t)old_tag, dirty_bit);
    }
  } else {
    if (args->associativity > 1) {
      printf("%zu %s %lx %lx %d -1 0 0 %zu\n", i, case_id, index, tag,
             line_used, last_used);
    } else {
      printf("%zu %s %lx %lx %d -1 0 0\n", i, case_id, index, tag, line_used);
    }
  }
}