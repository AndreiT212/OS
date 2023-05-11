#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void printRights(char* path)
{
    struct stat st;
    if(stat(path,&st))
    {
        perror("couldn't get file info");
        exit(-1);
    }

    printf("User:\n");
    printf("Write - %s\n", st.st_mode & S_IWUSR ? "yes" : "no");
    printf("Write - %s\n", st.st_mode & S_IWUSR ? "yes" : "no");
    printf("Exec - %s\n", st.st_mode & S_IXUSR ? "yes" : "no");
    printf("\nGroup:\n");
    printf("Read - %s\n", st.st_mode & S_IRGRP ? "yes" : "no");
    printf("Write - %s\n", st.st_mode & S_IWGRP ? "yes" : "no");
    printf("Exec - %s\n", st.st_mode & S_IXGRP ? "yes" : "no");
    printf("\nOthers:\n");
    printf("Read - %s\n", st.st_mode & S_IROTH ? "yes" : "no");
    printf("Write - %s\n", st.st_mode & S_IWOTH ? "yes" : "no");
    printf("Exec - %s\n", st.st_mode & S_IXOTH ? "yes" : "no");
}

void createSymLink(char *path)
{
    char name[256];
    printf("Enter the name for symbolic link:");
    scanf("%s",name);

    if(symlink(path,name)==0)
        printf("created symlink: %s\n",name);
    else{
        perror("Couldn't create symlink");
        exit(-1);
    }
}

// Function to handle options for a regular file
void handleRegularFile(const char* path) {
    printf("Regular file: %s\n", path);

    char options[256];
    printf("Options:\n");
    printf("n - Name\n");
    printf("d - Size\n");
    printf("h - Hard link count\n");
    printf("m - Time of last modification\n");
    printf("a - Access rights\n");
    printf("l - Create symbolic link\n");

    struct stat st;
    if(stat(path,&st)==-1)
    {
        perror("Couldn't get file info");
        exit(-1);
    }

    // Get options from user
    while (1) {
        printf("Enter options: ");
        scanf("%s", options);

        // Check if the first character is '-'
        if (options[0] == '-') {
            // Process options
            int i = 1;  // Start from the second character
            while (options[i] != '\0') {
                switch (options[i]) {
                    case 'n':
                        printf("Name: %s\n", path);
                        break;
                    case 'd':
                        printf("Size: %lld bytes\n", st.st_size);
                        break;
                    case 'h':
                        printf("Hard link count: %ld\n", st.st_nlink);
                        break;
                    case 'm':
                        printf("Time of last modification: %s");// ctime(&st.st_mtime));
                        break;
                    case 'a':
                        printRights(path);
                        break;
                    case 'l':
                        createSymLink(path);
                        break;
                    default:
                        printf("Invalid option: %c\n", options[i]);
                        break;
                }
                i++;
            }
            break;  // Exit the loop
        } else {
            printf("Invalid options format. Please start with '-'\n");
        }
    }
}

int main(int argc, char **argv) 
{
    for (int i = 1; i < argc; i++) 
    {
        handleRegularFile(argv[i]);
    }
    return 0;
}