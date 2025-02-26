#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <alloca.h>
#include <stdio.h>
#include <string.h>
#define O_BINARY    0

int W_CheckNumForName (char* name);



int filelength (int handle) 
{ 
    struct stat	fileinfo;
    
    if (fstat (handle,&fileinfo) == -1) {
        printf("Failed\n");
        return -1;
    }

    return fileinfo.st_size;
}

typedef struct
{
    char	name[8];
    int		handle;
    int		position;
    int		size;
} lumpinfo_t;

int numlumps;
lumpinfo_t *lumpinfo;
int startlump = 0;






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
    numlumps = header.numlumps;
    int tablelength = header.numlumps * sizeof(filelump_t);
    fileinfo = malloc(tablelength);
    lseek(handle, header.infotableofs, SEEK_SET);
    read(handle, fileinfo, length);
    int y = 0;

    // print the first 10 
    for (int i = 0; i < 10; i++) {
        printf("name is: %s\n", fileinfo[i].name);
        printf("position is: %d\n", fileinfo[i].filepos);
        printf("size is: %d\n", fileinfo[i].size);
        y = i;
    }
    printf("Value of I is:%d\n", y);


    // Fill in lumpinfo
    lumpinfo = malloc (numlumps*sizeof(lumpinfo_t));

    lumpinfo_t *lump_p;
    lump_p = &lumpinfo[startlump];
	
    int storehandle = handle;
	
    // if i understood correctly,
    // we only read the info table into memory
    for (int i=startlump ; i<numlumps ; i++,lump_p++, fileinfo++)
    {
	lump_p->handle = storehandle;
	lump_p->position = (fileinfo->filepos);
	lump_p->size = (fileinfo->size);
	strncpy (lump_p->name, fileinfo->name, 8);
    }
    
    // should be 1 if working properly
    printf("We found COLORMAP: %d\n", W_CheckNumForName("COLORMAP"));
    int lump = W_CheckNumForName("COLORMAP");
    printf("SIZE OF THAT LUMP IS: %d\n", lumpinfo[lump].size);
    return 0;
}



int W_CheckNumForName (char* name)
{
    union {
	char	s[9];
	int	x[2];
	
    } name8;
    
    int		v1;
    int		v2;
    lumpinfo_t*	lump_p;

    // make the name into two integers for easy compares
    strncpy (name8.s,name,8);

    // in case the name was a fill 8 chars
    name8.s[8] = 0;



    v1 = name8.x[0];
    v2 = name8.x[1];


    // scan backwards so patch lump files take precedence
    lump_p = lumpinfo + numlumps;

    while (lump_p-- != lumpinfo)
    {
	if ( *(int *)lump_p->name == v1
	     && *(int *)&lump_p->name[4] == v2)
	{
        // because of pointer arithmetic
        // this gives the number of elements
        // not the number of bytes
	    return lump_p - lumpinfo;
	}
    }

    // TFB. Not found.
    return -1;
}