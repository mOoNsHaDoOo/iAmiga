
#ifndef SAVEDISK_SLOT
#define SAVEDISK_SLOT 512
#endif

size_t savedisk_get_checksum(void *mem, size_t size);
void savedisk_apply_changes(void *mem, void *patch, size_t patch_size);
size_t savedisk_get_changes_file(void *mem, size_t size, void *patch, char *filename);
size_t savedisk_get_changes(void *mem, size_t size, void *patch, void *orig);
