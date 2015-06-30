//wavfile.h
#include<stdio.h>
#include<stdlib.h>
#include<math.h>


void dft(short int *x[],int num_frames);
void calculate_mfcc(double spectrum[], const char *fname);

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

char *get_filename2(int k)
{
    char c[100];
    //itoa(k,c,10);
    sprintf(c,"mfcc%d.txt",k);
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

    double *Xre[num_frames];
    for (i=0; i<num_frames; i++)
        Xre[i] = (double *)malloc(N * sizeof(double));

    double *Xim[num_frames];
    for (i=0; i<num_frames; i++)
        Xim[i] = (double *)malloc(N * sizeof(double));

    double *P[num_frames];
    for (i=0; i<num_frames; i++)
        P[i] = (double *)malloc(N * sizeof(double));

    char *plot_names[num_frames];
    for (i=0; i<num_frames; i++)
        plot_names[i] = (char *)malloc(100 * sizeof(char));

    char *mfcc_coeff[num_frames];
    for (i=0; i<num_frames; i++)
        mfcc_coeff[i] = (char *)malloc(100 * sizeof(char));
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
            for (n=0 ; n<N ; ++n) Xre[i][k] += (double)(x[i][n] * cos(n * k * PI2 / N));
         
            // Imaginary part of X[k]
            Xim[i][k] = 0;
            for (n=0 ; n<N ; ++n) Xim[i][k] -= (double)(x[i][n] * sin(n * k * PI2 / N));
         
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
        strcpy(mfcc_coeff[i],get_filename2(i+1));
        calculate_mfcc(P[i],mfcc_coeff[i]);

    }
}



#pragma once

#define PI 3.14159265358979323846264338327

// Returns the specified (mth) MFCC
double GetCoefficient(double* spectralData, unsigned int samplingRate, unsigned int NumFilters, unsigned int binSize, unsigned int m);

// Compute the normalization factor (For internal computation only - not to be called directly)
double NormalizationFactor(int NumFilters, int m);

// Compute the filter parameter for the specified frequency and filter bands (For internal computation only - not the be called directly)
double GetFilterParameter(unsigned int samplingRate, unsigned int binSize, unsigned int frequencyBand, unsigned int filterBand);

// Compute the band-dependent magnitude factor for the given filter band (For internal computation only - not the be called directly)
double GetMagnitudeFactor(unsigned int filterBand);

// Compute the center frequency (fc) of the specified filter band (l) (For internal computation only - not the be called directly)
double GetCenterFrequency(unsigned int filterBand);





#include <math.h>
//#include "libmfcc.h"

/* 
 * Computes the specified (mth) MFCC
 *
 * spectralData - array of doubles containing the results of FFT computation. This data is already assumed to be purely real
 * samplingRate - the rate that the original time-series data was sampled at (i.e 44100)
 * NumFilters - the number of filters to use in the computation. Recommended value = 48
 * binSize - the size of the spectralData array, usually a power of 2
 * m - The mth MFCC coefficient to compute
 *
 */
double GetCoefficient(double* spectralData, unsigned int samplingRate, unsigned int NumFilters, unsigned int binSize, unsigned int m)
{
        double result = 0.0f;
        double outerSum = 0.0f;
        double innerSum = 0.0f;
        unsigned int k, l;

        // 0 <= m < L
        if(m >= NumFilters)
        {
                // This represents an error condition - the specified coefficient is greater than or equal to the number of filters. The behavior in this case is undefined.
                return 0.0f;
        }

        result = NormalizationFactor(NumFilters, m);

        
        for(l = 1; l <= NumFilters; l++)
        {
                // Compute inner sum
                innerSum = 0.0f;
                for(k = 0; k < binSize - 1; k++)
                {
                        innerSum += fabs(spectralData[k] * GetFilterParameter(samplingRate, binSize, k, l));
                }

                if(innerSum > 0.0f)
                {
                        innerSum = log(innerSum); // The log of 0 is undefined, so don't use it
                }

                innerSum = innerSum * cos(((m * PI) / NumFilters) * (l - 0.5f));

                outerSum += innerSum;
        }

        result *= outerSum;

        return result;
}

/* 
 * Computes the Normalization Factor (Equation 6)
 * Used for internal computation only - not to be called directly
 */
double NormalizationFactor(int NumFilters, int m)
{
        double normalizationFactor = 0.0f;

        if(m == 0)
        {
                normalizationFactor = sqrt(1.0f / NumFilters);
        }
        else 
        {
                normalizationFactor = sqrt(2.0f / NumFilters);
        }
        
        return normalizationFactor;
}

/* 
 * Compute the filter parameter for the specified frequency and filter bands (Eq. 2)
 * Used for internal computation only - not the be called directly
 */
double GetFilterParameter(unsigned int samplingRate, unsigned int binSize, unsigned int frequencyBand, unsigned int filterBand)
{
        double filterParameter = 0.0f;

        double boundary = (frequencyBand * samplingRate) / binSize;             // k * Fs / N
        double prevCenterFrequency = GetCenterFrequency(filterBand - 1);                // fc(l - 1) etc.
        double thisCenterFrequency = GetCenterFrequency(filterBand);
        double nextCenterFrequency = GetCenterFrequency(filterBand + 1);

        if(boundary >= 0 && boundary < prevCenterFrequency)
        {
                filterParameter = 0.0f;
        }
        else if(boundary >= prevCenterFrequency && boundary < thisCenterFrequency)
        {
                filterParameter = (boundary - prevCenterFrequency) / (thisCenterFrequency - prevCenterFrequency);
                filterParameter *= GetMagnitudeFactor(filterBand);
        }
        else if(boundary >= thisCenterFrequency && boundary < nextCenterFrequency)
        {
                filterParameter = (boundary - nextCenterFrequency) / (thisCenterFrequency - nextCenterFrequency);
                filterParameter *= GetMagnitudeFactor(filterBand);
        }
        else if(boundary >= nextCenterFrequency && boundary < samplingRate)
        {
                filterParameter = 0.0f;
        }

        return filterParameter;
}

/* 
 * Compute the band-dependent magnitude factor for the given filter band (Eq. 3)
 * Used for internal computation only - not the be called directly
 */
double GetMagnitudeFactor(unsigned int filterBand)
{
        double magnitudeFactor = 0.0f;
        
        if(filterBand >= 1 && filterBand <= 14)
        {
                magnitudeFactor = 0.015;
        }
        else if(filterBand >= 15 && filterBand <= 48)
        {
                magnitudeFactor = 2.0f / (GetCenterFrequency(filterBand + 1) - GetCenterFrequency(filterBand -1));
        }

        return magnitudeFactor;
}

/*
 * Compute the center frequency (fc) of the specified filter band (l) (Eq. 4)
 * This where the mel-frequency scaling occurs. Filters are specified so that their
 * center frequencies are equally spaced on the mel scale
 * Used for internal computation only - not the be called directly
 */
double GetCenterFrequency(unsigned int filterBand)
{
        double centerFrequency = 0.0f;
        double exponent;

        if(filterBand == 0)
        {
                centerFrequency = 0;
        }
        else if(filterBand >= 1 && filterBand <= 14)
        {
                centerFrequency = (200.0f * filterBand) / 3.0f;
        }
        else
        {
                exponent = filterBand - 14.0f;
                centerFrequency = pow(1.0711703, exponent);
                centerFrequency *= 1073.4;
        }
        
        return centerFrequency;
}


void calculate_mfcc(double spectrum[], const char *fname)
{
        int i = 0;

        // Determine which MFCC coefficient to compute
        unsigned int coeff;

        // Holds the value of the computed coefficient
        double mfcc_result;

        FILE *fp = fopen(fname,"w+");

        // Compute the first 13 coefficients
        for(coeff = 0; coeff < 13; coeff++)
        {
                mfcc_result = GetCoefficient(spectrum, 44100, 48, 400, coeff);
                fprintf(fp,"%i %f\n", coeff, mfcc_result);
        }
        fclose(fp);
}


#include <stdio.h>
#include <string.h>

//include "libmfcc.h"
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
