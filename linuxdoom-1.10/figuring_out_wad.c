#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <alloca.h>
#include <stdio.h>
#define O_BINARY    0

int filelength (int handle) 
{ 
    struct stat	fileinfo;
    
    if (fstat (handle,&fileinfo) == -1) {
        printf("Failed\n");
        return -1;
    }

    return fileinfo.st_size;
}


// this is how a WAD file begins
// first four bytes are for IWAD
// 0x49 0x57 0x41 0x44
// then 4 bytes for numlumps
// and 4 bytes for infotableofs
typedef struct
{
    // Should be "IWAD" or "PWAD".
    char		identification[4];		
    int			numlumps;
    int			infotableofs;
    
} wadinfo_t;


typedef struct
{
    int			filepos;
    int			size;
    char		name[8];
    
} filelump_t;

int main(void)
{
    wadinfo_t header;
    filelump_t *fileinfo;

    char *filename = "doom1.wad";
    char *filename2 = "doom2.wad";
    int handle = open(filename,O_RDONLY);
    int handle2 = open(filename2,O_RDONLY);

    printf("file length: %d\n", filelength(handle));

    // first is smaller than the second
    printf("HANDLE1: %d AND HANDLE2: %d\n", handle, handle2);
    
    read(handle, &header, sizeof(header));
    printf("IDENTIFICATION: %s \n", header.identification);
    printf("NUMLUMPS: %d\n", header.numlumps);
    printf("INFOTABLEOFS: %d\n", header.infotableofs);

    printf("Size of one info about lump: %d\n", sizeof(filelump_t));
    printf("File length - infotable position: %d\n", (filelength(handle) - header.infotableofs));
    printf("Size of * numlumps: %d\n", sizeof(filelump_t) * header.numlumps);
    
    int length = filelength(handle);
    int tablelength = header.numlumps * sizeof(filelump_t);
    fileinfo = malloc(tablelength);
    lseek(handle, header.infotableofs, SEEK_SET);
    read(handle, fileinfo, length);

    for (int i = 0; i < header.numlumps; i = i + sizeof(filelump_t)) {
        printf("name is: %s\n", fileinfo[i].name);
        printf("position is: %d\n", fileinfo[i].filepos);
        printf("size is: %d\n", fileinfo[i].size);
    }

    return 0;
}