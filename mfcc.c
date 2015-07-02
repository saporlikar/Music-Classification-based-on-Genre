#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> 


#define N 400                                // Number of samples in one frame
#define PI 3.14159265358979323846264338327

//Reads the given wav file and populates the short int data into an array.
void wave_Read(const char* filename);

//Computes the Discrete Fourier Transform for each frame. It is called iteratively from function wave_read()  
void dft(short int *x[],int num_frames);

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

//Computes the MFCC co-efficients for each frame. It is called iteratively from the function dft()
void calculate_mfcc(double spectrum[], const char *fname);

//wavfile header structure of 44 bytes 
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

/*
 *
 */
void wave_Read(const char* input_file)
{
        int i, j, k = 0;   // i,j,k indices for 'for' loops
        int BUFSIZE;       //BUFSIZE: size of buff16 array
        int num_frames;    //num_frames: number of frames
        int nb = 0;        // number of bytes actually read by fread() function

        header_p myHeader = (header_p)malloc(sizeof(struct wavfile_header)); //header containing 44 byte data

        FILE *fp = fopen(input_file,"r+"); //opens wav file

        if(!fp)
        {
                printf("File not found\n"); //check for valid pointer
        }
        else
        {
                fread(myHeader, 1, 44, fp);  //read first 44 bytes
    
                BUFSIZE = (myHeader->data_size)/2;
                num_frames = ceil(BUFSIZE/N);

                printf("Number of frames = %d\n",num_frames);

                short int *buff16=(short int *)malloc(BUFSIZE*sizeof(short int));
                short int *amplitude_data[num_frames];

                for (i = 0; i < num_frames; i++)
                {
                        amplitude_data[i] = (short int *)malloc(N * sizeof(short int));
                }      

                for (i = 0; i < num_frames; i++)
                {
                        for (j = 0; j < N; j++)
                        {
                                amplitude_data[i][j] = 0;           //initializing amplitude_data 2D array with all zeros
                        }
                }

                while (!feof(fp))
                {
                        nb = fread(buff16,2,BUFSIZE,fp);
                }

                for (i = 0;(i < num_frames) && (k < BUFSIZE); i++)
                {
                        for (j = 0;(j < N) && (k < BUFSIZE); j++)
                        {
                                amplitude_data[i][j] = buff16[k];   //copying data from buff16 to amplitude_data dividing the data into frames
                                k++;
                        }
                }

                dft((short int **)amplitude_data,num_frames);

                fclose(fp);
        }
}
/*
 *
 */
void dft(short int *x[], int num_frames)
{
        // time and frequency domain data arrays
        int n, k, i;             // indices for time and frequency domains
        //double x[N];           // discrete-time signal, x
        //float Xre[num_frames][N], Xim[num_frames][N]; 
        //float P[num_frames][N];           // power spectrum of x

        double *Xre[num_frames];

        for (i = 0; i < num_frames; i++)
        {
                Xre[i] = (double *)malloc(N * sizeof(double));              // DFT of x (real part)
        }                                    
            

        double *Xim[num_frames];

        for (i = 0; i < num_frames; i++)
        {
                Xim[i] = (double *)malloc(N * sizeof(double));              // DFT of x (imaginary part)
        }

        double *P[num_frames];

        for (i = 0; i < num_frames; i++)
        {
                P[i] = (double *)malloc(N * sizeof(double));                // Power Spectrum
        }                                  
            

        char *mfcc_coeff[num_frames];

        for (i = 0; i < num_frames; i++)
        {
                mfcc_coeff[i] = (char *)malloc(100 * sizeof(char));         // Names of txt files containing mfcc co-efficients for each frame
        }                                    
            


        // Calculate DFT of x using brute force
        for (i = 0; i < num_frames; i++)
        {
                for (k = 0 ; k < N ; ++k)
                {
                        // Real part of X[k]
                        Xre[i][k] = 0;

                        for (n = 0 ; n < N ; ++n) 
                        {
                                Xre[i][k] += (double)(x[i][n] * cos(n * k * PI * 2 / N));
                        }
         
                         // Imaginary part of X[k]
                        Xim[i][k] = 0;

                        for (n=0 ; n<N ; ++n)
                        {
                                Xim[i][k] -= (double)(x[i][n] * sin(n * k * PI * 2 / N));
                        }
         
                        // Power at kth frequency bin
                        P[i][k] = Xre[i][k]*Xre[i][k] + Xim[i][k]*Xim[i][k];
                }

                sprintf(mfcc_coeff[i],"mfcc%d.txt",i+1);

                calculate_mfcc(P[i],mfcc_coeff[i]);
        }
}

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
/*
 *
 */
void calculate_mfcc(double spectrum[], const char *mfcc_txt)
{
        // Determine which MFCC coefficient to compute
        unsigned int coeff;

        // Holds the value of the computed coefficient
        double mfcc_result;

        FILE *fp = fopen(mfcc_txt,"w+");

        // Compute the first 13 coefficients
        for (coeff = 0; coeff < 13; coeff++)
        {
                mfcc_result = GetCoefficient(spectrum, 44100, 48, N, coeff);
                fprintf(fp,"%i %f\n", coeff, mfcc_result);
        }
        fclose(fp);
}
/*
 * driver.c
 */
int main(int argc, char* argv[])
{
        char input_file[100];
        strcpy(input_file,argv[1]);
    
        wave_Read(input_file);

        return 0;
}

