#include <00ways/cds.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char recordTerminator[2] = "\r\n";

struct cdsRecord {
    char filename[16];
    long length;
    struct cdsRecord* next;
};
struct cdsmap {
    const char* filename;
    unsigned long recordCount;
    struct cdsRecord* recordsStack;
};

static struct cdsRecord* readRecord(FILE* fptr) {
    struct cdsRecord* record = malloc(sizeof(struct cdsRecord));
    fread(record->filename, sizeof(char), 16, fptr);
    fread(&record->length, sizeof(long), 1, fptr);
    // printf("filename: %s\n", );
    record->next = NULL;
    return record;
}
cdsmap* cdsMapFile(const char* filename) {
    FILE* fptr = fopen(filename, "rb");
    if (fptr == NULL) {
        return NULL;
    }
    unsigned long recordCount = 0;
    fread(&recordCount, sizeof(unsigned long), 1, fptr);
    if (recordCount == 0) {
        perror("No records found");
        fclose(fptr);
        return NULL;
    }
    int current = 0;
    struct cdsRecord* record = NULL;
    struct cdsRecord** cursor = &record;
    while(current++ < recordCount) {
        *cursor = readRecord(fptr);
        cursor = &(*cursor)->next;
        printf("read %d\n", current);
    }
    *cursor = NULL;
    cdsmap* map = malloc(sizeof(cdsmap));
    map->filename = filename;
    map->recordsStack = record;
    map->recordCount = recordCount;
    fclose(fptr);
    return map;
}
cdsmap* cdsMapCreate(const char* filename, int recordcount) {
    cdsmap* map = malloc(sizeof(struct cdsmap));
    map->filename = filename;
    map->recordCount = recordcount;
    map->recordsStack = NULL;
    return map;
}
void cdsMapAddRecord(cdsmap* map, char filename[16], long length) {
    struct cdsRecord** current = &map->recordsStack;
    long offset = 0;
    while (*current != NULL) {
        current = &(*current)->next;
    }
    *current = malloc(sizeof(struct cdsRecord));
    memcpy((*current)->filename, filename, 16);
    (*current)->length = length;
    (*current)->next = NULL;
}
void cdsCloseMap(cdsmap* map) {
    if (!map) return;

    struct cdsRecord* cursor = map->recordsStack;
    while (cursor != NULL) {
        struct cdsRecord* next = cursor->next;
        free(cursor);
        cursor = next;
    }
    free(map);
}
char* cdsMapReadFile(cdsmap* map, char filename[16]) {
    long headersize =
         map->recordCount 
         * (sizeof(struct cdsRecord) - sizeof(void*))
         + sizeof(unsigned long);
    struct cdsRecord* cursor = map->recordsStack;
    long offset = 0;
    long length = 0;
    while (cursor != NULL) {
        // sprintf(buffer, "%s\n%s %ld + %ld", buffer, cursor->filename, cursor->location, cursor->length);
        if (strcmp(cursor->filename, filename) == 0) {
            length = cursor->length;
            break;
        }
        else {
            offset += cursor->length;
        }
        cursor = cursor->next;
    }
    if (offset == 0 && length == 0) {
        return NULL;
    }
    offset += headersize;
    FILE* fstream = fopen(map->filename, "rb");
    fseek(fstream, offset, SEEK_SET);
    char* buffer = calloc(length + 1, sizeof(char));
    fread(buffer, length, sizeof(char), fstream);
    buffer[length] = 0x00;
    return buffer;
}
static int cdsMapWriteFileRecord(FILE* fptr, struct cdsRecord* record) {
    if (fwrite(record->filename, sizeof(char), 16, fptr) < 16) return 1;
    if (fwrite(&record->length, sizeof(long), 1, fptr) < 1) return 1;
    return 0;
}
int cdsMapWriteFileHeader(cdsmap* map, FILE** pfptr) {
    FILE* fstream = fopen(map->filename, "wb");
    if (fstream == NULL) {
        printf("Unable to write file\n");
        return 1;
    }
    printf("[write] recordCount: %ld\n", map->recordCount);
    fwrite(&map->recordCount, sizeof(long), 1, fstream);
    struct cdsRecord* current = map->recordsStack;
    while (current != NULL) {
        cdsMapWriteFileRecord(fstream, current);
        current = current->next;
    }
    *pfptr = fstream;
    return 0;
}
char* cdsMapToString(cdsmap* map) {
    struct cdsRecord* cursor = map->recordsStack;
    char* buffer = calloc(map->recordCount * (1 + 16 + 1 + 8 + 3 + 8), sizeof(char));
    long offset = 0;
    while (cursor != NULL) {
        sprintf(buffer, "%s\n%s %ld + %ld", buffer, cursor->filename, offset, cursor->length);
        offset += cursor->length;
        cursor = cursor->next;
    }
    return buffer;
}
