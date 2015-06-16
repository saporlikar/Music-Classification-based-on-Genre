//wavfile.h
#include<stdio.h>
#include<stdlib.h>

FILE * wavfile_open( const char *filename );
void wavfile_write( FILE *file, short data[], int length );
void wavfile_close( FILE * file );

#define WAVFILE_SAMPLES_PER_SECOND 44100
//

//wavfile.c
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

FILE * wavfile_open( const char *filename )
{
    struct wavfile_header header;
    int bits_per_sample = 16;
    int sample_rate = WAVFILE_SAMPLES_PER_SECOND;

    strcpy(header.riff_tag,"RIFF");
    header.riff_size = 0;
    strcpy(header.wave_tag,"WAVE");
    strcpy(header.fmt_tag,"fmt ");
    header.fmt_length = 16;
    header.format_type = 1;
    header.num_channels = 1;
    header.sample_rate = sample_rate;
    header.byte_rate = sample_rate*(bits_per_sample/8);
    header.block_align = (bits_per_sample)/8;
    header.bits_per_sample = bits_per_sample;
    strcpy(header.data_tag,"data");
    header.data_size = 0;

    FILE *fp = fopen(filename,"w+");
    if(!fp)
    {
        printf("File not found\n");
        return 0;
    }
    else
    {
        fwrite(&header,sizeof(header),1,fp);
        fflush(fp);
        return fp;
    }
}
void wavfile_write(FILE *fp, short data[], int length)
{
    fwrite(data,sizeof(short),length,fp);
}
void wavfile_close(FILE * fp)
{
    int file_size = ftell(fp);
    int data_size = file_size-44;
    fseek(fp,40,SEEK_SET);
    fwrite(&data_size,sizeof(int),1,fp);

    int riff_length = file_size - 8;
    fseek(fp,4,SEEK_SET);
    fwrite(&riff_length,sizeof(int),1,fp);

    fclose(fp);
}
/*void wavfile_read(const char *filename)
{
    struct wavfile_header h;
    FILE *fp = fopen(filename,"r+");
    fscanf(fp,"%s%d%s%s%d%hd%hd%d%d%hd%hd%s%d", h.riff_tag, &h.riff_size, h.wave_tag, h.fmt_tag, &h.fmt_length, &h.format_type, &h.num_channels, &h.sample_rate, &h.byte_rate, &h.block_align, &h.bits_per_sample, h.data_tag, &h.data_size);
    FILE *fpo = fopen("output.txt","w+");
	fprintf(fpo,"%s %d %s %s %d %hd %hd %d %d %hd %hd %s %d", h.riff_tag, h.riff_size, h.wave_tag, h.fmt_tag, h.fmt_length, h.format_type, h.num_channels, h.sample_rate, h.byte_rate, h.block_align, h.bits_per_sample, h.data_tag, h.data_size);
	fclose(fpo);
}*/


void wave_Read(const char* filename,int x,int y)
{
    //struct wavfile_header* myHeader;
    int count[10];
    int i,j;
    header_p myHeader = (header_p)malloc(sizeof(struct wavfile_header));

    for(i=0;i<10;i++)
		count[i]=0;

    FILE *fp = fopen(filename,"r+");
    FILE *fp1 = fopen("output.txt","w+");
    if(fp)
    {
        fread(myHeader, 1, 44, fp);

        fprintf(fp1,"riff_tag = %s\n", myHeader->riff_tag);
        fprintf(fp1,"riff_size = %d\n", myHeader->riff_size);
        fprintf(fp1,"wave_tag = %s\n", myHeader->wave_tag);
        fprintf(fp1,"fmt_tag = %s\n", myHeader->fmt_tag);
        fprintf(fp1,"fmt_length = %d\n", myHeader->fmt_length);
        fprintf(fp1,"format_type = %hd\n", myHeader->format_type);
        fprintf(fp1,"num_channels = %hd\n", myHeader->num_channels);
        fprintf(fp1,"sample_rate = %d\n", myHeader->sample_rate);
        fprintf(fp1,"byte_rate = %d\n", myHeader->byte_rate);
        fprintf(fp1,"block_align = %hd\n", myHeader->block_align);
        fprintf(fp1,"bits_per_sample = %hd\n", myHeader->bits_per_sample);
        fprintf(fp1,"data_tag = %s\n", myHeader->data_tag);
        fprintf(fp1,"data_size = %d\n", myHeader->data_size);
    }
    else
    {
        printf("File not found\n");
    }
    int nb = 0;
    int county = 0, countn = 0;
    int BUFSIZE = (myHeader->data_size)/2;
    short int buff16[BUFSIZE];
    while(!feof(fp))
    {
        nb = fread(buff16,2,BUFSIZE,fp);
        fwrite(buff16,1,nb,fp1);
    }
    FILE *fp2 = fopen("output2.txt","w+");
    for(i=0;i<BUFSIZE;i++)
    {
        fprintf(fp2,"%hd\n",buff16[i]);
    }

    fclose(fp2);
    fclose(fp1);
    fclose(fp);

    for(i=0;i<BUFSIZE;i++)
    {
        for(j=0;j<10;j++)
        {
            if(buff16[i]>=(x+((j)*(y-x)/10)) && buff16[i]<(x+((j+1)*(y-x)/10)))
                count[j]++;
        }
    }

    FILE *fp3 = fopen("output3.txt","w+");
    for(j=0;j<10;j++){
        fprintf(fp3,"%d - %d : %d\n",(x+((j)*(y-x)/10)),(x+((j+1)*(y-x)/10)),count[j]);

    }

    fclose(fp3);
}


//

//example.c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<errno.h>
#include<time.h>

int main(int argc,char* argv[])
{
	char fname[20];
	strcpy(fname,argv[1]);
	int x = atoi(argv[2]);
	int y = atoi(argv[3]);
        wave_Read(fname,x,y);

	return 0;
}
//
