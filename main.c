#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

typedef enum 
{
    REGULAR,
    DIRECTORY,
    SYM,
    UNKNOWN
}Type;

Type getType(mode_t mode)
{
    if(S_ISLNK(mode)) {
        return SYM;
    }else if (S_ISDIR(mode)){
            return DIRECTORY;
    }else if (S_ISREG(mode)){
            return REGULAR;
    }else
        return UNKNOWN;
}

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

int countCFiles(char* path)
{
    DIR* dir;
    struct dirent* entry;
    int count =0;

    dir= opendir(path);
    if(dir==NULL)
    {
        perror("Couldn't open dir");
        return -1;
    }

    while((entry=readdir(dir))!=NULL)
    {
        if(entry->d_type==DT_REG && strstr(entry->d_name,".c")!=NULL)
            count++;
    }

    closedir(dir);

    return count;
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

void handleRegularFile(const char* path) 
{
    printf("Regular file: %s\n", path);

    char options[256];

    struct stat st;
    if(stat(path,&st)==-1)
    {
        perror("Couldn't get file info");
        exit(-1);
    }

    while (1) 
    {
        printf("Options:\n");
        printf("n - Name\n");
        printf("d - Size\n");
        printf("h - Hard link count\n");
        printf("m - Time of last modification\n");
        printf("a - Access rights\n");
        printf("l - Create symbolic link\n");
        printf("Enter options: ");
        scanf("%s", options);

        if (options[0] == '-') 
        {
            int i = 1;  
            while (options[i] != '\0') 
            {
                switch (options[i]) 
                {
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
                        printRights(st.st_mode);
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
            break;
        } else {
            printf("Invalid options format. Please start with '-'\n");
        }
    }
}

void handleDirectory(const char* path) 
{
    printf("Directory: %s\n", path);

    char options[256];

    while (1) 
    {
        printf("Options:\n");
        printf("n - Name\n");
        printf("d - Size\n");
        printf("a - Access rights\n");
        printf("c - Total number of files with .c extension\n");
        printf("Enter options: ");
        scanf("%s", options);

        if (options[0] == '-')
        {
            int i = 1;  
            while (options[i] != '\0') {
                switch (options[i]) 
                {
                    case 'n':
                        printf("Name: %s\n", path);
                        break;
                    case 'd':
                        printf("Size: %lld bytes\n"); //getDirectorySize(path));
                        break;
                    case 'a':
                        printRights(path);
                        break;
                    case 'c': {
                        int count = countCFiles(path);
                        if (count == -1)
                            printf("Failed to count files with .c extension\n");
                        else
                            printf("Total number of .c files: %d\n", count);
                        break;
                    }
                    default:
                        printf("Invalid option: %c\n", options[i]);
                        break;
                }
                i++;
            }
            break;  
        } else {
            printf("Invalid options format. Please start with '-'\n");
        }
    }
}

void handleSymbolicLink(char* path)
{
    printf("Symbolic Link: %s\n", path);

    struct stat st;
    char options[256];

    if (lstat(path, &st) < 0) 
    {
        printf("Failed to get symbolic link stats: %s\n", path);
        return;
    }

    while (1)
    {
        printf("Options: (Example input: -nld)\n");
        printf("-n: Display symbolic link name\n");
        printf("-l: Delete symbolic link\n");
        printf("-d: Display size of symbolic link\n");
        printf("-t: Display size of target file\n");
        printf("-a: Display access rights\n");
        printf("Enter options: ");
        scanf("%s", options);

        if (options[0] == '-') 
        {
            int i=1;
            while(options[i]!='\0')
            {
                char option = options[i];
                switch (option) 
                {
                    case 'n':
                        printf("Symbolic Link Name: %s\n", path);
                        break;
                    case 'l':
                        if (unlink(path) == 0) {
                            printf("Symbolic link deleted successfully\n");
                            return;
                        } else {
                            printf("Failed to delete symbolic link\n");
                        }
                        break;
                    case 'd':
                        printf("Size of Symbolic Link: %ld bytes\n", st.st_size);
                        break;
                    case 't': {
                        struct stat targetStat;
                        if (stat(path, &targetStat) == 0) {
                            printf("Size of Target File: %ld bytes\n", targetStat.st_size);
                        } else {
                            printf("Failed to get target file stats\n");
                        }
                        break;
                    }
                    case 'a':
                        printRights(st.st_mode);
                        break;
                    default:
                        printf("Invalid option: %c\n", options[i]);
                        break;
                }
                i++;
            }
            break;
        } else {
            printf("Invalid options format. Please start with '-'\n");
        }
    }
}

int main(int argc, char **argv) 
{
    struct stat st;

    for (int i = 1; i < argc; i++) 
    {
        char *path=argv[i];

        if(stat(path,&st)!=0)
        {
            perror("Couldn't get file info");
            exit(-1);
        }

        Type inputType=getType(st.st_mode);

        switch (inputType)
        {
        case REGULAR:{
            handleRegularFile(path);
            break;
        }
        case DIRECTORY:{
            handleDirectory(path);
            break;
        }
        case SYM:{
            handleSymbolicLink(path);
            break;
        }
        case UNKNOWN:{
            perror("Not a valid type of file");
            break;
        }
        default:
            break;
        }
    }
    return 0;
}