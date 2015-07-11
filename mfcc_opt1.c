#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> 
#include <dirent.h>


#define FRAMES_25ms 1200
#define PI 3.14159265358979323846264338327

//Reads the given wav file and populates the short int data into an array.
void wave_Read(const char* filename, const char* output);

//Computes the Discrete Fourier Transform for each frame. It is called iteratively from function wave_read()  
void dft(short int *x[], unsigned int samples_per_frame, const char* output);

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
void calculate_mfcc(double spectrum[], double *mfcc_result[], unsigned int i, unsigned int samples_per_frame, FILE *fp);

// Checks if the file is a wav file or not
int check_wav(const char *file_name);

// Lists all the (wav) files in the directory
void list_dir (FILE *fp,const char *path);

// Returns the number of (wav) files in the directory
int noof_files (const char *path);

// Returns modified filename without .wav extension
char *trim_wav2(const char *track_name);

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
void wave_Read(const char* input_file, const char* output)
{
        unsigned int i, j, k;                            // i,j,k indices for 'for' loops
        unsigned int BUFSIZE;                            //BUFSIZE: size of buff16 array
        unsigned int num_frames;                         //num_frames: number of frames
        unsigned int nb;                                 // number of bytes actually read by fread() function
        unsigned int samples_per_frame;                  // number of samples per frame
        unsigned int total_samples;
        unsigned int samples_required;
        unsigned int start_index,end_index;

        header_p myHeader = (header_p)malloc(sizeof(struct wavfile_header)); //header containing 44 byte data

        FILE *fp = fopen(input_file,"r+");      //opens wav file

        if(!fp)
        {
                printf("File not found\n");     //check for valid pointer
        }
        else
        {
                fread(myHeader, 1, 44, fp);     //read first 44 bytes
    
                BUFSIZE = (myHeader->data_size)/2;
                samples_per_frame = (myHeader->sample_rate)/40; //(sample_rate * 25)/1000 for 25 ms frame
                num_frames = ceil(BUFSIZE/samples_per_frame);

                short int *buff16=(short int *)malloc(BUFSIZE*sizeof(short int));
                short int *amplitude_data[FRAMES_25ms];

                for (i = FRAMES_25ms; i-- ; )
                {
                        amplitude_data[i] = (short int *)malloc(samples_per_frame * sizeof(short int));

                        for (j = samples_per_frame; j--; )
                        {
                                amplitude_data[i][j] = 0;                                   //initializing amplitude_data 2D array with all zeros
                        }
                }      

                while (!feof(fp))
                {
                        nb = fread(buff16,sizeof(int),BUFSIZE,fp);
                }

                total_samples = samples_per_frame * num_frames;
                samples_required = samples_per_frame * FRAMES_25ms;
                start_index = (total_samples - samples_required + 2)/2;
                end_index = (samples_required + start_index - 1);
                k = start_index;

                for (i = 0;(i < FRAMES_25ms) && (k <= end_index); i++)
                {
                        for (j = 0;(j < samples_per_frame) && (k <= end_index); j++)
                        {
                                amplitude_data[i][j] = buff16[k];                           //copying data from buff16 to amplitude_data dividing the data into frames
                                k++;
                        }
                }

                dft((short int **)amplitude_data, samples_per_frame, output);

                fclose(fp);
        }
}
/*
 *
 */
void dft(short int *x[], unsigned int samples_per_frame, const char* output)
{
        // time and frequency domain data arrays
        unsigned int n, k, i;             // indices for time and frequency domains
        //double x[N];           // discrete-time signal, x
        //float Xre[num_frames][N], Xim[num_frames][N]; 
        //float P[num_frames][N];           // power spectrum of x
        unsigned int coeff;

        double *mfcc_result[FRAMES_25ms];

        double *Xre[FRAMES_25ms];

        double *Xim[FRAMES_25ms];

        double *P[FRAMES_25ms];

        char *mfcc_coeff[FRAMES_25ms];

        char fname[10][50];

        char extension[10][10];

        for(i=0;i<10;i++)
        {
                sprintf(extension[i],"%d.txt",i+1);
        }

        for(i=0;i<10;i++)
        {
                strcpy(fname[i],output);
                strcat(fname[i], extension[i]);
        }
 

        FILE *fp1 = fopen(fname[0],"w+");
        FILE *fp2 = fopen(fname[1],"w+");
        FILE *fp3 = fopen(fname[2],"w+");
        FILE *fp4 = fopen(fname[3],"w+");
        FILE *fp5 = fopen(fname[4],"w+");
        FILE *fp6 = fopen(fname[5],"w+");
        FILE *fp7 = fopen(fname[6],"w+");
        FILE *fp8 = fopen(fname[7],"w+");
        FILE *fp9 = fopen(fname[8],"w+");
        FILE *fp10 = fopen(fname[9],"w+");


                


        for (i = 0; i < FRAMES_25ms; i++)
        {
                mfcc_result[i] = (double *)malloc(13 * sizeof(double));                     // MFCC for each frame

                Xre[i] = (double *)malloc(samples_per_frame * sizeof(double));              // DFT of x (real part)

                Xim[i] = (double *)malloc(samples_per_frame * sizeof(double));              // DFT of x (imaginary part)

                P[i] = (double *)malloc(samples_per_frame * sizeof(double));                // Power Spectrum

                mfcc_coeff[i] = (char *)malloc(100 * sizeof(char));                         // Names of txt files containing mfcc co-efficients for each frame

                for (k = samples_per_frame ; k-- ; )
                {
                        // Real part of X[k]
                        Xre[i][k] = 0;

                        // Imaginary part of X[k]
                        Xim[i][k] = 0;

                        for (n = samples_per_frame ; n-- ; ) 
                        {
                                Xre[i][k] += (double)(x[i][n] * cos(n * k * PI * 2 / samples_per_frame));

                                Xim[i][k] -= (double)(x[i][n] * sin(n * k * PI * 2 / samples_per_frame));
                        }
         
                        // Power at kth frequency bin
                        P[i][k] = Xre[i][k]*Xre[i][k] + Xim[i][k]*Xim[i][k];
                }


		        // Compute the first 13 coefficients
		        for (coeff = 1; coeff < 14; coeff++)
		        {
		                mfcc_result[i][coeff] = GetCoefficient(P[i], 44100, 48, samples_per_frame, coeff);
		        }


                fprintf(fp1, "1 ");

                for (coeff = 1; coeff < 14; coeff++)
                {
                        fprintf(fp1,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                }

                fprintf(fp1,"\n");

                
                fprintf(fp2, "2 ");
                
                for (coeff = 1; coeff < 14; coeff++)
                {
                           
                        fprintf(fp2,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                            
                }

                fprintf(fp2,"\n");

                    
                fprintf(fp3, "3 ");
                    
                for (coeff = 1; coeff < 14; coeff++)
                {
                        fprintf(fp3,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                }

                fprintf(fp3,"\n");

                    

                fprintf(fp4, "4 ");
                    
                for (coeff = 1; coeff < 14; coeff++)
                {
                            
                        fprintf(fp4,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                }

                fprintf(fp4,"\n");

                
                fprintf(fp5, "5 ");
                    
                for (coeff = 1; coeff < 14; coeff++)
                {
                        fprintf(fp5,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                }

                fprintf(fp5,"\n");

                
                fprintf(fp6, "6 ");
                
                for (coeff = 1; coeff < 14; coeff++)
                {
                        fprintf(fp6,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                }

                fprintf(fp6,"\n");

                
                fprintf(fp7, "7 ");
                    
                for (coeff = 1; coeff < 14; coeff++)
                {
                        fprintf(fp7,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                }

                fprintf(fp7,"\n");

                    
                fprintf(fp8, "8 ");
                
                for (coeff = 1; coeff < 14; coeff++)
                {
                        fprintf(fp8,"%i:%lf ", coeff, mfcc_result[i][coeff]);            
                }

                fprintf(fp8,"\n");

                
                fprintf(fp9, "9 ");
                
                for (coeff = 1; coeff < 14; coeff++)
                {
                        fprintf(fp9,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                }

                fprintf(fp9,"\n");


                fprintf(fp10, "10 ");
                    
                for (coeff = 1; coeff < 14; coeff++)
                {
                        fprintf(fp10,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                }

                fprintf(fp10,"\n");
                    
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
 * Used for internal computation only - not to be called directly
 */
double GetFilterParameter(unsigned int samplingRate, unsigned int binSize, unsigned int frequencyBand, unsigned int filterBand)
{
        double filterParameter = 0.0f;

        double boundary = (frequencyBand * samplingRate) / binSize;                     // k * Fs / N
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
 * Used for internal computation only - not to be called directly
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
 * Used for internal computation only - not to be called directly
 */
double GetCenterFrequency(unsigned int filterBand)
{
        double centerFrequency = 0.0f;
        double exponent;

        if(!filterBand)
        {
                centerFrequency = 0;
        }
        else if(filterBand > 0 && filterBand < 15)
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
void calculate_mfcc(double spectrum[], double *mfcc_result[], unsigned int i, unsigned int samples_per_frame, FILE *fp)
{
        // Determine which MFCC coefficient to compute
        unsigned int coeff;

        // File containing MFCC coefficents for each frame
        //FILE *fp1 = fopen(mfcc_txt,"w+");

        // Compute the first 13 coefficients
        for (coeff = 1; coeff < 14; coeff++)
        {
                mfcc_result[i][coeff] = GetCoefficient(spectrum, 44100, 48, samples_per_frame, coeff);
                fprintf(fp,"%i:%lf ", coeff, mfcc_result[i][coeff]);
                //fprintf(fp1,"%i %lf\n ", coeff, mfcc_result[i][coeff]);
        }

        fprintf(fp,"\n");
        //fclose(fp1);
}
/*
*
*/
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
/*
*
*/
void list_dir (FILE *fp,const char *path)
{
        struct dirent *entry;
        DIR *dir;
        dir = opendir (path);

        while ((entry = readdir (dir)) != NULL)
        {
                if(check_wav(entry->d_name))
                fprintf(fp,"\n%s",entry->d_name);
        }
}
/*
*
*/
int noof_files (const char *path)
{
        struct dirent *entry;
        int ret = 0;
        DIR *dir;
        dir = opendir (path);

        while ((entry = readdir (dir)) != NULL)
        {
            if(check_wav(entry->d_name))
                    ret ++;
        }
        return ret;
}
/*
*
*/
char *trim_wav2(const char *track_name)
{
    int i = 0,j,len,l;
    len = strlen(track_name);
    char *track_txt = malloc(len+1);  // allocate memory from the heap
    for(i=0;i<(len-4);i++)
    {
        track_txt[i] = track_name[i];
    }
    track_txt[i]='.';
    track_txt[i+1]='\0';
    return track_txt;
}
/*
 * driver.c
 */
int main(int argc, char* argv[])
{
        char input_file[100];
        char output[100];

        strcpy(input_file,argv[1]);
        strcat(input_file,".wav");
        strcpy(output, trim_wav2(input_file));
    
        wave_Read(input_file, output);              // call the wave_Read() function

        return 0;
}
