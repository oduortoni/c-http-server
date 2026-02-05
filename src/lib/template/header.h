#ifndef _TEMPLATE_HEADER_H
#define _TEMPLATE_HEADER_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Maximum allowed size for a template file.
 * Used to prevent excessive memory usage.
 * Approximately 512 KiB.
 */
#define MAX_TEMPLATE_SIZE (512 * 1024)

/**
 * template_load
 * Loads a template file into memory.
 * @param filename The path to the template file.
 * @return A pointer to the loaded template content, or NULL on failure.
 *         The caller is responsible for freeing the returned pointer
 *          using free_template.
 */
char* template_load(const char* filename);

/**
 * template_free
 * Frees the memory allocated for a template.
 * @param template A pointer to the template content previously loaded
 *          with load_template().
 */
void template_free(char* template);

#endif  // _TEMPLATE_HEADER_H