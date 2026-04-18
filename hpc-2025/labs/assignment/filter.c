#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

/**
 * @brief Reads PGM (portable GrayMap) image
 * 
 * The routine will allocate (malloc) the `img` buffer, this must be 
 * deallocated explicitly once it is no longer necessary
 * 
 * @note  Only bit depths of up to 255 are supported
 * 
 * @param img       Output image
 * @param dims      Image dimensions
 * @param fname     File name
 * @return int 
 */
int read_pgm( uint8_t ** img, int dims[], char * fname ) {
    FILE *fp = fopen( fname, "r");
    if ( !fp ) {
        perror("Unable to open file\n" );
        return -1;
    }

    char buffer[1024];

    // Check magic
    fread( buffer, 1, 3, fp );
    if ( buffer[0] != 'P' || buffer[1] != '5' || buffer[2] != '\n') {
        fprintf(stderr, "Unsupported file format\n");
        return -1;
    }

    // Skip comment, if any
    char comment = 0;
    do {
        fread( &comment, 1, 1, fp );
        if ( comment == '#' ) {
            fscanf( fp, "%[^\n]s", buffer );
            // printf("%s: %s\n", fname, buffer );
        } else {
            fseek( fp, -1, SEEK_CUR );
        }
    } while ( comment == '#' );

    int depth;
    fscanf( fp, "%d %d %d", &dims[0], &dims[1], &depth );

    if ( depth > 255 ) {
        fprintf(stderr, "Unsupported bit depth (%d)\n", depth);
        fclose(fp);
        return -1;
    }

    *img = malloc( dims[0] * dims[1] );
    if ( *img == NULL ) {
        fprintf(stderr, "Unable to allocate memory for file\n");
        fclose(fp);
        return -1;
    }

    fread( *img, 1, dims[0] * dims[1], fp );
    fclose(fp);

    return 0;
}

/**
 * @brief Write PGM (portable GrayMap) image
 * 
 * @note Only bit depths of 8 bits are supported
 * 
 * @param image     Image data
 * @param dims      Image dimensions
 * @param fname     File name
 */
void write_pgm( uint8_t *image, const int dims[], const char * fname ) {
    
    FILE *f = fopen(fname, "wb");
    
    // Write PGM header
    fprintf(f, "P5\n%d %d\n255\n", dims[0], dims[1]);
    // Write image data
    fwrite(image, 1, dims[0] * dims[1], f);
    
    fclose(f);
}

/**
 * @brief Write CSV data file
 * 
 * @param data      1D vector data
 * @param n         Number of elements
 * @param fname     File name
 */
void write_csv( int data[], const int n, const char * fname ) {
    FILE *f = fopen( fname, "w" );
    fprintf(f, "level, count\n");
    for( int i = 0; i < n; i++ ) {
        fprintf(f,"%d,%d\n", i, data[i]);
    }
    fclose(f);
}


/**
 * @brief Apply gaussian blur filter to image
 * 
 * @param img       Image data (input / ouput)
 * @param dims      Image dimensions
 * @param level     Number of filter passes
 * @return          Pointer to image (may be different from input)
 */
uint8_t * gauss( uint8_t *img, const int dims[], int level ) {
    /**
     * @brief Gaussian blur kernel
     * 
     */
    float kernel[3][3] = {
        { 0.0625, 0.125, 0.0625},
        { 0.125,  0.25,  0.125},
        { 0.0625, 0.125, 0.0625}
    };

    uint8_t * tmp = malloc( dims[0] * dims[1] );

    // Repeat for level times
    // Não aplicar paralelismo aqui, pois cada passo depende do resultado do passo anterior
    for( int l = 0; l < level; l++ ) {
        // Apply kernel convolution
        #pragma omp parallel for collapse(2)
        for( int j = 1; j < dims[1]-1; j++ ) {
            for( int i = 1; i < dims[1]-1; i++ ) {
                float f = 0.;
                // No need to parallelize this loop, as it is only 9 iterations
                for( int k1 = -1; k1 <= 1; k1 ++ )
                    for( int k0 = -1; k0 <= 1; k0 ++ ) {
                        f += img[ (j + k1) * dims[0] + (i+k0) ] * kernel[k1+1][k0+1];
                    }
                tmp[ j * dims[0] + i ] = f;
            }
        }

        // Swap image buffers
        {
            uint8_t * swap = img;
            img = tmp;
            tmp = swap;
        }
    }

    free( tmp );
    return img;
}

/**
 * @brief Compute image histogram
 * 
 * The histogram counts how many pixels of a given intensity show up on the
 * image
 * 
 * @param histogram     Histogram data
 * @param img           Image data
 * @param dims          Image dimensions
 */
void hist( int histogram[], uint8_t * restrict img, const int dims[] ) {

    // Set all histogram values to 0
    for( int i = 0; i < 256; i++ ) 
        histogram[i] = 0;

    // Compute histogram
    for( long i = 0; i < dims[0] * dims[1]; i++ ) {
        int value = img[i];
        histogram[ value ] ++;
    }
}

int main( int argc, char * argv[]) {
    if (argc != 2 && argc != 3) {
        printf("Usage: %s <file.pgm> [threads]\n", argv[0]);
        return 1;
    }
    
    int dims[2];
    uint8_t * img;


    /**
     * @brief Number of passes
     * 
     */
    const int level = 32;

    int threads = 1;
    if (argc == 3) {
        threads = atoi(argv[2]);
        if (threads < 1) {
            threads = 1;
        }
    }
    omp_set_num_threads(threads);

    int err = read_pgm( &img, dims, argv[1] );
    if ( ! err ) {

        // Compute histogram
        int histogram[256];
        hist( histogram, img, dims ); 
        write_csv( histogram, 256, "histogram.csv");

        // Apply image filter
        double t0 = omp_get_wtime();
        img = gauss( img, dims, level );
        double t1 = omp_get_wtime();
        printf("gauss() time with %d thread(s): %.6f s\n", threads, t1 - t0);
        write_pgm( img, dims, "filter.pgm");
        free( img );
    }

}