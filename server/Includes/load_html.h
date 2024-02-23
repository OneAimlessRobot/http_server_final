#ifndef LOAD_HTML_H
#define LOAD_HTML_H

char* generateDirListing(char* dir);

void generateClientListing(char targetinout[PATHSIZE]);

void deleteDirListingHTML(void);

void deleteClientListingHTML(void);

#endif
