//
// dft.c
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
char *get_filename(int k)
{
    char c[100];
    //itoa(k,c,10);
    sprintf(c,"mydftplot%d.m",k);
    return c;
}


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
#define N 400
#define PI2 6.2832
 
void dft(short int *x[],int num_frames)
{
    // time and frequency domain data arrays
    int n, k;             // indices for time and frequency domains
    //float x[N];           // discrete-time signal, x
    //float Xre[num_frames][N], Xim[num_frames][N]; // DFT of x (real and imaginary parts)
    //float P[num_frames][N];           // power spectrum of x
    int i;
    //char plot_names[num_frames][100];

    float *Xre[num_frames];
    for (i=0; i<num_frames; i++)
        Xre[i] = (float *)malloc(N * sizeof(float));

    float *Xim[num_frames];
    for (i=0; i<num_frames; i++)
        Xim[i] = (float *)malloc(N * sizeof(float));

    float *P[num_frames];
    for (i=0; i<num_frames; i++)
        P[i] = (float *)malloc(N * sizeof(float));

    char *plot_names[num_frames];
    for (i=0; i<num_frames; i++)
        plot_names[i] = (char *)malloc(100 * sizeof(char));
    // Generate random discrete-time signal x in range (-1,+1)
    //srand(time(0));
    //for (n=0 ; n<N ; ++n) x[n] = ((2.0 * rand()) / RAND_MAX) - 1.0;

 // Calculate DFT of x using brute force
    for(i=0;i<num_frames;i++)
    {
        for (k=0 ; k<N ; ++k)
        {
            // Real part of X[k]
            Xre[i][k] = 0;
            for (n=0 ; n<N ; ++n) Xre[i][k] += (float)(x[i][n] * cos(n * k * PI2 / N));
         
            // Imaginary part of X[k]
            Xim[i][k] = 0;
            for (n=0 ; n<N ; ++n) Xim[i][k] -= (float)(x[i][n] * sin(n * k * PI2 / N));
         
            // Power at kth frequency bin
            P[i][k] = Xre[i][k]*Xre[i][k] + Xim[i][k]*Xim[i][k];
        }
     
        // Output results to MATLAB / Octave M-file for plotting
        strcpy(plot_names[i],get_filename(i+1));
        FILE *f = fopen(plot_names[i], "w");
        fprintf(f, "n = [0:%d];\n", N-1);
        fprintf(f, "x = [ ");
        for (n=0 ; n<N ; ++n) fprintf(f, "%hd ", x[i][n]);
        fprintf(f, "];\n");
        fprintf(f, "Xre = [ ");
        for (k=0 ; k<N ; ++k) fprintf(f, "%f ", Xre[i][k]);
        fprintf(f, "];\n");
        fprintf(f, "Xim = [ ");
        for (k=0 ; k<N ; ++k) fprintf(f, "%f ", Xim[i][k]);
        fprintf(f, "];\n");
        fprintf(f, "P = [ ");
        for (k=0 ; k<N ; ++k) fprintf(f, "%f ", P[i][k]);
        fprintf(f, "];\n");
        fprintf(f, "subplot(3,1,1)\nplot(n,x)\n");
        fprintf(f, "xlim([0 %d])\n", N-1);
        fprintf(f, "subplot(3,1,2)\nplot(n,Xre,n,Xim)\n");
        fprintf(f, "xlim([0 %d])\n", N-1);
        fprintf(f, "subplot(3,1,3)\nstem(n,P)\n");
        fprintf(f, "xlim([0 %d])\n", N-1);
        fclose(f);
     
        // exit normally
    }
}


//wavfile.h
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

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
    int k =0;
    int m;
    int BUFSIZE = (myHeader->data_size)/2;
    int num_frames = ceil(BUFSIZE/400);
    //short int amplitude_data[num_frames][400];
    //short int buff16[BUFSIZE];

    short int *buff16=(short int *)malloc(BUFSIZE*sizeof(short int));
    //short int **amplitude_data;

    /*short int ** amplitude_data = (short int **)malloc(sizeof(short int*)*num_frames);
    for(i =0 ; i <400;i++)
        *(amplitude_data+i) = (short int*)malloc(sizeof(short int)*num_frames);*/

    short int *amplitude_data[num_frames];
    for (i=0; i<num_frames; i++)
         amplitude_data[i] = (short int *)malloc(400 * sizeof(short int));

    for(i=0;i<num_frames;i++)
    {
        for(j=0;j<400;j++)
        {
            amplitude_data[i][j] = 0;
        }
    }


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

    for(i=0;(i<num_frames) && (k<BUFSIZE);i++)
    {
        for(j=0;(j<400) && (k<BUFSIZE);j++)
        {
            amplitude_data[i][j] = buff16[k];
            k++;
        }
    }

    FILE *fp4 = fopen("output4.txt","w+");

    for(i=0;i<num_frames;i++)
    {
        fprintf(fp4,"Frame %d:\n",i+1);
        for(j=0;j<400;j++)
        {
            fprintf(fp4,"%hd\n",amplitude_data[i][j]);
        }
        fprintf(fp4,"\n");
    }
    dft((short int **)amplitude_data,num_frames);

    fclose(fp4);
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
