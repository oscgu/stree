#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
        char foldercol[16];
        char filecol[16];
        char symlinkcol[16];
        char hardlinkcol[16];
        char binarycol[16];
} Theme;

static unsigned char depth = 4;

static void readdir(const char *dirname);

void
showhelp()
{
        printf("\
    Usage: stree [options]\n\
    Options:\n\
        -d <number>             Folder depth\n\
        -c                      Disable color\n\
        -s                      Show file size\n");
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
        while ((opt = getopt(argc, argv, "d:csh")) != 1) {
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
