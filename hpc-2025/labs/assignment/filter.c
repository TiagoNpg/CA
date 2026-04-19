#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

/*
 * Membros do grupo:
 * 1. Francisco Ribeiro - 123930
 * 2. Tiago Nunes - 123298
 */

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
 * @brief Write gauss benchmark CSV table
 *
 * @param times         Execution times indexed by thread count
 * @param max_threads   Maximum thread count
 * @param fname         File name
 */
void write_benchmark_csv( const double times[], int max_threads, const char * fname ) {
    FILE *f = fopen( fname, "w" );
    if ( !f ) {
        fprintf(stderr, "Unable to write benchmark CSV: %s\n", fname);
        return;
    }

    fprintf(f, "threads,time_s,speedup\n");
    for ( int t = 1; t <= max_threads; t++ ) {
        double speedup = times[1] / times[t];
        fprintf(f, "%d,%.9f,%.9f\n", t, times[t], speedup);
    }
    fclose(f);
}

/**
 * @brief Write gnuplot script for speedup plot
 *
 * @param csv_file      Input benchmark CSV
 * @param script_file   Output script file
 */
void write_gnuplot_script( const char * csv_file, const char * script_file ) {
    FILE *f = fopen( script_file, "w" );
    if ( !f ) {
        fprintf(stderr, "Unable to write gnuplot script: %s\n", script_file);
        return;
    }

    fprintf(f, "set terminal pngcairo size 900,600\n");
    fprintf(f, "set output 'gauss_speedup.png'\n");
    fprintf(f, "set datafile separator ','\n");
    fprintf(f, "set title 'gauss() Speedup'\n");
    fprintf(f, "set xlabel 'Threads'\n");
    fprintf(f, "set ylabel 'Speedup'\n");
    fprintf(f, "set grid\n");
    fprintf(f, "set key top left\n");
    fprintf(f, "plot '%s' using 1:3 with linespoints lw 2 pt 7 title 'Measured', x with lines dt 2 title 'Ideal'\n", csv_file);
    fclose(f);
}

/**
 * @brief Print benchmark table and an ASCII speedup plot
 *
 * @param times         Execution times indexed by thread count
 * @param max_threads   Maximum thread count
 */
void print_benchmark_report( const double times[], int max_threads ) {
    printf("\nGauss benchmark (1..%d threads)\n", max_threads);
    printf("threads,time_s,speedup\n");
    for ( int t = 1; t <= max_threads; t++ ) {
        double speedup = times[1] / times[t];
        printf("%d,%.6f,%.3f\n", t, times[t], speedup);
    }

    printf("\nASCII speedup plot\n");
    for ( int t = 1; t <= max_threads; t++ ) {
        double speedup = times[1] / times[t];
        int bars = (int) lround( speedup * 10.0 );
        printf("%2d | ", t);
        for ( int i = 0; i < bars; i++ )
            putchar('#');
        printf(" (%.2fx)\n", speedup);
    }
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

    long npixels = (long) dims[0] * dims[1];
    int nthreads = omp_get_max_threads();
    int * private_hist = calloc( (size_t) nthreads * 256, sizeof(int) );

    if ( private_hist == NULL ) {
        // Fallback to serial histogram if memory allocation fails.
        for( long i = 0; i < npixels; i++ ) {
            int value = img[i];
            histogram[value]++;
        }
        return;
    }

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int * local_hist = &private_hist[tid * 256];

        #pragma omp for
        for( long i = 0; i < npixels; i++ ) {
            int value = img[i];
            local_hist[value]++;
        }
    }

    for( int t = 0; t < nthreads; t++ ) {
        int * local_hist = &private_hist[t * 256];
        for( int i = 0; i < 256; i++ ) {
            histogram[i] += local_hist[i];
        }
    }

    free( private_hist );
}

/**
 * @brief Compute image histogram (serial reference)
 *
 * @param histogram     Histogram data
 * @param img           Image data
 * @param dims          Image dimensions
 */
void hist_serial( int histogram[], uint8_t * restrict img, const int dims[] ) {
    for( int i = 0; i < 256; i++ )
        histogram[i] = 0;

    long npixels = (long) dims[0] * dims[1];
    for( long i = 0; i < npixels; i++ ) {
        int value = img[i];
        histogram[value]++;
    }
}

/**
 * @brief Compare two histograms
 *
 * @param a     Histogram A
 * @param b     Histogram B
 * @return      1 if equal, 0 otherwise
 */
int hist_equal( const int a[], const int b[] ) {
    for ( int i = 0; i < 256; i++ ) {
        if ( a[i] != b[i] )
            return 0;
    }
    return 1;
}

int main( int argc, char * argv[]) {
    if (argc != 2 && argc != 3) {
        printf("Usage: %s <file.pgm> [threads]\n", argv[0]);
        printf("  - With [threads]: single gauss run with that thread count\n");
        printf("  - Without [threads]: run gauss benchmark from 1 to 8 threads\n");
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
    int benchmark_mode = (argc == 2);
    const int max_bench_threads = 8;
    if (argc == 3) {
        threads = atoi(argv[2]);
        if (threads < 1) {
            threads = 1;
        }
    }
    omp_set_num_threads(threads);

    int err = read_pgm( &img, dims, argv[1] );
    if ( ! err ) {

        // Compute serial and parallel histograms and compare for correctness.
        int histogram[256];
        int histogram_serial[256];
        hist_serial( histogram_serial, img, dims );
        hist( histogram, img, dims );
        write_csv( histogram, 256, "histogram.csv");
        write_csv( histogram_serial, 256, "histogram_serial.csv" );

        if ( hist_equal( histogram, histogram_serial ) ) {
            printf("hist() correctness check: OK (parallel == serial)\n");
        } else {
            printf("hist() correctness check: FAILED (parallel != serial)\n");
        }

        long npixels = (long) dims[0] * dims[1];

        if ( benchmark_mode ) {
            double times[max_bench_threads + 1];

            for ( int t = 1; t <= max_bench_threads; t++ ) {
                uint8_t * img_work = malloc( (size_t) npixels );
                if ( !img_work ) {
                    fprintf(stderr, "Unable to allocate image copy for benchmark\n");
                    free( img );
                    return 1;
                }
                memcpy( img_work, img, (size_t) npixels );

                omp_set_num_threads(t);
                double t0 = omp_get_wtime();
                uint8_t * filtered = gauss( img_work, dims, level );
                double t1 = omp_get_wtime();
                times[t] = t1 - t0;

                if ( t == max_bench_threads ) {
                    write_pgm( filtered, dims, "filter.pgm" );
                }
                free( filtered );
            }

            print_benchmark_report( times, max_bench_threads );
            write_benchmark_csv( times, max_bench_threads, "gauss_benchmark.csv" );
            write_gnuplot_script( "gauss_benchmark.csv", "gauss_speedup.gnuplot" );
            printf("Wrote gauss_benchmark.csv and gauss_speedup.gnuplot\n");
            printf("To generate plot PNG: gnuplot gauss_speedup.gnuplot\n");
        } else {
            // Apply image filter (single run)
            omp_set_num_threads(threads);
            double t0 = omp_get_wtime();
            img = gauss( img, dims, level );
            double t1 = omp_get_wtime();
            printf("gauss() time with %d thread(s): %.6f s\n", threads, t1 - t0);
            write_pgm( img, dims, "filter.pgm");
            free( img );
            return 0;
        }

        free( img );
    }

}