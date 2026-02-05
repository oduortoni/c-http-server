#ifndef _TEMPLATE_HEADER_H
#define _TEMPLATE_HEADER_H

/*
 * Maximum allowed size for a template file.
 * Used to prevent excessive memory usage.
 * Approximately 512 KiB.
 */
#define MAX_TEMPLATE_SIZE (512 * 1024)

/* template_load
 * Loads a template file from disk into memory.
 *
 * filename: Path to the template file.
 *
 * Returns: Pointer to the loaded template content on success.
 *          NULL if the file cannot be loaded or exceeds MAX_TEMPLATE_SIZE.
 *
 * Note: The caller is responsible for freeing the returned memory
 *       using free_template().
 */
char* template_load(const char* filename);

/* free_template
 * Frees memory previously returned by template_load.
 *
 * template: Pointer returned by template_load.
 */
void template_free(char* template);

#endif  // _TEMPLATE_HEADER_H