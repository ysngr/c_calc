/* scan.h */
#ifndef SCAN_H
#define SCAN_H

#define KEYWORDNUM 7

struct key{
    int keynum;
    char keyname[MAXSTRLEN];
};

extern struct key ks[KEYWORDNUM];

int is_invalid_char(char);
int str_to_tokennum(char*);

#endif
