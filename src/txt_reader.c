#include "txt_reader.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 256

struct file {
  FILE *file;
  int lines_read;
  char *actual_line;
};

// funciones auxiliares
char *read_line_aux(file_t *file) {
  int error = 0, over = 0;
  size_t size_buffer = INITIAL_CAPACITY;
  size_t char_read = 0;
  char *line = calloc(size_buffer, sizeof(char));
  if (!line) {
    return NULL;
  }
  while (!error && !over &&
         fgets(line + char_read, (int)(size_buffer - char_read), file->file)) {
    char_read = strlen(line);
    if (char_read >= size_buffer - 1) {
      size_buffer *= 2;
      char *bufferaux = realloc(line, sizeof(char) * size_buffer);
      if (!bufferaux) {
        error = 1;
      } else {
        line = bufferaux;
      }
    } else if (char_read < size_buffer - 1) {
      line[strcspn(line, "\n")] = '\0';
      char_read = strlen(line);
      char *bufferaux = realloc(line, sizeof(char) * (char_read + 1));
      if (!bufferaux) {
        error = 1;
      } else {
        line = bufferaux;
        over = 1;
      }
    }
  }
  if (error) {
    free(line);
    return NULL;
  }
  return line;
}

file_t *file_open(const char *route) {
  if (!route) {
    return NULL;
  }
  file_t *file = calloc(1, sizeof(file_t));
  if (!file) {
    return NULL;
  }

  file->file = fopen(route, "r");
  if (!file->file) {
    free(file);
    return NULL;
  }
  return file;
}

const char *file_read_line(file_t *file) {
  if (!file || feof(file->file)) {
    return NULL;
  }
  if (file->actual_line) {
    free(file->actual_line);
    file->actual_line = NULL;
  }
  char *line = read_line_aux(file);
  if (!line) {
    return NULL;
  }
  file->actual_line = line;
  file->lines_read++;
  return file->actual_line;
}

int file_there_are_more_lines(file_t *file) {
  return (!file) ? 0 : !feof(file->file);
}

int file_lines_read(file_t *file) { return (!file) ? 0 : file->lines_read; }

void file_close(file_t *file) {
  if (file) {
    fclose(file->file);

    if (file->actual_line)
      free(file->actual_line);

    free(file);
  }
}
