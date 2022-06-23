#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct {
        char foldercol[16];
        char filecol[16];
        char symlinkcol[16];
        char hardlinkcol[16];
        char binarycol[16];
} Theme;

static unsigned char depth = 4;

static void
analysedir(const char *dirname, int level)
{
        struct dirent *dir;
        DIR *dp = opendir(dirname);

        while ((dir = readdir(dp))) {
                if (strstr(dir->d_name, ".") || strstr(dir->d_name, ".."))
                        continue;

                switch (dir->d_type) {
                case DT_DIR:
               /* Scan dir again and print before continuing
                * pass level
                */ 
                        printf("%s is a dir\n", dir->d_name);
                        break;
                case DT_REG:
                        printf("%s is a file\n", dir->d_name);
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
        -p <string>             Directory path");
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
