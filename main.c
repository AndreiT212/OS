#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void printRights(mode_t mode) 
{
    printf("User:\n");
    printf("Write - %s\n", mode & S_IWUSR ? "yes" : "no");
    printf("Write - %s\n", mode & S_IWUSR ? "yes" : "no");
    printf("Exec - %s\n", mode & S_IXUSR ? "yes" : "no");
    printf("\nGroup:\n");
    printf("Read - %s\n", mode & S_IRGRP ? "yes" : "no");
    printf("Write - %s\n", mode & S_IWGRP ? "yes" : "no");
    printf("Exec - %s\n", mode & S_IXGRP ? "yes" : "no");
    printf("\nOthers:\n");
    printf("Read - %s\n", mode & S_IROTH ? "yes" : "no");
    printf("Write - %s\n", mode & S_IWOTH ? "yes" : "no");
    printf("Exec - %s\n", mode & S_IXOTH ? "yes" : "no");
}

void createSymLink(char *path)
{
    char *linkpath = strcat(path,"symlink");
    char command[256];
    sprintf(command, "ln -s %s %s", &path, linkpath);
    int result = system(command);
    if (result == -1) {
        perror("couldn't create symlink");
        exit(-1);
    }

    //     if (symlink(path, strcat(&path,"symlink")) == -1) {
    //     perror("symlink");
    //     return 1;
    // }

}

void fileProcess(char *path) 
{
    struct stat st;
    char opt;

    if (stat(path, &st) == -1) 
    {
        perror("stat");
        exit(-1);
    }
    printf("Name: %s\n", path);
    printf("Type: regular file\n");

    printf("Enter options: ");
    scanf("-%c", &opt);

    while (opt != '\n') 
    {
        switch (opt) 
        {
            case 'n':
                printf("Name: %s\n", path);
                break;
            case 'd':
                printf("Size: %ld bytes\n", st.st_size);
                break;
            case 'h':
                printf("Hard link count: %ld\n", st.st_nlink);
                break;
            case 'm':
                printf("Time of last modification: %s", ctime(&st.st_mtime));
                break;
            case 'a':
                printRights(st.st_mode);
                break;
            case 'l':
            	createSymLink(path);
                break;
            default:
                printf("Invalid option: -%c\n", opt);
        }

        scanf("%c", &opt);
    }
}

int main(int argc, char **argv) 
{
    for (int i = 1; i < argc; i++) 
    {
        fileProcess(argv[i]);
    }
    return 0;
}