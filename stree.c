#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define THROW_SIZE_ERR(req)                                                   \
        {                                                                     \
                fprintf(stderr, "Dest must hold atleast '%ld' chars\n", req); \
                exit(1);                                                      \
        }

typedef struct {
        char foldercol[16];
        char filecol[16];
        char symlinkcol[16];
        char hardlinkcol[16];
        char binarycol[16];
} Theme;

short maxdepth = 15;

/* config  */
#include "config.h"

static void
checkdirname(char *dirname, unsigned int len)
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
printbranch(const char *text, int level, int type)
{
        for (int i = 0; i < level; i++) {
                printf("   ");
        }

        char *col = "\033[0m";
        char *reset = "\033[0m";

        switch (type) {
        case DT_DIR:
                col = theme.foldercol;
                break;
        case DT_REG:
                col = theme.filecol;
                break;
        case DT_LNK:
                col = theme.symlinkcol;
                break;
        case DT_FIFO:
                col = theme.symlinkcol;
                break;
        case DT_BLK:
                /* Fallthrough  */
        case DT_CHR:
                /* Fallthrough  */
        case DT_SOCK:
                /* Fallthrough  */
        case DT_UNKNOWN:
                break;
        }
        printf("%s%s%s%s%s\n", vert, hori, col, text, reset);
}

static void
analysedir(const char *dirname, int level)
{
        if (level >= maxdepth) return;

        struct dirent *dir;
        DIR *dp = opendir(dirname);
        int dirlvl;

        while ((dir = readdir(dp)) != NULL) {
                if (strstr(dir->d_name, ".") || strstr(dir->d_name, "..")) {
                        continue;
                }

                switch (dir->d_type) {
                case DT_DIR:
                        dirlvl = level;
                        printbranch(dir->d_name, level, DT_DIR);

                        char fulldirname[1024];
                        strnjoin(dirname, dir->d_name, fulldirname,
                                 sizeof(fulldirname));

                        checkdirname(fulldirname, strlen(fulldirname));
                        analysedir(fulldirname, ++dirlvl);
                        break;
                case DT_REG:
                        printbranch(dir->d_name, level, DT_REG);
                        break;
                case DT_LNK:
                        printbranch(dir->d_name, level, DT_LNK);
                        break;
                case DT_FIFO:
                        printbranch(dir->d_name, level, DT_FIFO);
                        break;
                case DT_BLK:
                        printbranch(dir->d_name, level, DT_BLK);
                        break;
                case DT_CHR:
                        printbranch(dir->d_name, level, DT_CHR);
                        break;
                case DT_SOCK:
                        printbranch(dir->d_name, level, DT_SOCK);
                        break;
                case DT_UNKNOWN:
                        printbranch(dir->d_name, level, DT_UNKNOWN);
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
            <string>                Directory path\n\
            -d <number>             Folder depth\n\
            -c                      Disable color\n\
            -s                      Show file size\n\
            -f                      Find file\n");
}

void
getcurrdir(char *buff)
{
}

int
main(int argc, char *argv[])
{
        if (argc == 1) {
                char currdir[256];
                getcwd(currdir, 256);
                checkdirname(currdir, strlen(currdir));
                analysedir(currdir, 0);
                exit(1);
        }

        int opt;
        char dir[256];
        while ((opt = getopt(argc, argv, "d:cshp:")) != 1) {
                switch (opt) {
                case 'd':
                        maxdepth = *optarg - '0';
                        char currdir[256];
                        getcwd(currdir, 256);
                        checkdirname(currdir, strlen(currdir));
                        analysedir(currdir, 0);
                        goto exit;
                case 'c':
                        goto exit;
                case 's':
                        goto exit;
                case 'h':
                        showhelp();
                        goto exit;
                case 'p':
                        strcpy(dir, optarg);
                        checkdirname(dir, strlen(dir));
                        analysedir(dir, 0);
                        goto exit;
                case '?':
                        printf("Unkown option: %c\n", optopt);
                        goto exit;
                case ':':
                        printf("Missing arg for %c\n", optopt);
                        goto exit;
                default:
                        checkdirname(argv[1], strlen(argv[1]));
                        analysedir(argv[1], 0);
                        goto exit;
                }
        }
exit:
        return 0;
}
