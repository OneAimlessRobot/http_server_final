#ifndef RESOURCE_CONSTS_H
#define RESOURCE_CONSTS_H





extern char* excluded_xtensions[];


extern MimeEntry mime_types[];

extern string_pair upload_target_to_location_mappings[];

u_int8_t get_mime_type_compressibility(u_int64_t table_index);

u_int8_t get_mime_type_cacheability(u_int64_t table_index);

int64_t get_mime_type_max_cacheable_size(u_int64_t table_index);

int64_t get_mime_type_max_compress_size(u_int64_t table_index);

const char* get_mime_type(const char *filename, int64_t* index_obtained);

int check_stat_of_file(const char* full_file_path,struct stat * statbuf,u_int8_t logging,FILE*logstream);

const char* get_upload_target(const char *target, int64_t* index_obtained);

#endif
