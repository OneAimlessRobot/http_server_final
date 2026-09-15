#ifndef CUSTOM_ENUMS_H
#define CUSTOM_ENUMS_H

typedef enum {GET,HEAD,POST,INVALID,MAX_REQUEST_TYPE} http_req_type;

typedef enum {SIZE, FULL_DATE,FILE_NAME,CATEGORY_ID_MAX}category_id;


typedef enum {ATOM=0,XHTML,MAX_NAMESPACE_URI_ID}namespace_uri_id;

typedef enum {TITLE=0,DESC,CAT,RSS_DATE,LINK,MAX_TAG_ID}tag_id;

typedef enum {CH_TITLE=0,CH_LINK,CH_LANGUAGE,CH_UPDATE_PERIOD,CH_UPDATE_FREQUENCY,CH_DESCRIPTION,CH_DATE,CH_GENERATOR,MAX_CH_TAG_ID}channel_tag_id;

typedef enum {ADDR=0,PORT,PASS,CMD,MAX_NUM_RCON_ARGS}rcon_args;

typedef enum {REGEXP=0,SHOULD_WE_AUTOREDIRECT,MAX_NUM_FIND_ARGS}find_args;

#endif
