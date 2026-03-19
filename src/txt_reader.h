#ifndef TXT_READER
#define TXT_READER

typedef struct file file_t;

/**
 * Abre un archivo DE TEXTO para su posterior lectura.
 *
 * Devuelve el archivo si se pudo abrir, NULL en caso contrario.
 */
file_t *file_open(const char *route);

/**
 * Lee una línea del archivo.
 *
 * Devuelve un puntero a la línea leída, o NULL si no hay más líneas.
 */
const char *file_read_line(file_t *file);

/**
 * Indica si hay más líneas por leer en el archivo.
 *
 * Devuelve 1 si hay más líneas, 0 en caso contrario.
 */
int file_there_are_more_lines(file_t *file);

/**
 * Devuelve la cantidad de líneas leídas hasta el momento.
 *
 * Devuelve 0 si el archivo es NULL.
 */
int file_lines_read(file_t *file);

/**
 * Cierra el archivo y libera toda la memoria reservada.
 */
void file_close(file_t *file);

#endif // TXT_READER_H
