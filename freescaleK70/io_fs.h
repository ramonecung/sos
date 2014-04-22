#ifndef IOFS_H
#define IOFS_H

#define FILE_SIZE 2048

struct NamedFile {
    const char *filename;
    struct NamedFile *next;
    char *data;
};
typedef struct NamedFile NamedFile;

Stream *find_stream_fs(enum device_instance di);
int create_fs(const char *filename);
int file_exists(const char *filename);
int delete_fs(const char *filename);
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
void purge_open_files_fs(void);

NamedFile *find_file(const char *filename);

#endif
