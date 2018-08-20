/*THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
 A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - OWEN HANDEL */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<ar.h>
#include<fcntl.h>
#include<time.h>

void list(int archive);
void extract(int archive, int argc, char **argv);
void append(int archive, int argc, char **argv);
void appendall(int archive);
void verbose(int archive);

int main(int argc, char **argv) {
    char command;
    char *archivefile;
    int archive;
    command = argv[1][0];
    archivefile = argv[2];
    if (access(archivefile, F_OK) != -1) {
        archive = open(archivefile, O_RDWR);
        char test[8];
        test[8] = '\0';
        read(archive, test, 8); //reads first eight bytes of archive file
        if (strcmp(test, ARMAG) != 0) { //tests to see if archive file
            printf("Error: Not an archive file\n");
            close(archive);
            exit(-1);
        }
    } else {
        if (command == 'q' || command == 'A') { //for the append and append all commands
            archive = open(archivefile, O_RDWR | O_CREAT, 0666); //creates archive file giving rdwr permis
            printf("Creating new archive file\n");
            write(archive, ARMAG, 8); //write start of archive file
        } else {
            printf("Error: You need to create the archive file\n"); //error message
            exit(-1);
        }
    }
    switch (command) { //switch statements for various commands
        case ('t'):
            list(archive);
            break;
        case ('q'):
            append(archive, argc, argv);
            break;
        case ('x'):
            extract(archive, argc, argv);
            break;
        case ('A'):
            appendall(archive);
            break;
        case ('v'):
            verbose(archive);
            break;
        default:
            exit(-1);
    }
}

void append(int archive, int argc, char **argv) {
    char *filename; //filename pointer
    int i;
    lseek(archive, 0, SEEK_END); //seek to end of arch file
    printf("%d\n", argc);
    for (i = 3; i < argc; i++) { //start at three because files are argv[3] and beyond
        filename = argv[i]; //point to name entered
        printf("%s\n", filename);
        int file;
        struct ar_hdr *archiveheader = malloc(sizeof(struct ar_hdr)); //
        struct stat *filestat = malloc(sizeof(struct stat));
        file = open(filename, O_RDONLY); //open the file to be appended
        if (file == -1) { //error
            printf("File does not exist\n");
            close(archive);
            exit(-1);
        }
        fstat(file, filestat); //get metadata for the file
        sprintf(archiveheader->ar_name, "%-16s", filename); //metadata
        sprintf(archiveheader->ar_date, "%-12d", (int) filestat->st_mtime);
        sprintf(archiveheader->ar_uid, "%-6d", (int) filestat->st_uid);
        sprintf(archiveheader->ar_gid, "%-6d", (int) filestat->st_gid);
        sprintf(archiveheader->ar_mode, "%-8o", filestat->st_mode);
        sprintf(archiveheader->ar_size, "%-10d", (int) filestat->st_size);
        sprintf(archiveheader->ar_fmag, "%-2s", ARFMAG);
        write(archive, archiveheader, sizeof(struct ar_hdr)); //write metadata into the archive
        int filesize = (int) filestat->st_size; //get the size of the file
        char buffer[filesize]; //buffer to read file data
        while (filesize == (read(file, buffer, filesize))) {
            write(archive, buffer, filesize); //write into the archive the data from the file
        }
        if(filesize%2 != 0){
		write(archive,"\n", 1);
	}
	close(file);
    }
    close(archive);
    exit(0);
}

void list(int archive) {
    struct ar_hdr *ar_file = malloc(sizeof(struct stat));
    struct stat *ar_stat = malloc(sizeof(struct stat));
    struct stat *filestat = malloc(sizeof(struct stat));
    fstat(archive, ar_stat);
    int target_fd;
    int ar_size = (int) ar_stat->st_size;
    int i = 0;
    int k = 8;
    while (k < ar_size) { //start at 8 to account for archive size, and loop until end of archive
        read(archive, ar_file, 60); //getting name of file
        char *name = ar_file->ar_name;
        for (i = 0; i < 15; i++) {
            if ((name[i] == ' ') || (name[i] == '/')){
                name[i] = '\0';
            }
        }
        target_fd = open(name, O_RDONLY);
        printf("%s\n", name);
        if (target_fd < 0) {
            //printf("file no longer exists\n");
        }
        char *size = ar_file->ar_size;
        for (i = 0; i < 10; i++) {
            if (size[i] == ' ') {
                size[i] = '\0';
            }
        }
        int file_size = atoi(size);
        fstat(target_fd, filestat);
        //printf("File size: %d\n\n", file_size);
        if(file_size%2 != 0){
            file_size += 1;
        }
	lseek(archive, (file_size), SEEK_CUR); //setting next offset
        k = k + 60 + file_size; //iterate through archive
    }
}

void extract(int archive, int argc, char **argv) {
    struct ar_hdr *ar_file = malloc(sizeof(struct stat));
    struct stat *ar_stat = malloc(sizeof(struct stat));
    fstat(archive, ar_stat);
    int ar_size = (int) ar_stat->st_size;
    int i;
    char *filename;
    int k;
    for (i = 3; i < argc; i++) { //multiple extractions
        filename = argv[i]; //filename pointer set to input
        printf("Extracting %s\n", filename);
        k = 8;
        while (k < ar_size) {
            read(archive, ar_file, 60); //getting name of file
            char *name = ar_file->ar_name;
            int j;
            for (j = 0; j < 15; j++) {
                if ((name[j] == ' ') || (name[j] == '/')) {
                    name[j] = '\0';
                }
            }
            char *uid = ar_file->ar_uid;
            for (j = 0; j < 6; j++) {
                if (uid[j] == ' ') {
                    uid[j] = '\0';
                }
            }
            char *gid = ar_file->ar_gid;
            for (j = 0; j < 6; j++) {
                if (gid[j] == ' ') {
                    gid[j] = '\0';
                }
            }
            char *mode = ar_file->ar_mode;
            for (j = 0; j < 8; j++) {
                if (mode[j] == ' ') {
                    mode[j] = '\0';
                }
            }
            char *size = ar_file->ar_size;
            for (j = 0; j < 10; j++) {
                if (size[j] == ' ') {
                    size[j] = '\0';
                }
            }
            int file_size = atoi(size);
            uid_t uid_ = atoi(uid);
            gid_t gid_ = atoi(gid);
            int mode_ = strtol(mode, 0, 8);
            int test = strcmp(filename, name);
            if (test == 0) {
                char buffer[file_size];
                int fd = creat(filename, mode_);
                fchown(fd, uid_, gid_);
                read(archive, buffer, file_size);
                write(fd, buffer, file_size);
                printf("%s extracted\n\n", filename);
                close(fd);
                lseek(archive, 8, SEEK_SET);
                break;
            }
	    if(file_size%2 != 0){
            file_size += 1;
            }
            lseek(archive, (file_size), SEEK_CUR); //setting next offset
            k = k + 60 + file_size; //iterate through archive
        }
    }
    printf("File extraction complete\n");
    close(archive);
    exit(0);
}

int test_file(struct dirent *direct) {
    int fd = open(direct->d_name, O_RDWR);
    char test[8];
    test[8] = '\0';
    read(fd, test, 8);
    if (direct->d_type != DT_REG) {
        return 0;
    } else if ((strcmp(direct->d_name, "myar") == 0) || (strcmp(direct->d_name, "myar.c") == 0)) {
        return 0;
    } else if (strcmp(test, ARMAG) == 0) {
        return 0;
    } else {
        return 1;
    }
}

void appendall(int archive) {
    char *dir; //variables
    struct dirent *direct;
    char *append_file[1024];
    struct stat *filestat = malloc(sizeof(struct stat));
    struct ar_hdr *archiveheader = malloc(sizeof(struct ar_hdr));
    DIR *dirp = NULL;
    int file;
    time_t curtime;
    char path[1000];
    int index_f = 3;
    if (getcwd(path, sizeof(path)) == NULL) { //testing if in directory
        printf("error");
        exit(0);
    }
    dir = path; //path to directory
    char *filename;
    dirp = opendir(dir); //open directory
    curtime = time(NULL); //current system time in seconds
    while ((direct = readdir(dirp)) != NULL) { //while still files in direct
        filename = direct->d_name; //set filename
        file = open(filename, O_RDONLY); //open file
        fstat(file, filestat); //get time of file
        int file_time = (int) filestat->st_mtime;
        int file_size = (int) filestat->st_size;
        if ((curtime - file_time < 7200) && (test_file(direct) == 1)) { //append any file that meets requirements
            printf("Appending %s\n", filename);
            append_file[index_f] = filename;
            index_f++;
        }
    }
    append(archive, index_f, append_file);
    close(archive);
    exit(0);
}

void verbose(int archive) {
    struct ar_hdr *ar_file = malloc(sizeof(struct stat));
    struct stat *ar_stat = malloc(sizeof(struct stat));
    struct stat *filestat = malloc(sizeof(struct stat));
    fstat(archive, ar_stat);
    int target_fd;
    int ar_size = (int) ar_stat->st_size;
    int i = 0;
    int k = 8;
    while (k < ar_size) { //start at 8 to account for archive size, and loop until end of archive
        read(archive, ar_file, 60); //getting name of file
        char *name = ar_file->ar_name;
        for (i = 0; i < 15; i++) {
            if ((name[i] == ' ') || (name[i] == '/')) {
                name[i] = '\0';
            }
        }
        char *time = ar_file->ar_date;
        for (i = 0; i < 12; i++) {
            if (time[i] == ' ') {
                time[i] = '\0';
            }
        }
        char *uid = ar_file->ar_uid;
        for (i = 0; i < 6; i++) {
            if (uid[i] == ' ') {
                uid[i] = '\0';
            }
        }
        char *gid = ar_file->ar_gid;
        for (i = 0; i < 6; i++) {
            if (gid[i] == ' ') {
                gid[i] = '\0';
            }
        }
        char *mode = ar_file->ar_mode;
        for (i = 0; i < 8; i++) {
            if (mode[i] == ' ') {
                mode[i] = '\0';
            }
        }
        char *size = ar_file->ar_size;
        for (i = 0; i < 10; i++) {
            if (size[i] == ' ') {
                size[i] = '\0';
            }
        }
        int file_size = atoi(size);
        uid_t uid_ = atoi(uid);
        gid_t gid_ = atoi(gid);
        time_t time_ = atoi(time);
        struct tm *date;
        date = localtime(&time_);
        fstat(target_fd, filestat);
        char *modestr[6];
        for(i = 3; i < 6; i++){
            char c = mode[i] - '0';
            int perm = (int) c;
            switch(perm){
                case (1):
                    modestr[i] = "e--";
                    break;
                case (2):
                    modestr[i] = "w--";
                    break;
                case (3):
                    modestr[i] = "we-";
                    break;
                case (4):
                    modestr[i] = "r--";
                    break;
                case (5):
                    modestr[i] = "re-";
                    break;
                case (6):
                    modestr[i] = "rw-";
                    break;
                case (7):
                    modestr[i] = "rwe";
                    break;
                default:
                    break;
            }
        }
        char *real_date = asctime(date);
        real_date[strlen(real_date)-1] = 0;
        printf("%s%s%s %d/%d    %d %s %s\n",modestr[3],modestr[4],modestr[5],uid_,gid_,file_size,real_date,name);
        if(file_size%2 != 0){
            file_size += 1;
        }
        lseek(archive, (file_size), SEEK_CUR); //setting next offset
        k = k + 60 + file_size; //iterate through archive
    }
}

