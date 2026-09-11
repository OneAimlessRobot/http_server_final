#ifndef RESOURCE_CONSTS_H
#define RESOURCE_CONSTS_H


#define tmpOneDIR_HTML "%s.tmp%lx.html"
#define tmpTwoDIR_HTML "%s.tmp1%lx.html"

#define tmpOneFIND_HTML "/.tmp%lx.find"
#define tmpTwoFIND_HTML "/.tmp1%lx.find"


#define tmpOneTXT_HTML "%s.tmp%lx.txt"
#define tmpTwoTXT_HTML "%s.tmp1%lx.txt"


#define tmpOneRSS_XML "%s.tmp%lx.rss"
#define tmpTwoRSS_XML "%s.tmp1%lx.rss"


#define tmpOnePHP_HTML "%s.tmp%lx.php"
#define tmpTwoPHP_HTML "%s.tmp1%lx.php"


#define tmpOneRCON_HTML "/.tmp%lx.rcon"
#define tmpTwoRCON_HTML "/.tmp1%lx.rcon"

#define tmpOneDATE_HTML "/.tmp%lx.date"
#define tmpTwoDATE_HTML "/.tmp1%lx.date"


#define DEFAULT_COMPRESS_RESOURCE 0
#define DEFAULT_CACHE_RESOURCE 0

extern char* excluded_xtensions[];


extern MimeEntry mime_types[];

u_int8_t get_mime_type_compressibility(u_int64_t table_index);

u_int8_t get_mime_type_cacheability(u_int64_t table_index);

const char* get_mime_type(const char *filename, u_int64_t* index_obtained);

int check_stat_of_file(const char* full_file_path,struct stat * statbuf,u_int8_t logging,FILE*logstream);


#endif
