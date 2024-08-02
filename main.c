#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <dirent.h>  // for directory operations
#include <sys/stat.h>  // for mkdir (if permission checking is needed)
#include <openssl/sha.h>// for hashing
#include <unistd.h>  // for getcwd
#include <sys/types.h>  // for opendir, readdir, closedir


int get_sha256_hash(const char *file_path, char output_hash[SHA256_DIGEST_LENGTH * 2 + 1]) {
  // Open the file in binary read mode
  FILE *file = fopen(file_path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Error: Could not open file '%s'\n", file_path);
    return -1;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate memory to read the file content
  char *file_content = malloc(file_size);
  if (file_content == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    fclose(file);
    return -1;
  }

  // Read the entire file content
  size_t bytes_read = fread(file_content, 1, file_size, file);
  if (bytes_read != file_size) {
    fprintf(stderr, "Error: Reading file '%s' failed\n", file_path);
    free(file_content);
    fclose(file);
    return -1;
  }

  // Calculate SHA-256 hash
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256((unsigned char *)file_content, file_size, hash);

  // Convert hash to hexadecimal string
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(output_hash + (i * 2), "%02x", hash[i]);
  }

  // Free allocated memory and close the file
  free(file_content);
  fclose(file);

  return 0;
}


///opt/homebrew/etc/openssl@3
//gcc main.c  -o file_manager -lssl -lcrypto

//gcc main.c -o file_manager -L/opt/homebrew/opt/openssl@3 -I/opt/homebrew/opt/openssl@3 -lssl -lcrypto


//gcc main.c -I /opt/homebrew/etc/openssl@3/include -o file_manager -lssl -lcrypto
// method to do sha-256 hashing a file
void sha256_hash_file(char *file_path) {
    // hash file
    char command[1024];
    snprintf(command, sizeof(command), "sha256sum %s", file_path);
    system(command);
}

//method to create a folder
void create_folder(char *folder_path) {
    // create folder
    int status = mkdir(folder_path, 0777);
    if (status == 0) {
        printf("Folder created\n");
    } else {
        printf("Error: Folder not created\n");
    }
}

//method to get current working directory
void get_current_working_directory() {
    // get current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        printf("Error: Current working directory not found\n");
    }
}

//method to list files in a directory and its subdirectories recursively
void list_files_in_directory(char *directory_path) {
    DIR *directory;
    struct dirent *entry;
    struct stat statbuf;
    char file_path[1024];

    // open main directory
    if ((directory = opendir(directory_path)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    //list files and if it is a directory, list files in the directory
    while ((entry = readdir(directory)) != NULL) {
        
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, entry->d_name);
        if (lstat(file_path, &statbuf) == -1) {
            perror("lstat");
            continue;
        }
        if (S_ISDIR(statbuf.st_mode)) {
            list_files_in_directory(file_path);
        } else {
            printf("%s\n", file_path);
        }
    }
    closedir(directory);
   
}

//METHOD To move a file from one directory to another
void move_file(char *source_file_path, char *destination_file_path) {
    // move file
    int status = rename(source_file_path, destination_file_path);
    if (status == 0) {
        printf("File moved\n");
    } else {
        printf("Error: File not moved\n");
    }
}


// hash all files in a directory
void hash_all_files_in_directory(char *directory_path) {
    // open directory
    DIR *directory = opendir(directory_path);
    if (directory == NULL) {
        printf("Error: Directory not found\n");
        return;
    }

    // hash files
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_type == DT_REG) {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, entry->d_name);
            char hash[SHA256_DIGEST_LENGTH * 2 + 1];
            get_sha256_hash(file_path, hash);
            printf("%s %s\n", hash, file_path);
        }
    }
    closedir(directory);
}

//move all files from subdirectories to the main directory
void moveFilesToMainDirectory(const char *mainDir) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char filePath[1024];
    

    // Open main directory
    if ((dir = opendir(mainDir)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }



    

    // Close main directory
    closedir(dir);
}




//sort files by date created
//sort files by type (extension) in to folders, search inside folder in the directory
void sort_files_by_type(char *directory_path) {
    // open directory
    DIR *directory = opendir(directory_path);
    if (directory == NULL) {
        printf("Error: Directory not found\n");
        return;
    }
    // list files
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_type == DT_REG) {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, entry->d_name);
            char *extension = strrchr(entry->d_name, '.');
            if (extension != NULL) {
                char folder_path[1024];
                snprintf(folder_path, sizeof(folder_path), "%s/%s", directory_path, extension + 1);
                create_folder(folder_path);
                char destination_file_path[1024];
                snprintf(destination_file_path, sizeof(destination_file_path), "%s/%s", folder_path, entry->d_name);
                move_file(file_path, destination_file_path);
            }
        }
    }
    closedir(directory);
}



int main() {
    //ask user for working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        printf("Error: Current working directory not found\n");
    }

    //go to /Users/alistairchambers/Downloads/tosort and list all files
    char directory_path[1024] = "/Users/alistairchambers/Downloads";
   //char directory_path[1024] = "/Volumes/Extreme SSD/to sort/to sort";
    //char directory_path[1024] = "/Users/alistairchambers/Desktop";
    //char directory_path[1024] = "/Volumes/Extreme SSD/to sort";
    
    //list all files in the directory
    //list_files_in_directory(directory_path);

    //hash all files in the directory
    //hash_all_files_in_directory(directory_path);

    //sort files by date created
    //sort_files_by_date_created(directory_path);

    //sort files by type (extension) in to folders
    sort_files_by_type(directory_path);

    //move files from subdir to main directories
    //moveFilesToMainDirectory(directory_path);



}




//gcc main.c -o file_manager -L/opt/homebrew/opt/openssl@3/lib -I/opt/homebrew/opt/openssl@3/include -lssl -lcrypto


//gcc main.c -o file_manager -L/opt/homebrew/opt/openssl@3/lib -I/opt/homebrew/opt/openssl@3/include -lssl -lcrypto

