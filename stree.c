#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxdepth 4
#define THROW_SIZE_ERR(req)                                                   \
{                                                                     \
    fprintf(stderr, "Dest must hold atleast '%ld' chars\n", req); \
    exit(1);                                                      \
}

/* Example output
   |_ Folder
   |    |_ File
   |
   |_ File
   |
   |_Folder
   |_Folder
   |_File
   |_File
   |_Folder
   |_File
   */

    void
strnjoin(const char *first, const char *sec, char *dest, int destsize)
{
    unsigned long firstlen = strlen(first);
    unsigned long seclen = strlen(sec);

    if (firstlen + seclen + 1 > destsize) {
        THROW_SIZE_ERR(firstlen + seclen + 1);
    }

    strncpy(dest, first, destsize);
    int rest = destsize - firstlen - 1;
    strncat(dest, sec, rest);
    dest[firstlen + seclen] = '\0';
}

typedef struct {
    char foldercol[16];
    char filecol[16];
    char symlinkcol[16];
    char hardlinkcol[16];
    char binarycol[16];
} Theme;

   static void
analysedir(const char *dirname, int level)
{
    struct dirent *dir;
    DIR *dp = opendir(dirname);

    while ((dir = readdir(dp)) != NULL) {
        if (strstr(dir->d_name, ".") || strstr(dir->d_name, "..")) {
            continue;
        }

        switch (dir->d_type) {
            case DT_DIR:
                /* Scan dir again and print before continuing
                 * pass level
                 */ 
                printf("%s is a dir of dir %s\n", dir->d_name, dirname);
                char fulldirname[256 * maxdepth];
                strnjoin(dirname, dir->d_name, fulldirname, sizeof(fulldirname));
                analysedir(fulldirname, level++);
                break;
            case DT_REG:
                printf("%s is a file of dir %s\n", dir->d_name, dirname);
                break;
            case DT_LNK:
                printf("%s is a symlink\n", dir->d_name);
                break;
            case DT_FIFO:
                printf("%s is a named pipe\n", dir->d_name);
                break;
            case DT_BLK:
                printf("%s is a block dev\n", dir->d_name);
                break;
            case DT_CHR:
                printf("%s is a char dev\n", dir->d_name);
                break;
            case DT_SOCK:
                printf("%s is a socket\n", dir->d_name);
                break;
            case DT_UNKNOWN:
                printf("%s is unknown\n", dir->d_name);
                break;
        }
    }
    closedir(dp);
}

    void
showhelp()
{
    printf("\
            Usage: stree [options]\n\
            Options:\n\
            -d <number>             Folder depth\n\
            -c                      Disable color\n\
            -s                      Show file size\n\
            -p <string>             Directory path\n");
}

    int
main(int argc, char *argv[])
{
#include "config.h"
    printf("%shallo\033[0m\n", theme.foldercol);

    if (argc == 1) {
        printf("do stuff w/o options\n");
        exit(1);
    }

    int opt;
    while ((opt = getopt(argc, argv, "d:cshp:")) != 1) {
        switch (opt) {
            case 'd':
                printf("Option d has arg: %s\n", optarg);
                break;
            case 'c':
                break;
            case 's':
                break;
            case 'h':
                showhelp();
                exit(0);
            case 'p':
                analysedir(optarg, 0);
                exit(0);
            case '?':
                printf("Unkown option: %c\n", optopt);
                exit(1);
            case ':':
                printf("Missing arg for %c\n", optopt);
                exit(1);
            default:
                printf("%s\n", argv[1]);
                exit(0);
        }
    }

    return 0;
}
