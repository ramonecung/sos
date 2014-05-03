#ifndef IOFS_H
#define IOFS_H

void initialize_io_fs(void);

int create_fs(const char *filename);

int delete_fs(const char *filename);
void free_file_blocks(NamedFile *file);

int setup_stream_fs(Stream *stream, NamedFile *file);

NamedFile *find_file(const char *filename);
int file_exists(const char *filename);

int fputc_fs(int c, Stream *stream);
int fgetc_fs(Stream *stream);

int filename_valid(const char *filename);
int prefix_valid(const char *filename);
int basename_valid(const char *basename);
int valid_basename_character(char c);

#endif
