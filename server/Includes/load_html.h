#ifndef LOAD_HTML_H
#define LOAD_HTML_H

int isDirectory(char* resourceTarget);

char* generateDirListing(char* dir);

void deleteDirListingHTML(char* dir);

#endif
