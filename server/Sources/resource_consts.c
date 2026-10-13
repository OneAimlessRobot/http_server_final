#include "../Includes/preprocessor.h"
#include "../Includes/resource_consts.h"




char* excluded_xtensions[]={
		"class",
		"pem",
		"sample",
		"boundary",
		"dll",
		"so",
		"o",
		"exe",
		"bin",
		"iso",
		NULL
};



MimeEntry mime_types[] = {
	
	/*
	 
	 IMAGE MIMETYPES
    
    */
    
    
    {".png", "image/png", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".PNG", "image/png", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".jpg", "image/jpeg", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".jpeg", "image/jpeg", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".webp", "image/webp", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".gif", "image/gif", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".jfif", "image/jpeg", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".avif", "image/avif", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".bmp", "image/bmp", 0,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".tga", "image/x-targa", 0,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".dds","image/x-dds", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".vtf","image/x-vtf", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".ico", "image/x-icon", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".icon", "image/x-icon", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".svg", "image/svg+xml", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    
    /*
	 
	 AUDIO MIMETYPES
    
    */
    
    {".wav", "audio/wav", 0,1,DEFAULT_MAX_COMPRESS_SIZE, (high_order_to_byte((int64_t)2,MEGA))},
    {".webm", "audio/webm", 0,1,DEFAULT_MAX_COMPRESS_SIZE, (high_order_to_byte((int64_t)2,MEGA))},
    {".m4a", "audio/mp4", 0,1,DEFAULT_MAX_COMPRESS_SIZE, (high_order_to_byte((int64_t)2,MEGA))},
    {".mp3", "audio/mp3", 0,1,DEFAULT_MAX_COMPRESS_SIZE, (high_order_to_byte((int64_t)2,MEGA))},
    {".ogg","audio/ogg", 0,1,DEFAULT_MAX_COMPRESS_SIZE, (high_order_to_byte((int64_t)2,MEGA))},
    {".flac","audio/flac", 0,1,DEFAULT_MAX_COMPRESS_SIZE, (high_order_to_byte((int64_t)2,MEGA))},
      
    
    
    /*
	 
	 VIDEO MIMETYPES
    
    */
    
    {".mp4", "video/mp4", 0,1,DEFAULT_MAX_COMPRESS_SIZE,(high_order_to_byte((int64_t)20,MEGA))},
    {".flv","video/x-flv", 0,1,DEFAULT_MAX_COMPRESS_SIZE, (high_order_to_byte((int64_t)20,MEGA))},
    {".avi","video/x-msvideo",0,1,DEFAULT_MAX_COMPRESS_SIZE, (high_order_to_byte((int64_t)20,MEGA))},
    {".3gp","video/3gpp", 0,1,DEFAULT_MAX_COMPRESS_SIZE,(high_order_to_byte((int64_t)20,MEGA))},
    
    /*
     
		DOCUMENT MIMETYPES
     
    */
    
    
    {".html", "text/html", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".css", "text/css", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".yml", "text/yaml", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".yaml", "text/yaml", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".txt", "text/html", 0,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".sp", "text/x-sourcepawn", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".sma", "text/x-sourcepawn", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".inc", "text/x-sourcepawn", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".cpp", "text/x-c++", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".c", "text/x-c++", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".h", "text/x-c++", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".hpp", "text/x-cpp", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".cfg", "text/x-config", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".php", "text/html", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".vdf", "text/plain", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".gam", "text/plain", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".py", "text/x-python", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".sh","text/plain", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".lst","text/plain", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".db","text/plain", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".md","text/markdown", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".log", "text/plain", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".vmt","text/x-vmt", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".pl","text/plain", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".java", "text/x-java-source", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    
      
    /*
		MY APPLICATION MIMETYPES	
    */
    
    {".rss", "application/rss+xml", 1,0,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".date", "text/html", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".rcon","text/html", 1,0,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".find","text/html", 1,0,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".dir","text/html", 1,0,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".pretty_txt","text/html", 1,0,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    
    /*
     
		STANDARD APPLICATION MIMETYPES
     
    */
    
    {".js", "application/javascript", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".wasm", "application/wasm", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".webmanifest", "application/manifest+json", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".json", "application/json", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".wad", "application/x-doom-wad", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".php", "application/x-httpd-php", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".pdf", "application/pdf", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".xml", "application/xml", 1,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    /*
     
		MISC MIMETYPES
     
    */
    
    {".mdl", "application/octet-stream", 0,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".spr", "application/octet-stream", 0,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    {".bsp", "application/octet-stream", 0,1,DEFAULT_MAX_COMPRESS_SIZE, DEFALT_MAX_CACHE_SIZE},
    
    /*
     
		FALLBACK MIMETYPE
     
    */
    {NULL, "application/octet-stream", 0,1,DEFAULT_MAX_COMPRESS_SIZE,DEFALT_MAX_CACHE_SIZE}
};

const char* get_mime_type(const char *filename, int64_t* index_obtained){
    int64_t safeguard_ptr = 0;
    
    index_obtained = ((index_obtained!=NULL)?index_obtained:&safeguard_ptr);
	*index_obtained=-1;
    
    const char *dot = strrchr(filename, '.');
    
    if (!dot){
		return "application/octet-stream";
	}
    for ( (*index_obtained) = 0;
		  mime_types[(*index_obtained)].extension != NULL;
		  (*index_obtained)++ ) {

        if (strcicmp(dot, mime_types[(*index_obtained)].extension) == 0) {
            return mime_types[(*index_obtained)].mime_type;
        }
    }
    *index_obtained=-1;
    return "application/octet-stream";
}

u_int8_t get_mime_type_compressibility(u_int64_t table_index) {
    return (mime_types[table_index].allows_compression);
}
u_int8_t get_mime_type_cacheability(u_int64_t table_index) {
    return (mime_types[table_index].to_be_cached);
}
int64_t get_mime_type_max_cacheable_size(u_int64_t table_index) {
    return (mime_types[table_index].maximum_size_to_cache_bytes);
}
int64_t get_mime_type_max_compress_size(u_int64_t table_index) {
    return (mime_types[table_index].maximum_size_to_compress_bytes);
}
int check_stat_of_file(const char* full_file_path,struct stat * statbuf,u_int8_t logging, FILE*logstream){
		
	int result= stat(full_file_path,statbuf);
	if(result <0){
		if(logging){
				fprintf(logstream,"The file |%s| could not be stat-ed!\nError string: %s\n",
								full_file_path,
								strerror(errno));
		}
	}
	return result;
}

/*
	these paths are relative to defaultTarget (The root of the server)
	slash at the end not included
	(The second member is the actual target)
	(The first member is an alias received from the uploader)
*/
string_pair upload_target_to_location_mappings[] = {
	
	{ "agarioSkin", "agarioSkins" },
	
	
	{ "uploadVideo", "videos/uploadedVideos" },
	
    /*
     
		FALLBACK TARGET
     
    */
	{ NULL, "upload" }
	
};
const char* get_upload_target(const char *target, int64_t* index_obtained){
    int64_t safeguard_ptr = 0;
    
    index_obtained = ((index_obtained!=NULL)?index_obtained:&safeguard_ptr);
	*index_obtained=-1;
    
    if (!target){
		return "upload";
	}
    for ( (*index_obtained) = 0;
		  upload_target_to_location_mappings[(*index_obtained)][0] != NULL;
		  (*index_obtained)++ ) {

        if (!strcmp(target, upload_target_to_location_mappings[(*index_obtained)][0])) {
            return upload_target_to_location_mappings[(*index_obtained)][1];
        }
    }
    *index_obtained=-1;
    return "upload";
}
