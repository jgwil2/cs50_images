/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Enlarge infile by n times
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy n infile outfile\n");
        return 1;
    }

    // check that n is an integer between 1 and 100
    int n = atoi(argv[1]);
    if (n < 1 || n > 100)
    {
        printf("n must be between 2 and 100\n");
        return 1;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // modify BITMAPFILEHEADER and BITMAPINFOHEADER for outfile
    BITMAPFILEHEADER newBf = bf;
    BITMAPINFOHEADER newBi = bi;

    // modify dimensions
    newBi.biWidth = bi.biWidth * n;
    newBi.biHeight = bi.biHeight * n;
    int absoluteHeight = abs(newBi.biHeight);

    // calculate new padding in bytes
    int outputPadding = (4 - (newBi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // size is height * width * size of pixel plus metadata
    newBf.bfSize = (newBi.biWidth * sizeof(RGBTRIPLE) + outputPadding) * absoluteHeight + 54;

    // calculate new total size of image in bytes (including padding)
    newBi.biSizeImage = (newBi.biWidth * sizeof(RGBTRIPLE) + outputPadding) * absoluteHeight;

    // write new outfile's BITMAPFILEHEADER
    fwrite(&newBf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write new outfile's BITMAPINFOHEADER
    fwrite(&newBi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int inputPadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // repeat this n times for each line
        for (int j = 0; j < n; j++)
        {
            // iterate over pixels in scanline
            for (int k = 0; k < bi.biWidth; k++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write each pixel n times
                for (int l = 0; l < n; l++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
                /* PIXEL IN PIXELS HAS BEEN WRITTEN N TIMES */
            }
            /* SCANLINE IN LINES HAS BEEN WRITTEN ONCE */

            // add padding to the end of the line
            for (int m = 0; m < outputPadding; m++)
            {
                fputc(0x00, outptr);
            }

            // if this is not last iteration, reset cursor to beginning
            if (j < n - 1)
            {
                fseek(inptr, -bi.biWidth * sizeof(RGBTRIPLE), SEEK_CUR);
            }
            else
            {
                // skip over padding, if any
                fseek(inptr, inputPadding, SEEK_CUR);
            }
        }
        /* SCANLINE IN LINES HAS BEEN WRITTEN N TIMES */
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
