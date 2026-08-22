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
    {".html", "text/html", 1},
    {".css", "text/css", 1},
    {".js", "application/javascript", 1},
    {".png", "image/png", 1},
    {".PNG", "image/png", 1},
    {".jpg", "image/jpeg", 1},
    {".jpeg", "image/jpeg", 1},
    {".webp", "image/webp", 1},
    {".mp4", "video/mp4", 0},
    {".yml", "text/yaml", 1},
    {".yaml", "text/yaml", 1},
    {".mp3", "audio/mp3", 0},
    {".wasm", "application/wasm", 1},
    {".webmanifest", "application/manifest+json", 1},
    {".gif", "image/gif", 1},
    {".jfif", "image/jpeg", 1},
    {".avif", "image/avif", 1},
    {".java", "text/x-java-source", 1},
    {".ico", "image/x-icon", 1},
    {".icon", "image/x-icon", 1},
    {".json", "application/json", 1},
    {".svg", "image/svg+xml", 1},
    {".txt", "text/html", 1},
    {".webm", "audio/webm", 0},
    {".wav", "audio/wav", 0},
    {".m4a", "audio/mp4", 0},
    {".sp", "text/x-sourcepawn", 1},
    {".sma", "text/x-sourcepawn", 1},
    {".inc", "text/x-sourcepawn", 1},
    {".cpp", "text/x-c++", 1},
    {".c", "text/x-c++", 1},
    {".h", "text/x-c++", 1},
    {".hpp", "text/x-cpp", 1},
    {".cfg", "text/x-config", 1},
    {".php", "text/html", 1},
    {".vdf", "text/plain", 1},
    {".gam", "text/plain", 1},
    {".py", "text/x-python", 1},
    {".date", "text/plain", 1},
    {".bmp", "image/bmp", 1},
    {".log", "text/plain", 1},
    {".tga", "image/x-targa", 1},
    {".tga", "image/x-targa", 1},
    {".dds","image/x-dds", 1},
    {".vtf","image/x-vtf", 1},
    {".vmt","text/x-vmt", 1},
    {".ogg","audio/ogg", 0},
    {".flv","video/x-flv", 0},
    {".flac","audio/flac", 0},
    {".avi","video/x-msvideo", 0},
    {".pl","text/plain", 1},
    {".wad","application/x-doom-wad", 1},
    {".lst","text/plain", 1},
    {".db","text/plain", 1},
    {".md","text/markdown", 1},
    {".php","application/x-httpd-php", 1},
    {".php2","application/json", 1},
    {".pdf","application/pdf", 1},
    {".sh","text/plain", 1},
    {".rss","application/rss+xml", 1},
    {".rcon","text/html", 1},
    {".find","text/html", 1},
    {".xml","application/xml", 1},
    {".3gp","video/3gpp", 0},
    {NULL, "application/octet-stream", 1}
};

const char* get_mime_type(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot){
		return "application/octet-stream";
	}
    for (int i = 0; mime_types[i].extension != NULL; i++) {
        if (strcmp(dot, mime_types[i].extension) == 0) {
            return mime_types[i].mime_type;
        }
    }
    return "application/octet-stream";
}

u_int8_t get_mime_type_compressibility(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot){
		return 1;
	}
    for (int i = 0; mime_types[i].extension != NULL; i++) {
        if (strcmp(dot, mime_types[i].extension) == 0) {
            return mime_types[i].allows_compression;
        }
    }
    return 1;
}
