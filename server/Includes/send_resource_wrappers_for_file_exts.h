#ifndef STYLE_EXTENDER_H
#define STYLE_EXTENDER_H

typedef struct comp_result{
	int comp_enable,fd;
	FILE* stream;

}comp_result;

static void push_bytes(client* c, comp_result* result, int is_partial, int start_point);


static char* compressionCmd = "%s -c -%d %s";

static char inpathbuff[PATHSIZE*6]={0};
static char outpathbuff[PATHSIZE*6+10]={0};
static char cmd[PATHSIZE*30]={0};


typedef struct {
    const char *extension;
    const char *mime_type;

} MimeEntry;

static MimeEntry mime_types[] = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".PNG", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".webp", "image/webp"},
    {".mp4", "video/mp4"},
    {".yml", "text/yaml"},
    {".yaml", "text/yaml"},
    {".mp3", "audio/mp3"},
    {".wasm", "application/wasm"},
    {".webmanifest", "application/manifest+json"},
    {".gif", "image/gif"},
    {".jfif", "image/jpeg"},
    {".avif", "image/avif"},
    {".java", "text/x-java-source"},
    {".ico", "image/x-icon"},
    {".icon", "image/x-icon"},
    {".json", "application/json"},
    {".svg", "image/svg+xml"},
    {".txt", "text/markdown"},
    {".webm", "audio/webm"},
    {".wav", "audio/wav"},
    {".m4a", "audio/mp4"},
    {".sp", "text/x-sourcepawn"},
    {".sma", "text/x-sourcepawn"},
    {".inc", "text/x-sourcepawn"},
    {".cpp", "text/x-c++"},
    {".c", "text/x-c++"},
    {".h", "text/x-c++"},
    {".hpp", "text/x-cpp"},
    {".cfg", "text/x-config"},
    {".php", "text/html"},
    {".vdf", "text/plain"},
    {".gam", "text/plain"},
    {".py", "text/x-python"},
    {".bmp", "image/bmp"},
    {".log", "text/plain"},
    {".tga", "image/x-targa"},
    {".tga", "image/x-targa"},
    {".dds","image/x-dds"},
    {".vtf","image/x-vtf"},
    {".vmt","text/x-vmt"},
    {".ogg","audio/ogg"},
    {".flv","video/x-flv"},
    {".flac","audio/flac"},
    {".avi","video/x-msvideo"},
    {".pl","text/plain"},
    {".wad","application/x-doom-wad"},
    {".lst","text/plain"},
    {".db","text/plain"},
    {".md","text/markdown"},
    {".php","application/x-httpd-php"},
    {".php2","application/json"},
    {".pdf","application/pdf"},
    {".sh","text/plain"},
    {".rss","application/rss+xml"},
    {".rcon","text/html"},
    {".xml","application/xml"},
    {".3gp","video/3gpp"},
    {NULL, "application/octet-stream"}
};

static const char* get_mime_type(const char *filename) {
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
void send_wrapper(client* c,char* buff, int buff_size) {
	if((c->is_tls?SSL_write(c->ctx_if_ssl,buff,buff_size):send(c->socket,buff,buff_size,0))!=(buff_size)){
		if(logging>5){
			fprintf(logstream,"ERRO NO SEND!!%s\n",strerror(errno));
		}
	}	
}

static void push_bytes(client* c, comp_result* result, int is_partial, int start_point){
	
	(use_fd)?
		((use_chunked)?
			sendallchunkedfd(c,result->fd,is_partial?start_point:0)
			:
			sendallfd(c,result->fd,is_partial?start_point:0)
			
		):
		((use_chunked)?
			sendallchunkedstream(c,result->stream,is_partial?start_point:0)
			:
			sendallstream(c,result->stream,is_partial?start_point:0)
		);
	
}

static void final_cleanup(comp_result* result){
	
	close(result->fd);
	if(result->stream!=NULL){
		fclose(result->stream);
	}

	printf("Trying to delete %s...\n",outpathbuff);
	if(result->comp_enable){
		remove(outpathbuff);
	}

}

	
#endif