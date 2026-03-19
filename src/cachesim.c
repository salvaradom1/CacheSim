#include "arguments.h"
#include "cache.h"
#include "metrics.h"
#include "txt_reader.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char *argv[]) {
  config_t *arguments = arguments_parse(argc, argv);
  if (!arguments) {
    fputs("Pasaste mal los argumentos", stderr);
    return -1;
  }
  if (!arguments_verify(arguments)) {
    fputs("Pasaste bien los argumentos pero sus valores son invalidos", stderr);
    arguments_destroy(arguments);
    return -1;
  }
  file_t *file_xex = file_open(arguments->file_route);
  if (!file_xex) {
    fputs("El archivo que pasaste como argumento es invalido", stderr);
    arguments_destroy(arguments);
    return -1;
  }
  cache_t *cache = cache_create(arguments);
  if (!cache) {
    fputs("no se pudo reservar memoria", stderr);
    arguments_destroy(arguments);
    file_close(file_xex);
    return -2;
  }
  operation_t *operation = operation_create();
  if (!operation) {
    file_close(file_xex);
    cache_destroy(cache);
    arguments_destroy(arguments);
    return -2;
  }
  for (size_t i = 0; file_there_are_more_lines(file_xex); i++) {
    const char *line = file_read_line(file_xex);
    if (!line) {
      file_close(file_xex);
      cache_destroy(cache);
      arguments_destroy(arguments);
      operation_destroy(operation);
      return -2;
    } else if (sscanf(line, "%x: %c %x %hhu %lx",
                      &operation->instruction_pointer, &operation->operation,
                      &operation->memory_address, &operation->bytes,
                      &operation->data) != 5) {
      break;
    }
    operation->i = i;
    cache_memory_access(cache, operation);
  }
  show_metrics(cache->metrics, arguments);
  file_close(file_xex);
  cache_destroy(cache);
  arguments_destroy(arguments);
  operation_destroy(operation);
  return 0;
}