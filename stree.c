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

/* global */
static int maxdepth = 15;  /* default value */
static int maxroots = 100; /* default value */
static char dirnbuff[_POSIX_PATH_MAX] = {0};
static char *ignore[10] = {0};
static int ignoreCount = 0;

/* config  */
#include "config.h"

/* function implementations */
static int
isIgnored(char *name)
{
        if (!ignoreCount) return 0;
        int i;

        for (i = 0; i < ignoreCount; i++) {
                if (!strcmp(ignore[i], name)) { return 1; }
        }

        return 0;
}

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
        int rootcnt = 0;

        while ((dir = readdir(dp)) != NULL) {
                if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") || isIgnored(dir->d_name)) {
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
                if (++rootcnt == maxroots) { return; }
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
                        -d <num>                Folder depth\n\
                        -r <num>                Amount of roots\n\
                        -i <num>                Ignore files (max. 10)\n");
}

int
main(int argc, char *argv[])
{
        char opt;

        while ((opt = getopt(argc, argv, "d:cshr:i:")) != -1) {
                switch (opt) {
                case 'h':
                        showhelp();
                        return 0;
                case 'd':
                        maxdepth = *optarg - '0';
                        break;
                case 'r':
                        maxroots = *optarg - '0';
                        break;
                case 'i':
                        ignore[ignoreCount++] = optarg;
                        break;
                case '?':
                        return 1;
                case ':':
                        return 1;
                default:
                        return 1;
                }
        }

        for (; optind < argc; optind++) {
                strncpy(dirnbuff, argv[optind], sizeof(dirnbuff));
                chkdirname(dirnbuff, strlen(dirnbuff));
                analysedir(dirnbuff, 0);
                return 0;
        }

        analysecurrdir();

        return 0;
}
