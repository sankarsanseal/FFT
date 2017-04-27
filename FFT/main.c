//
//  main.c
//  FFT
//
//  Created by Sankarsan Seal on 25/03/17.
//  Copyright © 2017 Sankarsan Seal. All rights reserved.
//

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct
{
    double r; /* real */
    double i; /* imaginary */
} complex;

complex complex_mul(complex a, complex b)
{
    complex ans;
    ans.r = a.r * b.r - a.i * b.i;
    ans.i = a.r * b.i + a.i * b.r;
    return ans;
}

/* complex_add */
complex complex_add(complex a, complex b)
{
    complex ans;
    ans.r = a.r + b.r;
    ans.i = a.i + b.i;
    return ans;
}
/* complex_sub */
complex complex_sub(complex a, complex b)
{
    complex ans;
    ans.r = a.r - b.r;
    ans.i = a.i - b.i;
    return ans;
}


void recursive_fft(complex* a, complex* y, int n, int inv)
{
    complex w, wn, wnyk;
    complex* a0;
    complex* a1;
    complex* y0;
    complex* y1;
    int i, k;
    
    /* Base Case */
    if (n == 1)
    {
        y[0] = a[0];
        return;
    }
    /* Calculating nth root of unity (i.e. exp(2*PI*i/n)) */
    if (inv)
    {
        wn.r = cos(-2*M_PI/(double)n);
        wn.i = sin(-2*M_PI/(double)n);
    }
    else
    {
        wn.r = cos(2*M_PI/(double)n);
        wn.i = sin(2*M_PI/(double)n);
    }
    w.r = 1.0;
    w.i = 0.0;
    /* allocate memory for even/odd coefficients and corresponding FFTs */
    a0 = (complex*)malloc((n/2) * sizeof(complex));
    a1 = (complex*)malloc((n/2) * sizeof(complex));
    y0 = (complex*)malloc((n/2) * sizeof(complex));
    y1 = (complex*)malloc((n/2) * sizeof(complex));
    /* Extract even and odd coefficients */
    for (i = 0; i < (n/2); i++)
    {
        a0[i] = a[2*i];
        a1[i] = a[2*i+1];
    }
    /* Calculate 2 FFTs of size n/2 */
    recursive_fft(a0, y0, n/2, inv);
    recursive_fft(a1, y1, n/2, inv);
    /* Combine results from half-size FFTs */
    for (k = 0; k < (n/2); k++)
        
    {
        wnyk  = complex_mul(w, y1[k]);
        y[k] = complex_add(y0[k], wnyk);
        y[k+n/2] = complex_sub(y0[k], wnyk);
        w = complex_mul(w, wn);
    }
    
    free(a0);
    free(a1);
    free(y0);
    free(y1);
    
}


int main (int argc , char * argv[])
{
    
    FILE * inputfile;
    FILE * outputfile;
    char magicchar[3];
    char widthchar[4];
    char heightchar[4];
    char maxvalchar[4];
    unsigned char nothash;
    int width=0;
    int height=0;
    int maxval=0;
    complex **input_image;
    complex **fourier;
    complex **output_image;
    complex *fourier_temp1;
    complex *fourier_temp2;
    int i,j;
    double no_of_pixels=0;
    
    
    
    inputfile=fopen("aerial.pgm","rb");
    
    if ( inputfile != NULL)
    {
        fprintf(stdout,"Image file opened.\n");
        fscanf(inputfile,"%s%*c",magicchar);
        fprintf(stdout,"%s\n",magicchar);
        while((nothash=fgetc(inputfile))=='#')
        {
            fscanf(inputfile,"%*[^\n]s");
            //fprintf(stdout, "%c\n",nothash);
        }
        fputc((int)nothash, inputfile);
        
        fscanf(inputfile,"%s%s%*c",widthchar,heightchar);
        width=atoi(widthchar);
        height=atoi(heightchar);
        fprintf(stdout, "Width:%d Height%d\n", width, height);
        fscanf(inputfile, "%s%*c",maxvalchar);
        maxval=atoi(maxvalchar);
        fprintf(stdout,"Maxval: %d\n",maxval);
        
        no_of_pixels=(double)(width*height);
        
        input_image=(complex **)malloc(sizeof(complex*)*height);
        fourier=(complex **)malloc(sizeof(complex*)*height);
        output_image=(complex **)malloc(sizeof(complex*)*height);
        
        fourier_temp1=(complex *)malloc(sizeof(complex)*height);
        fourier_temp2=(complex *)malloc(sizeof(complex)*height);

        
        for(i=0;i<height;i++)
        {
            *(input_image+i)=(complex *)malloc(sizeof(complex)*width);
            *(fourier+i)=(complex *)malloc(sizeof(complex)*width);
            *(output_image+i)=(complex *)malloc(sizeof(complex)*width);
            for(j=0;j<width;j++)
            {
                input_image[i][j].r=fourier[i][j].r=output_image[i][j].r=0;
                input_image[i][j].i=fourier[i][j].i=output_image[i][j].i=0;
            }
        }
        
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                fscanf(inputfile,"%c",&nothash);
                input_image[i][j].r=(double)nothash;
               // fprintf(stdout,"%lf ",input_image[i][j].r);
            }
            //fprintf(stdout,"\n");
        }
        
        for(i=0;i<height;i++)
        {
            recursive_fft(input_image[i], fourier[i], width, 0);
        }
        
        for(i=0;i<width;i++)
        {
            for(j=0;j<height;j++)
            {
                fourier_temp1[j]=fourier[j][i];
                fourier_temp2[j].i=0;
                fourier_temp2[j].r=0;
                
                
            }
            
            recursive_fft(fourier_temp1, fourier_temp2, height, 0);
            
            for(j=0; j<height; j++)
            {
                fourier[j][i]=fourier_temp2[j];
            }
        }
        
//        for(i=0;i<height;i++)
//        {
//            for(j=0;j<width;j++)
//            {
//                fprintf(stdout,"Freq real: %lf Freq imag: %lf ", fourier[i][j].r,fourier[i][j].i);
//            }
//            
//            fprintf(stdout,"\n");
//        }
//        
        
        for (i=0;i<height;i++)
        {
            recursive_fft(fourier[i], output_image[i], height, 1);
        }
        
        for(i=0;i<width;i++)
        {
            for(j=0;j<height;j++)
            {
                fourier_temp1[j]=output_image[j][i];
                fourier_temp2[j].i=0;
                fourier_temp2[j].r=0;
            }
            
            recursive_fft(fourier_temp1, fourier_temp2, height, 1);
            
            for(j=0; j<height; j++)
            {
                output_image[j][i]=fourier_temp2[j];
                output_image[j][i].r/=no_of_pixels;
            }
            
        }
        
        outputfile=fopen("Output.pgm", "wb");
        fprintf(outputfile,"%s\n",magicchar);
        fprintf(outputfile,"%s %s\n",widthchar,heightchar);
        fprintf(outputfile,"%s\n",maxvalchar);
        
        if(outputfile!=NULL)
        {
            for(i=0;i<height;i++)
            {
                for(j=0;j<width;j++)
                {
                    fprintf(outputfile,"%c",(char)output_image[i][j].r);
                }

            }
            
                    fclose(outputfile);
        }
        
        for(i=0;i<height;i++)
        {
            free(*(input_image+i));
            free(*(fourier+i));
            free(*(output_image+i));

        }
        free(input_image);
        free(fourier);
        free(output_image);
        free(fourier_temp1);
        free(fourier_temp2);


        fclose(inputfile);
    }
    
      
    
    return 0;
}
