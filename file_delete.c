#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>


void displayDIR(char *path) {
    DIR           *d;
    struct dirent *dir;
    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL)
             printf("\t\t\t%s\n", dir->d_name);

        closedir(d);
    }
}


void deleteFile(char *path) {
    int status;
    char filetodelete[25];

    printf("\n \t **Delete File**\n");

    displayDIR(path);

 //   printf("\n\tChoose the name of the file to delete:\t");
 //   scanf("%s", filetodelete);

    //char fullpath[40];// = "C:/cygwin/tmp/";
 //   strcat(path, filetodelete);
    status = remove("home/yashasvi/Desktop/ML/track4.txt");


//    status = remove(filetodelete);
    if( status == 0 )
        printf("%s file deleted successfully.\n",  filetodelete);
    else {
//        printf("\n\tUnable to delete the file");
        perror("\n\tError");
    }
}


///home/yashasvi/Desktop/ML/track5.txt

void main(){
    deleteFile("/home/yashasvi/Desktop/ML/");

}
