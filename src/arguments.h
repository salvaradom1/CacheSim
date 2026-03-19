#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct config {
  const char *file_route;
  uint64_t cache_size;
  uint64_t associativity;
  uint64_t set_amount;
  uint64_t first_line;
  uint64_t last_line;
  uint64_t block_size;
  bool verbose_mode;
} config_t;

config_t *arguments_parse(int argc, const char *argv[]);
bool arguments_verify(config_t *arguments);
void arguments_destroy(config_t *arguments);

#endif // ARGUMENTS.H