#ifndef TEMPLATE_FUNCS_H
#define TEMPLATE_FUNCS_H

#define STYLESHEET_FORMAT "<!DOCTYPE html>" \
			"\n<html>" \
			"\n<head>" \
			"\n<meta charset=\"UTF-8\">\n" \
			"\n<base href=\'\'>" \
			"\n<link rel='stylesheet' type='text/css' href='%s'>" \
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
			"\n<link rel=\"manifest\" href=\"/webcrookedstuff/.site.webmanifest\">" \
			"\n</head>"

#define TXT_HTML_PREFIX "<!DOCTYPE html>" \
			"\n<html>" \
			"\n<head>" \
			"\n<meta charset=\"UTF-8\">\n" \
			"\n<base href=\'\'>" \
			"\n<link rel='stylesheet' type='text/css' href='%s'>" \
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
			"\n<link rel=\"manifest\" href=\"/webcrookedstuff/.site.webmanifest\">" \
			"\n</head>"

#define FIND_HTML_PREFIX "<!DOCTYPE html>" \
			"\n<html>" \
			"\n<head>" \
			"\n<meta charset=\"UTF-8\">\n" \
			"\n<base href=\'\'>" \
			"\n<link rel='stylesheet' type='text/css' href='%s'>" \
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
			"\n<link rel=\"manifest\" href=\"/webcrookedstuff/.site.webmanifest\">" \
			"\n</head>"
			
/*
window.setTimeout( function() {
  window.location.reload();
}, 30000);
*/
#define LOAD_DATE_PREFIX "<!DOCTYPE html>" \
			"\n<html>" \
			"\n<head>" \
			"\n<meta charset=\"UTF-8\">\n" \
			"\n<base href=\'\'>" \
			"\n<link rel='stylesheet' type='text/css' href=''>" \
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
			"\n<link rel=\"manifest\" href=\"/webcrookedstuff/.site.webmanifest\">" \
			"\n</head><br>\n<body>\n" \
			"<script>\nwindow.setTimeout( function() {\n" \
			"window.location.reload();\n" \
			"}, %lu);\n</script>\n" \
			"<div>%s</div>\n<br>\n" \
			"</body><br>" \
			"\n</html>\n"
#endif
