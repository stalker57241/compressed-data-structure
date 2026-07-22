#ifndef __00ways_cds_h
#define __00ways_cds_h

typedef struct _IO_FILE FILE;
typedef struct cdsmap cdsmap;

cdsmap* cdsMapFile(const char* filename);
cdsmap* cdsMapCreate(const char* filename, int recordcount);
void cdsMapAddRecord(cdsmap* map, char filename[16], long length);
void cdsCloseMap(cdsmap* map);

char* cdsMapToString(cdsmap* map);
char* cdsMapReadFile(cdsmap* map, char filename[16]);
int cdsMapWriteFileHeader(cdsmap* map, FILE** fptr);

#endif