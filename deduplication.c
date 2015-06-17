#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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

int main()
{
    int i,j,x,min;
    int num_tracks;
    //double percentage_matching;
    //printf("Enter number of tracks:\n");
    //scanf("%d", &num_tracks);

    FILE *fp = fopen("input.txt","r+");
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
		printf("Files %d and %d are matching\n",i+1,j+1);
        }
    }
    return 0;
}

