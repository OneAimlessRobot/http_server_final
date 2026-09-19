#ifndef TEMPLATE_FUNCS_H
#define TEMPLATE_FUNCS_H



#define STYLESHEET_FORMAT "<!DOCTYPE html>" \
			"\n<html>" \
			"\n<head>" \
			"\n<meta charset=\"UTF-8\">\n" \
			"\n<base href=\'\'>" \
			"\n<link rel=\"icon\" type=\"image/png\" sizes=\"32x32\" href=\"/icons/.favicon-32x32.png\">" \
			"\n<link rel=\"icon\" type=\"image/png\" sizes=\"16x16\" href=\"/icons/.favicon-16x16.png\">" \
			"\n<link rel='stylesheet' type='text/css' href='" \
			DIR_LISTING_CSS_FILE_PATH \
			"'>" \
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
			"\n<link rel=\"manifest\" href=\"" \
			WEBMANIFEST_FILE_PATH \
			"\">" \
			"\n</head>"
			
			
#define TXT_HTML_PREFIX "<!DOCTYPE html>" \
			"\n<html>" \
			"\n<head>" \
			"\n<meta charset=\"UTF-8\">\n" \
			"\n<base href=\'\'>" \
			"\n<link rel=\"icon\" type=\"image/png\" sizes=\"32x32\" href=\"/icons/.favicon-32x32.png\">" \
			"\n<link rel=\"icon\" type=\"image/png\" sizes=\"16x16\" href=\"/icons/.favicon-16x16.png\">" \
			"\n<link rel='stylesheet' type='text/css' href='" \
			TXT_PRETTIFY_CSS_FILE_PATH \
			"'>" \
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
			"\n<link rel=\"manifest\" href=\"" \
			WEBMANIFEST_FILE_PATH \
			"\">" \
			"\n</head>"

#define FIND_HTML_PREFIX "<!DOCTYPE html>" \
			"\n<html>" \
			"\n<head>" \
			"\n<meta charset=\"UTF-8\">\n" \
			"\n<base href=\'\'>" \
			"\n<link rel=\"icon\" type=\"image/png\" sizes=\"32x32\" href=\"/icons/.favicon-32x32.png\">" \
			"\n<link rel=\"icon\" type=\"image/png\" sizes=\"16x16\" href=\"/icons/.favicon-16x16.png\">" \
			"\n<link rel='stylesheet' type='text/css' href='" \
			FIND_LISTING_CSS_FILE_PATH \
			"'>" \
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
			"\n<link rel=\"manifest\" href=\"" \
			WEBMANIFEST_FILE_PATH \
			"\">" \
			"\n</head>"
			
#define LOAD_DATE_PREFIX "<!DOCTYPE html>" \
			"\n<html>" \
			"\n<head>" \
			"\n<meta charset=\"UTF-8\">\n" \
			"\n<base href=\'\'>" \
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
			"\n</head>\n<body>\n" \
			"<script>\nwindow.setTimeout( function() {\n" \
			"window.location.reload();\n" \
			"}, %lu);\n</script>\n" \
			"<div>%s</div>\n<br>\n" \
			"</body>" \
			"\n</html>\n"
			
			
#endif
