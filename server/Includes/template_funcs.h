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

/*
* https://stackoverflow.com/questions/72263107/how-do-i-use-markdown-it-without-node-js
*
*/

/*
 * 

#define TXT_HTML_PREFIX "<!DOCTYPE html>" \
			"\n<html>" \
			"\n<head>" \
			"\n<meta charset=\"UTF-8\">\n" \
			"\n<base href=\'\'>" \
			"\n<script src=\"%s\"></script>" \
			"\n<link rel='stylesheet' type='text/css' href='%s'>" \
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
			"\n<link rel=\"manifest\" href=\"/webcrookedstuff/.site.webmanifest\">" \
			"\n<script>\n" \
			"\nwindow.onload = function() {" \
			"\nvar md = window.markdownit();" \
			"\nvar div = document.getElementsByClassName('markdown');" \
			"\nfor(var i = 0; i < div.length; i++) {" \
			"\n	var content = div[i].innerHTML;" \
			"\n	document.getElementsByClassName('markdown')[i].innerHTML = md.render(content);" \
			"\n }" \
			"\n}\n" \
			"</script>" \
			"\n</head>"

*/


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
			
#endif