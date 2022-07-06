#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* macros */
#define THROW_SIZE_ERR(req)                                                   \
{                                                                     \
        fprintf(stderr, "Dest must hold atleast '%ld' chars\n", req); \
        exit(1);                                                      \
}

/* structs */
typedef struct {
        char foldercol[16];
        char filecol[16];
        char symlinkcol[16];
        char hardlinkcol[16];
        char binarycol[16];
} Theme;

/* enums */
typedef enum {
        SHELL,
        BASH,
        ZSH,
        FISH,
        ASH,
        BINARY,
        UNKNOWN
} ExeType;

/* global */
static short maxdepth = 15; /* default value */
static short maxroots = 100; /* default value */
static char dirnbuff[_POSIX_PATH_MAX] = {0};

/* config  */
#include "config.h"

/* function implementations */
        static void
chkdirname(char *dirname, unsigned int len)
{
        if (dirname[len - 1] == '/') { return; }

        dirname[len] = '/';
        dirname[len + 1] = '\0';
}

        static void
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

        static void
printbranch(const char *text, int level, const char *col)
{
        for (int i = 0; i < level; i++) {
                printf("   ");
        }

        char *reset = "\033[0m";

        printf("%s%s%s%s%s\n", vert, hori, col, text, reset);
}

        static ExeType
getexetype(const char *path)
{
        FILE *fp = fopen(path, "r");
        if (!fp) {
                fprintf(stderr, "Cannot open %s\n", path);
        }

        char lineBuff[256];
        ExeType exeType = UNKNOWN;

        fgets(lineBuff, sizeof(lineBuff), fp);

        if (strstr(lineBuff, "#!/bin/bash")) {
                exeType = BASH;
        }
        if (strstr(lineBuff, "#!/bin/ash")) {
                exeType = ASH;
        }
        if (strstr(lineBuff, "#!/bin/zsh")) {
                exeType = ZSH;
        }
        if (strstr(lineBuff, "ELF")) {
                exeType = BINARY;
        }

        fclose(fp);
        return exeType;
}

        static void
analysedir(const char *dirname, int level)
{
        if (level >= maxdepth) return;

        struct dirent *dir;
        DIR *dp = opendir(dirname);
        if (!dp) {
                fprintf(stderr, "Cannot open dir%s\n", dirname);
                return;
        }
        int dirlvl;

        while ((dir = readdir(dp)) != NULL) {
                if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
                        continue;
                }

                switch (dir->d_type) {
                        case DT_DIR:
                                printbranch(dir->d_name, level, theme.foldercol);
                                dirlvl = level;
                                char recdirnbuff[_POSIX_PATH_MAX];
                                strnjoin(dirname, dir->d_name, recdirnbuff,
                                                sizeof(recdirnbuff));
                                chkdirname(recdirnbuff, strlen(recdirnbuff));
                                analysedir(recdirnbuff, ++dirlvl);
                                break;
                        case DT_REG:
                                printbranch(dir->d_name, level, theme.filecol);
                                break;
                        case DT_LNK:
                                printbranch(dir->d_name, level, theme.symlinkcol);
                                break;
                        case DT_FIFO:
                                printbranch(dir->d_name, level, theme.symlinkcol);
                                break;
                        case DT_BLK:
                                printbranch(dir->d_name, level, theme.filecol);
                                break;
                        case DT_CHR:
                                printbranch(dir->d_name, level, theme.filecol);
                                break;
                        case DT_SOCK:
                                printbranch(dir->d_name, level, theme.filecol);
                                break;
                        case DT_UNKNOWN:
                                printbranch(dir->d_name, level, theme.filecol);
                                break;
                }
        }
        closedir(dp);
}

        static void
analysecurrdir()
{
        getcwd(dirnbuff, 256);
        chkdirname(dirnbuff, strlen(dirnbuff));
        analysedir(dirnbuff, 0);
}

        static void
showhelp()
{
        printf("\
                        Usage: stree [options]\n\
                        Options:\n\
                        <path>                  Directory to display\n\
                        -h                      Show help menu\n\
                        -d <int>                Folder depth\n\
                        -n <int>                Amount of roots\n");
}

        int
main(int argc, char *argv[])
{
        char opt;
        while ((opt = getopt(argc, argv, "d:cshp:n:")) != -1) {
                switch (opt) {
                        /*
                         * Dont directly call methods from here to allow multiple args
                         *
                         */
                        case 'd':
                                maxdepth = *optarg - '0';
                                break;
                        case 'h':
                                showhelp();
                                return 0;
                        case 'n':
                                maxroots = *optarg - '0';
                                break;
                        case '?': return 1;
                        case ':': return 1;
                        default: return 1;
                }
        }
        analysedir(argv[optind], 0);
        return 0;
}

