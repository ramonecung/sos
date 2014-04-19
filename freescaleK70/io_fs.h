#ifndef IOFS_H
#define IOFS_H

#define MAX_OPEN_FILES 1024
#define FILE_SIZE 2048

Stream *fopen_fs(const char *filename);
int fclose_fs(Stream *stream);
int fputc_fs(int c, Stream *stream);
int fgetc_fs(Stream *stream);
int filename_valid(const char *filename);
int prefix_valid(const char *filename);
int basename_valid(const char *basename);
int valid_basename_character(char c);
unsigned short next_file_id(void);
void initialize_io_fs(void);
void purge_open_files(void);

#endif
