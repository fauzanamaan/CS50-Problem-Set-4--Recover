#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t BYTE;

#define BLOCK_SIZE 512

int main(int argc, char *argv[])
{
    // Check for correct command-line argument
    if (argc != 2)
    {
        printf("Usage: ./recover IMAGE\n");
        return 1;
    }

    // Open the forensic image
    FILE *raw_file = fopen(argv[1], "r");
    if (raw_file == NULL)
    {
        printf("Could not open forensic image.\n");
        return 1;
    }

    // Initialize variables
    BYTE buffer[BLOCK_SIZE];
    FILE *output_file = NULL;
    int file_count = 0;
    char filename[8]; // ###.jpg + '\0'

    // Read blocks from the forensic image
    while (fread(buffer, 1, BLOCK_SIZE, raw_file) == BLOCK_SIZE)
    {
        // Check for the start of a JPEG
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // Close previous output file
            if (output_file != NULL)
            {
                fclose(output_file);
            }

            // Create new output file
            sprintf(filename, "%03i.jpg", file_count);
            output_file = fopen(filename, "w");
            if (output_file == NULL)
            {
                printf("Could not create output file.\n");
                fclose(raw_file);
                return 1;
            }

            file_count++;
        }

        // Write buffer to output file
        if (output_file != NULL)
        {
            fwrite(buffer, 1, BLOCK_SIZE, output_file);
        }
    }

    // Close the last output file
    if (output_file != NULL)
    {
        fclose(output_file);
    }

    // Close the forensic image
    fclose(raw_file);

    return 0;
}
