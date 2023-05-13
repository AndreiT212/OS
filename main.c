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
                        printf("Size: %ld bytes\n"); //getDirectorySize(path));
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

int computeScore(int pf[2]) {
    close(pf[1]); 

    char buffer[256];
    ssize_t bytesRead;
    int numErrors = 0;
    int numWarnings = 0;

    while ((bytesRead = read(pf[0], buffer, sizeof(buffer))) > 0) {

        char* errorStart = strstr(buffer, "error:");
        if (errorStart != NULL) {
            int scannedErrors;
            sscanf(errorStart, "error:%d", &scannedErrors);
            numErrors += scannedErrors;
        }

        char* warningStart = strstr(buffer, "warning:");
        if (warningStart != NULL) {
            int scannedWarnings;
            sscanf(warningStart, "warning:%d", &scannedWarnings);
            numWarnings += scannedWarnings;
        }
    }

    close(pf[0]);

    int score;

    if (numErrors > 0) {
        score = 1; 
    } else if (numWarnings > 10) {
        score = 2; 
    } else {
        score = 2 + 8 * (10 - numWarnings) / 10; 
    }

    return score;
}

void script(int pf[2],char* path)
{
        close(pf[0]); 

        dup2(pf[1], STDOUT_FILENO);

        execl("script.sh", "script.sh", path, NULL);

        perror("Failed to execute script");
        exit(-1);
}

int checkC(char* path)
{
    if(strstr(path,".c"))
    {
        return 1;
    }
    return 0;
}

void printNrLines(char *path)
{
    execl("/usr/bin/wc", "wc", "-l", path, NULL);

    printf("Failed to execute wc command.\n");
    exit(-1);
}

void createTxtFile(const char* directoryName) {
    size_t directoryLength = strlen(directoryName);

    char fileName[directoryLength + 10];

    strncpy(fileName, directoryName, directoryLength);

    strncpy(fileName + directoryLength, "_file.txt", 10);

    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Failed to create the file.\n");
        return;
    }

    fclose(file);

    printf("File created: %s\n", fileName);
}

void changePermission(char *path)
{
    if (chmod(path, S_IRWXU | S_IRGRP | S_IWGRP) == -1) {
        perror("Failed to change symbolic link permissions");
    } else {
        printf("Symbolic link permissions changed successfully.\n");
    }
}



int main(int argc, char **argv) 
{
    struct stat st;

    int status;


    for (int i = 1; i < argc; i++) 
    {
        int pf[2];
        if(pipe(pf)<0)
        {
            perror("Couldn't create pipe");
            exit(-1);
        }

        int pid1=fork();
        if(pid1<0)
        {
            perror("Couldn't create first child");
            exit(-1);
        }

        if(pid1==0)
        {
            char *path=argv[i];

            if(lstat(path,&st)!=0)
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

            exit(1);
        }

        int finishedPid = wait(&status);
        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            printf("The process with PID %d has ended with exit code %d\n", finishedPid, exitCode);
        } else {
            printf("The process with PID %d did not terminate normally\n", finishedPid);
        }

        pid1=fork();
        if(pid1<0)
        {
            perror("Couldn't create second child");
            exit(-1);
        }

        if(pid1==0)
        {
             if(lstat(argv[i],&st))
            {
                perror("Couldn't get file info");
                exit(-1);
            }

            if(S_ISREG(st.st_mode))
            {
                if(checkC(argv[i]))
                {
                    script(argv[i],pf);
                }else{
                    printNrLines(argv[i]);
                }
            }

            if(S_ISDIR(st.st_mode))
            {
                createTxtFile(argv[i]);
            }

            if(S_ISLNK(st.st_mode))
            {
                changePermission(argv[i]);
            }
        }

        if(S_ISREG(st.st_mode))
        {
            if(checkC(argv[i]))
            {
                int score = computeScore(pf);

                // Get the file name from the path
                char* fileName = strrchr(argv[i], '/');
                if (fileName == NULL) {
                    fileName = argv[i];
                } else {
                    fileName++; // Move past the '/'
                }

                char output[100]; 
                snprintf(output, sizeof(output), "%s: %d", fileName, score);

                // Open the text file for writing
                int fd = open("grades.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
                if (fd == -1) {
                    perror("Failed to open grades.txt for writing");
                    exit(-1);
                }

                write(fd, output, strlen(output));
                write(fd, "\n", 1);

                close(fd);
            }
        }

        finishedPid = wait(&status);
        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            printf("The process with PID %d has ended with exit code %d\n", finishedPid, exitCode);
        } else {
            printf("The process with PID %d did not terminate normally\n", finishedPid);
        }
    }
    return 0;
}