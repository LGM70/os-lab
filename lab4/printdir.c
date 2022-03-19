#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pwd.h>

int showAll = 0;

void parseStMode(struct stat* statbuf, char* res) {
    // res is at least 10 bytes long
    switch (statbuf->st_mode & S_IFMT) { // file type
        case S_IFREG:
            res[0] = '-';
            break;
        case S_IFDIR:
            res[0] = 'd';
            break;
        case S_IFLNK:
            res[0] = 'l';
            break;
        case S_IFBLK:
            res[0] = 'b';
            break;
        case S_IFCHR:
            res[0] = 'c';
            break;
        case S_IFSOCK:
            res[0] = 's';
            break;
        case S_IFIFO:
            res[0] = 'p';
            break;
        default:
            res[0] = 'u';
    }
    res[1] = statbuf->st_mode & S_IRUSR ? 'r' : '-';
    res[2] = statbuf->st_mode & S_IWUSR ? 'w' : '-';
    res[3] = statbuf->st_mode & S_IXUSR ? 'x' : '-';
    res[4] = statbuf->st_mode & S_IRGRP ? 'r' : '-';
    res[5] = statbuf->st_mode & S_IWGRP ? 'w' : '-';
    res[6] = statbuf->st_mode & S_IXGRP ? 'x' : '-';
    res[7] = statbuf->st_mode & S_IROTH ? 'r' : '-';
    res[8] = statbuf->st_mode & S_IWOTH ? 'w' : '-';
    res[9] = statbuf->st_mode & S_IXOTH ? 'x' : '-';
}

void printdir(char* dir, int depth) {
    DIR* dp;
    struct dirent* entry;
    struct stat statbuf;
    char mode[10];
    char* modifyTime;

    if ((dp = opendir(dir)) == NULL) { // open dir
        // cannot open the directory
        fprintf(stderr, "Cannot open %s as a directory.\n", dir);
        exit(-1);
    }
    chdir(dir); // change directory to dir
    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if ((statbuf.st_mode & S_IFMT) == S_IFDIR) { // is directory
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) { // is . or ..
                continue;
            }
            if (!showAll & entry->d_name[0] == '.') {
                continue;
            }
            parseStMode(&statbuf, mode);
            modifyTime = ctime(&statbuf.st_mtime);
            modifyTime[strlen(modifyTime) - 1] = '\0';
            for (int i = 0; i < depth; i++) {
                printf("    ");
            }
            printf("%s %2ld %s %s %8ld %s \e[1;34m%s\e[0m/ depth=%d\n", mode, statbuf.st_nlink, getpwuid(statbuf.st_uid)->pw_name, getpwuid(statbuf.st_gid)->pw_name, statbuf.st_size, modifyTime + 4, entry->d_name, depth + 1);
            printdir(entry->d_name, depth + 1);
        }
        else if ((statbuf.st_mode & S_IFMT) == S_IFLNK) { // is link
            parseStMode(&statbuf, mode);
            modifyTime = ctime(&statbuf.st_mtime);
            modifyTime[strlen(modifyTime) - 1] = '\0';
            for (int i = 0; i < depth; i++) {
                printf("    ");
            }
            printf("%s %2ld %s %s %8ld %s \e[1;36m%s\e[0m depth=%d\n", mode, statbuf.st_nlink, getpwuid(statbuf.st_uid)->pw_name, getpwuid(statbuf.st_gid)->pw_name, statbuf.st_size, modifyTime + 4, entry->d_name, depth + 1);
        }
        else { // is regular file
            parseStMode(&statbuf, mode);
            modifyTime = ctime(&statbuf.st_mtime);
            modifyTime[strlen(modifyTime) - 1] = '\0';
            for (int i = 0; i < depth; i++) {
                printf("    ");
            }
            printf("%s %2ld %s %s %8ld %s %s depth=%d\n", mode, statbuf.st_nlink, getpwuid(statbuf.st_uid)->pw_name, getpwuid(statbuf.st_gid)->pw_name, statbuf.st_size, modifyTime + 4, entry->d_name, depth + 1);
        }
    }
    chdir(".."); // go back to parent dir
    closedir(dp); // close dir
}

void showHelp(char* cmd) {
    fprintf(stderr, "Usage: %s [OPTION]... [PATHNAME]\n", cmd);
    fprintf(stderr, "\nOPTION:\n");
    fprintf(stderr, "  -A, --almost-all\tdo not ignore entries starting with . (except for . and ..)\n"); 
    fprintf(stderr, "      --help\t\tdisplay this help and exit\n"); 
}

int main(int argc, char* argv[]) {
    if (argc == 1) { // default path is .
        printdir(".", 0);
        return 0;
    }
    for (int i = 1; i < argc - 1; i++) {
        char* option = argv[i];
        if (!strcmp(option, "-A") || !strcmp(option, "--almost-all")) {
            showAll = 1;
        }
        else if (!strcmp(option, "--help")) {
            showHelp(argv[0]);
            exit(0);
        }
        else {
            showHelp(argv[0]);
            exit(-1);
        }
    }
    if (!strcmp(argv[argc - 1], "--help")) {
        showHelp(argv[0]);
        exit(0);
    }
    printdir(argv[argc - 1], 0); // assume argv[argc - 1] is a directory
    return 0;
}