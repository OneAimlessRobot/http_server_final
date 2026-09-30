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
    
    
    {".png", "image/png", 1,1},
    {".PNG", "image/png", 1,1},
    {".jpg", "image/jpeg", 1,1},
    {".jpeg", "image/jpeg", 1,1},
    {".webp", "image/webp", 1,1},
    {".gif", "image/gif", 1,1},
    {".jfif", "image/jpeg", 1,1},
    {".avif", "image/avif", 1,1},
    {".bmp", "image/bmp", 0,1},
    {".tga", "image/x-targa", 0,1},
    {".dds","image/x-dds", 1,1},
    {".vtf","image/x-vtf", 1,1},
    {".ico", "image/x-icon", 1,1},
    {".icon", "image/x-icon", 1,1},
    {".svg", "image/svg+xml", 1,1},
    
    /*
	 
	 AUDIO MIMETYPES
    
    */
    
    {".wav", "audio/wav", 0,1},
    {".webm", "audio/webm", 0,1},
    {".m4a", "audio/mp4", 0,1},
    {".mp3", "audio/mp3", 0,1},
    {".ogg","audio/ogg", 0,1},
    {".flac","audio/flac", 0,1},
      
    /*
	 
	 VIDEO MIMETYPES
    
    */
    
    {".mp4", "video/mp4", 0,1},
    {".flv","video/x-flv", 0,1},
    {".avi","video/x-msvideo",0,1},
    {".3gp","video/3gpp", 0,1},
    
    /*
     
		DOCUMENT MIMETYPES
     
    */
    
    
    {".html", "text/html", 1,0},
    {".css", "text/css", 1,0},
    {".yml", "text/yaml", 1,0},
    {".yaml", "text/yaml", 1,0},
    {".txt", "text/html", 0,1},
    {".sp", "text/x-sourcepawn", 1,0},
    {".sma", "text/x-sourcepawn", 1,0},
    {".inc", "text/x-sourcepawn", 1,0},
    {".cpp", "text/x-c++", 1,0},
    {".c", "text/x-c++", 1,0},
    {".h", "text/x-c++", 1,0},
    {".hpp", "text/x-cpp", 1,0},
    {".cfg", "text/x-config", 1,0},
    {".php", "text/html", 1,0},
    {".vdf", "text/plain", 1,0},
    {".gam", "text/plain", 1,0},
    {".py", "text/x-python", 1,0},
    {".sh","text/plain", 1,0},
    {".rcon","text/html", 1,0},
    {".find","text/html", 1,0},
    {".lst","text/plain", 1,0},
    {".db","text/plain", 1,0},
    {".md","text/markdown", 1,0},
    {".date", "text/html", 1,0},
    {".log", "text/plain", 1,0},
    {".vmt","text/x-vmt", 1,0},
    {".pl","text/plain", 1,0},
    {".java", "text/x-java-source", 1,0},
    
    /*
     
		APPLICATION MIMETYPES
     
    */
    
    {".js", "application/javascript", 1,0},
    {".wasm", "application/wasm", 1,0},
    {".webmanifest", "application/manifest+json", 1,0},
    {".json", "application/json", 1,0},
    {".wad", "application/x-doom-wad", 1,0},
    {".php", "application/x-httpd-php", 1,0},
    {".php2", "application/json", 1,0},
    {".pdf", "application/pdf", 1,0},
    {".rss", "application/rss+xml", 1,0},
    {".xml", "application/xml", 1,0},
    /*
     
		MISC MIMETYPES
     
    */
    
    {".mdl", "application/octet-stream", 0,1},
    {".spr", "application/octet-stream", 0,1},
    {".bsp", "application/octet-stream", 0,1},
    
    /*
     
		FALLBACK MIMETYPE
     
    */
    {NULL, "application/octet-stream", 0,1}
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
    return "application/octet-stream";
}

u_int8_t get_mime_type_compressibility(u_int64_t table_index) {
    return (mime_types[table_index].allows_compression);
}
u_int8_t get_mime_type_cacheability(u_int64_t table_index) {
    return (mime_types[table_index].to_be_cached);
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

string_pair upload_target_to_location_mappings[] = {
	
	{ "agarioSkins", "agarioSkins" },
	
    /*
     
		FALLBACK TARGET
     
    */
	{ NULL, "upload" }
	
};
/*
	slash at the end not included
*/
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

        if (strcmp(target, upload_target_to_location_mappings[(*index_obtained)][0]) == 0) {
            return upload_target_to_location_mappings[(*index_obtained)][1];
        }
    }
    return "upload";
}
