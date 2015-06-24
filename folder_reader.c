#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

//struct dirent {
//    ino_t d_ino; /* inode number */
//    off_t d_off; /* offset to the next dirent */
//    unsigned short d_reclen; /* length of this record */
//   unsigned char d_type; /* type of file */
//    char d_name[256]; /* filename */
//};
int list_dir (const char *path)
{
    struct dirent *entry;
    int ret = 0;
    DIR *dir;
    dir = opendir (path);

    while ((entry = readdir (dir)) != NULL) {
        printf("\n%s",entry->d_name);
        ret ++;
    }
    return ret;
}

void main(){
	int a = list_dir("/home/yashasvi/Desktop/ML");
	printf("\nNumber of files: %d \n",a);
}
