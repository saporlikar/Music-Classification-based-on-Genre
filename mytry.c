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
//

//example.c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<errno.h>
#include<time.h>

int main(int argc, char** argv)
{
    int duration,i;
    double t;
    int frequency;
    int volume;
    char fname[20];

	//printf("Enter time duration of the wave:\n");
	//scanf("%d", &duration);
	duration = atoi(argv[1]);
   	frequency = atoi(argv[2]);
    	volume = atoi(argv[3]);
    	strcpy(fname,argv[4]);

	short waveform[WAVFILE_SAMPLES_PER_SECOND*duration];
	int length = WAVFILE_SAMPLES_PER_SECOND*duration;


	//printf("Enter frequency and volume:\n");
 	//scanf("%lf %d", &frequency, &volume);

 	//printf("Enter filename with .wav extension:\n");
    //scanf("%s", fname);

 	for(i=0;i<length;i++)
    {
        t = (double)i/ WAVFILE_SAMPLES_PER_SECOND;
        waveform[i]=volume*sin(2*(double)frequency*t*M_PI);
    }

    FILE * f = wavfile_open(fname);
	if(!f)
    {
		printf("couldn't open sound.wav for writing: %s",strerror(errno));
		return 1;
	}

	wavfile_write(f,waveform,length);
	wavfile_close(f);

	return 0;
}
//
