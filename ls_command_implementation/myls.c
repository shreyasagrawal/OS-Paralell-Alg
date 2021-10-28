/* Shreyas Agrawal
* This program is an implementation of the ls command on the command line
* The program prints the names of all the files and directories in a specified directory, 
  along with the user and group that own the file, the file’s permissions, and its size.
* myls command takes one optional parameter; if this parameter is provided, 
  it is the path to the directory that should be examined. 
  If no parameter is provided myls lists the current directory.
*/


#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void list_directory(const char* directory_name);

int main(int argc, char** argv) {
  // Process arguments
  if (argc == 1) {
    list_directory(".");
  } else if (argc == 2) {
    list_directory(argv[1]);
  } else {
    fprintf(stderr, "Please specify one or zero command line arguments.\n");
  }

  return 0;
}

void list_directory(const char* directory_name) {
  // opening the directory to read using the directory name
  DIR* p = opendir(directory_name);
  if (p == NULL) {
    perror("opendir failed\n");
    return;
  }

  struct dirent* file;

  // while loop to loop over the contents of a directory
  while ((file = readdir(p)) != NULL) {
    if (file == NULL) {
      perror("readdir failed\n");
      return;
    }

    // declaring the stat struct
    struct stat filedata;

    /*copying the directory_name into dirname
     * so that it can be used since directory_name is a const
     */
    char* dirname = strdup(directory_name);

    // creating a string that has the filepath
    char* filepath = malloc((strlen((dirname) + 1 + strlen(file->d_name))) * sizeof(char*));
    strcpy(filepath, dirname);
    strcat(filepath, "/");
    strcat(filepath, file->d_name);
    strcat(filepath, "\0");

    int c = stat(filepath, &filedata);
    if (c != 0) {
      perror("closesir failed \n");
      exit(1);
    }

    // printing out permissions by accesing bits for each permission type.
    printf("%c", S_ISDIR(filedata.st_mode) ? 'd' : '-');
    printf("%c", (filedata.st_mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (filedata.st_mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (filedata.st_mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (filedata.st_mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (filedata.st_mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (filedata.st_mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (filedata.st_mode & S_IROTH) ? 'r' : '-');
    printf("%c", (filedata.st_mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (filedata.st_mode & S_IXOTH) ? 'x' : '-');

    // printing out the user name
    struct passwd* pwd = getpwuid(filedata.st_uid);
    if (pwd == NULL) {
      perror("getpwuid failed \n");
      exit(1);
    }
    printf(" %s", pwd->pw_name);

    // printing out the group name
    struct group* grp = getgrgid(filedata.st_gid);
    if (grp == NULL) {
      perror("getgrid failed \n");
      exit(1);
    }
    printf(" %s", grp->gr_name);

    // Printing out the size
    printf(" %zu", (size_t)filedata.st_size);

    // printing out the filename
    printf(" %s\n", file->d_name);

    // clean up
    free(filepath);

  }  // while

  // closing the directory
  int a = closedir(p);
  if (a != 0) {
    perror("closesir failed \n");
    exit(1);
  }
}
