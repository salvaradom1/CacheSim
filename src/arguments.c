#include "arguments.h"

bool is_power_of_2(uint64_t n) { return n > 0 && (n & (n - 1)) == 0; }

bool string_to_uint64_t(const char *str, uint64_t *num) {
  if (!str || !num) {
    return false;
  }
  uint64_t result = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    char c = str[i];
    if (c < '0' || c > '9') {
      return false;
    }
    uint8_t digit = c - '0';
    if (result > (UINT64_MAX - digit) / 10) {
      return false;
    }
    result = result * 10 + digit;
  }
  *num = result;
  return true;
}

config_t *arguments_parse(int argc, const char *argv[]) {
  if (argc != 5 && argc != 8) {
    fputs("La cantidad de argumentos que estas pasando es invalida\n", stderr);
    fputs("./cachesim archivo.xex TAMAÑO_CACHE CANTIDAD_DE_VIAS CANTIDAD_DE "
          "SETS\n",
          stderr);
    fputs("./cachesim archivo.xex TAMAÑO_CACHE CANTIDAD_DE_VIAS CANTIDAD_DE "
          "SETS -v PRIMER_LINEA ULTIMA_LINEA\n",
          stderr);
    return NULL;
  }
  config_t *arguments = calloc(1, sizeof(config_t));
  if (!arguments) {
    perror("Error al asignar memoria");
    return NULL;
  }
  arguments->file_route = argv[1];
  if (!string_to_uint64_t(argv[2], &arguments->cache_size)) {
    free(arguments);
    return NULL;
  }
  if (!string_to_uint64_t(argv[3], &arguments->associativity)) {
    free(arguments);
    return NULL;
  }
  if (!string_to_uint64_t(argv[4], &arguments->set_amount)) {
    free(arguments);
    return NULL;
  }
  arguments->block_size = arguments->cache_size /
                          (arguments->set_amount * arguments->associativity);
  if (argc == 5) {
    arguments->verbose_mode = false;
    return arguments;
  } else {
    arguments->verbose_mode = true;
    if (strcmp("-v", argv[5]) == 0) {
      if (!string_to_uint64_t(argv[6], &arguments->first_line)) {
        free(arguments);
        return NULL;
      }
      if (!string_to_uint64_t(argv[7], &arguments->last_line)) {
        free(arguments);
        return NULL;
      }
      if (arguments->last_line < arguments->first_line) {
        free(arguments);
        return NULL;
      }
    } else {
      printf("No pasaste bien el modo verboso la flag es -v");
      free(arguments);
      return NULL;
    }
  }
  return arguments;
}

bool arguments_verify(config_t *arguments) {
  if (!arguments) {
    return false;
  }
  if (!is_power_of_2(arguments->cache_size) ||
      !is_power_of_2(arguments->set_amount) ||
      !is_power_of_2(arguments->associativity) ||
      !is_power_of_2(arguments->block_size)) {
    return false;
  }
  if (arguments->cache_size <=
      arguments->set_amount * arguments->associativity) {
    fputs("la combinacion del tamaño de la cache el numero de sets y el numero "
          "de vias es incorrecta\n",
          stderr);
    return false;
  }
  return true;
}

void arguments_destroy(config_t *arguments) {
  if (!arguments) {
    return;
  }
  free(arguments);
}