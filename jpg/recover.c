/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    // open file
    FILE* file = fopen("card.raw", "r");

    // declare buffer
    uint8_t buffer[512];

    // initialize title string
    // note: string needs at least 8 elements: "xxx.jpg\0"
    char title[8];

    // declare counter
    int counter = 0;

    // read 512 bytes
    while(fread(&buffer, sizeof(uint8_t), 512, file) == 512)
    {
        // match jpg signature
        if(buffer[0] == 0xff
        && buffer[1] == 0xd8
        && buffer[2] == 0xff
        && (buffer[3] == 0xe0 || buffer[3] == 0xe1))
        {
            // write counter number to title string
            // note: sprintf will write beyond allocated size of array;
            // prefer snprintf which is limited to sizeof(array)
            snprintf(title, sizeof(title), "%.3d.jpg", counter);

            // open new jpg file
            FILE* img = fopen(title, "a");

            // append buffer to file
            fwrite(&buffer, sizeof(buffer), 1, img);

            // close file
            fclose(img);

            // increment count AFTER title is written
            counter++;
        }
        // if jpg signature is not matched
        else
        {
            // if a jpg file has been found, i.e. cursor is inside jpg
            if(counter > 0)
            {
                // open file with current title
                FILE* img = fopen(title, "a");

                // continue appending to current file
                fwrite(&buffer, sizeof(buffer), 1, img);

                // close file
                fclose(img);
            }
        }
    }
    // close source file
    fclose(file);

    // print number of images found
    printf("%d jpg images found\n", counter);

    return 0;
}
