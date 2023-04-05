#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_access_rights(mode_t mode)
{
    printf("User:\n");
    printf("Read - %s\n", (mode & S_IRUSR) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWUSR) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXUSR) ? "yes" : "no");
    printf("Group:\n");
    printf("Read - %s\n", (mode & S_IRGRP) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWGRP) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXGRP) ? "yes" : "no");
    printf("Others:\n");
    printf("Read - %s\n", (mode & S_IROTH) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWOTH) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXOTH) ? "yes" : "no");
}

void display_regular_file_options(const char *filename)
{
    struct stat file_stat;
    if (stat(filename, &file_stat) == -1)
    {
        perror("stat");
        return;
    }

    printf("File name: %s\n", filename);
    printf("File type: Regular file\n");
    printf("File size: %lld bytes\n", (long long)file_stat.st_size);
    printf("Hard link count: %ld\n", (long)file_stat.st_nlink);
    printf("Time of last modification: %s", ctime(&file_stat.st_mtime));
    print_access_rights(file_stat.st_mode);
    printf("Options:\n");
    printf("-n: Display file name\n");
    printf("-d: Display file size\n");
    printf("-h: Display hard link count\n");
    printf("-m: Display time of last modification\n");
    printf("-a: Display access rights\n");
    printf("-1: Create symbolic link (input link name)\n");
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        struct stat file_stat;
        if (lstat(argv[i], &file_stat) == -1)
        {
            perror("lstat");
            continue;
        }

        if (S_ISREG(file_stat.st_mode))
        {
            display_regular_file_options(argv[i]);
            printf("Enter options string: ");
            char options[256];
            fgets(options, sizeof(options), stdin);

            for (int j = 0; options[j] != '\0'; j++)
            {
                switch (options[j])
                {
                case 'n':
                    printf("File name: %s\n", argv[i]);
                    break;
                case 'd':
                    printf("File size: %%lld bytes\n", (long long)file_stat.st_size);
                    break;
                case 'h':
                    printf("Hard link count: %ld\n", (long)file_stat.st_nlink);
                    break;
                case 'm':
                    printf("Time of last modification: %s", ctime(&file_stat.st_mtime));
                    break;
                case 'a':
                    print_access_rights(file_stat.st_mode);
                    break;
                case '1':
                    printf("Enter link name: ");
                    char linkname[256];
                    fgets(linkname, sizeof(linkname), stdin);
                    linkname[strcspn(linkname, "\n")] = '\0';
                    if (symlink(argv[i], linkname) == -1)
                    {
                        perror("symlink");
                    }
                    break;
                default:
                    break;
                }
            }
        }
        else if (S_ISDIR(file_stat.st_mode))
        {
            printf("File name: %s\n", argv[i]);
            printf("File type: Directory\n");
        }
        else if (S_ISLNK(file_stat.st_mode))
        {
            printf("File name: %s\n", argv[i]);
            printf("File type: Symbolic link\n");
            printf("Target: ");
            char target[256];
            ssize_t len = readlink(argv[i], target, sizeof(target));
            if (len == -1)
            {
                perror("readlink");
            }
            else
            {
                target[len] = '\0';
                printf("%s\n", target);
            }
        }
        else
        {
            printf("File name: %s\n", argv[i]);
            printf("File type: Unknown\n");
        }
    }

    return 0;
}
