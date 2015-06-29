#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
struct wavfile_header
{
    char riff_tag[4];
    int riff_size;
    char wave_tag[4];
    char fmt_tag[4];
    int fmt_length;
    short format_type;
    short num_channels;
    int sample_rate;
    int byte_rate;
    short block_align;
    short bits_per_sample;
    char data_tag[4];
    int data_size;
};
typedef struct wavfile_header* header_p;

void wave_Read(const char* filename, const char* fname)
{
    //struct wavfile_header* myHeader;
    int count[10];
    int i,j;
    header_p myHeader = (header_p)malloc(sizeof(struct wavfile_header));

    for(i=0;i<10;i++)
		count[i]=0;

    FILE *fp = fopen(filename,"r+");
    //FILE *fp1 = fopen("output.txt","w+");
    if(fp)
    {
        fread(myHeader, 1, 44, fp);
    }
    else
    {
        printf("File not found\n");
    }
    int nb = 0;
    int BUFSIZE = (myHeader->data_size)/2;
    short int buff16[BUFSIZE];
    while(!feof(fp))
    {
        nb = fread(buff16,2,BUFSIZE,fp);
        //fwrite(buff16,1,nb,fp1);
    }
    FILE *fp2 = fopen(fname,"w+");
    for(i=0;i<BUFSIZE;i++)
    {
        fprintf(fp2,"%hd\n",buff16[i]);
    }

    fclose(fp2);
    //fclose(fp1);
    fclose(fp);

}

char *trim_wav(const char *track_name)
{
    int i = 0,j,len,l;
    len = strlen(track_name);
    char track_txt[len+1];
    for(i=0;i<(len-4);i++)
    {
        track_txt[i] = track_name[i];
    }
    track_txt[i]='.';
    track_txt[i+1]='t';
    track_txt[i+2]='x';
    track_txt[i+3]='t';
    track_txt[i+4]='\0';
    //printf("%s\n",track_txt);
    return track_txt;
}

int compare_files(const char *fname1, const char *fname2)
{
    char ch1,ch2;
    int count = 0;
    double percentage_matching;
    FILE *fp1 = fopen(fname1, "r+");
    FILE *fp2 = fopen(fname2, "r+");

    if (fp1 == NULL)
    {
      printf("Cannot open %s for reading ", fname1);
      exit(1);
    }
    else if (fp2 == NULL)
    {
      printf("Cannot open %s for reading ", fname2);
      exit(1);
    }
    else
    {
      ch1 = getc(fp1);
      ch2 = getc(fp2);

    while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2))
    {
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    }

    /*while ((ch1 != EOF) && (ch2 != EOF))
    {
	if(ch1==ch2)
	   count++;
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    }*/
    if (ch1 == ch2)
	return 1;
        //printf("Files are identical \n");
    else if (ch1 != ch2)
	return 0;
	//printf("Files are Not identical \n");
    //percentage_matching = (double)(count*100)/size;
    fclose(fp1);
    fclose(fp2);
    //return percentage_matching;
   }
}



int check_wav(const char *file_name)
{
    int i = 0,len;
    len = strlen(file_name);
    i=(len-4);
    if(file_name[i]!='.')
        return 0;
    else if(file_name[i+1]!='w')
        return 0;
    else if(file_name[i+2]!='a')
        return 0;
    else if(file_name[i+3]!='v')
        return 0;
    return 1;
}
int check_txt(const char *file_name)
{
    int i = 0,len;
    len = strlen(file_name);
    i=(len-4);
    if(file_name[i]!='.')
        return 0;
    else if(file_name[i+1]!='t')
        return 0;
    else if(file_name[i+2]!='x')
        return 0;
    else if(file_name[i+3]!='t')
        return 0;
    return 1;
}

void list_dir (FILE *fp,const char *path)
{
    struct dirent *entry;
    DIR *dir;
    dir = opendir (path);

    while ((entry = readdir (dir)) != NULL) {
        if(check_wav(entry->d_name))
            fprintf(fp,"\n%s",entry->d_name);
    }
}

int noof_files (const char *path)
{
    struct dirent *entry;
    int ret = 0;
    DIR *dir;
    dir = opendir (path);

    while ((entry = readdir (dir)) != NULL) {
        if(check_wav(entry->d_name))
            ret ++;
    }
    return ret;
}





void deleteFile(char *path) {
    int status;
    char filetodelete[25];
    char temp[100];

 //   printf("\n \t **Delete File**\n");

//    displayDIR(path);

    printf("\n\tChoose the name of the file to delete:\t");
    scanf("%s", filetodelete);

    //char fullpath[40];// = "C:/cygwin/tmp/";
    strcat(path,filetodelete);
    //printf("%s\n", path);
    status = remove(path);


//    status = remove(filetodelete);
    if( status == 0 )
        printf("%s file deleted successfully.\n",  filetodelete);
    else {
//        printf("\n\tUnable to delete the file");
        perror("\n\tError");
    }
}

void del_txt(char *path)
{
    char path_2[100];
    struct dirent *entry;
    DIR *dir;
    dir = opendir (path);

    int status;
    char filetodelete[25];
    char temp[100];

    while ((entry = readdir (dir)) != NULL) {
        strcpy(path_2,path);
        if(check_txt(entry->d_name))
        {    
            strcpy(filetodelete,entry->d_name);
            strcat(path_2,filetodelete);
            status = remove(path_2);
//            printf("\n%s deleted.",entry->d_name);
            if( status == 0 );
//                printf("%s file deleted successfully.\n",  filetodelete);
            else {
                perror("\n\tError");
            }
        }
    }

}

//deduplication.c and wav files should be in the same folder
int main()
{
    char z;
    char path[100];
    char path_temp[100];
    printf("Enter path of folder:\n");
    scanf("%s",path);
//    strcpy(path,"/home/yashasvi/git/Audio-Sample-Generator/Test/");
    strcpy(path_temp,path);
    FILE *f=fopen("input1.txt","w+");
    int a=noof_files(path);
    fprintf(f,"%d",a);
    list_dir(f,path);    
    fclose(f);

    int i,j,x,min;
    int num_tracks;
    //double percentage_matching;
    //printf("Enter number of tracks:\n");
    //scanf("%d", &num_tracks);

    FILE *fp = fopen("input1.txt","r+");
    fscanf(fp,"%d", &num_tracks);
    //int total_results[num_tracks];
    //printf("num_tracks = %d\n",num_tracks);

    char track_names[num_tracks][20];
    char track_txt[num_tracks][20];
    //printf("Enter track names:\n");

    for(i=0;i<num_tracks;i++)
    {
        fscanf(fp,"%s", track_names[i]);
    }

    fclose(fp);

    for(i=0;i<num_tracks;i++)
    {
        strcpy(track_txt[i],trim_wav(track_names[i]));
        //printf("%s\n",track_txt[i]);
        wave_Read(track_names[i],track_txt[i]);
    }
    /*for(i=0;i<num_tracks;i++)
    {
	printf("%d\n",total_results[i]);
    }*/
    for(i=0;i<num_tracks;i++)
    {
        for(j=i+1;j<num_tracks;j++)
        {
            /*if(total_results[i]<total_results[j])
 		min = total_results[i];
	    else
		min = total_results[j];*/

            x = compare_files(track_txt[i],track_txt[j]);
   	        if(x)
            {
	       	    printf("Files %s and %s are matching\n",track_names[i],track_names[j]);
/*                printf("Do you want to delete a file[y/n]?\n");
                scanf("%s",z);
                if(!strcmp(z,"y"))
*/              deleteFile(path);
            }
                
        }
    }
    del_txt(path_temp);

    return 0;
}

