# Advanced Computing
## Project 1 - OpenMP

### Introduction

The project focuses on two common tasks in image processing, namely image filtering and histogram generation. We will be using "grayscale" images, which specify only light intensity using 1 byte per pixel, ranging from black (0) to white (255).

The filtering is performed with a standard technique that performs a convolution between a kernel (a small 3×3 matrix in our case) and the image. Each pixel in the output image is a function of the nearby pixels (including itself) using the kernel values as weights. The code does 32 passes of the so called "Gaussian blur" kernel. The histogram calculation computes the number of pixels in the image that have a particular light intensity. This is achieved by looping through all pixels in the image and accumulating the histogram bin corresponding to the intensity of each pixel. The provided `filter.c` code implements these tasks in the `gauss()` and `hist()` functions.


Note that this is a group assignment. Students must work in groups of two elements. Groups may be formed by any two students enrolled in the Advanced Computing course.

### Goals

The goal of this project is to parallelize the provided serial version of the image processing code using **OpenMP**, and to analyze the performance of the parallel program by measuring the execution time for an increasing number of threads.

### Implementation

Start by downloading the project file available in the e-learning (Moodle) platform and decompress it into the `labs` folder inside your Docker course image. The following files are provided:

+ `filter.c` - Serial version of the image processing code;
+ `dog8.pgm` - Small image for testing purposes;
+ `alex8.pgm` - Large image for performance measuring;
+ `filter.ipynb` - Simple jupyter notebook for visualizing the images and histogram. 

Once compiled, the `filter.c` program expects a single command line parameter with the name of the image file that is to be processed, e.g.:

```bash
$ ./filter dog8.ppm
```

The program will always save the results in the same files, namely `filter.pgm` (filtered image), and `histogram.csv` (histogram). If you have python (and required dependencies) installed on your system, you may use the supplied `filter.ipynb` to visualize your results.

#### 1. Paralelize the `gauss()` function

Start by identifying what particular loops inside the function are eligible for OpenMP parallelization. Test the parallel version with the `dog.pgm` file, and check for correctness.

#### 2. Add code to time the execution of the `gauss()` function

Time the execution of the `gauss()` function using the `alex8.pgm` file using 1 to (at least) 8 OpenMP threads. Compute the speedup for each of the situations. Results must be presented in both table and plot form.

#### 3. Paralelize the `hist()` function

Try to find the parallelization scheme that gives optimal performance. This may require rewriting the `hist()` function. Test the parallel version with the `dog.pgm` file, and check for correctness by comparing the resulting `histogram.csv` file with the result from the serial version.

### Documents to submit

The submission of the project must include both the parallel version of the serial code and a short report (maximum 2 pages) discussing the parallelization strategy followed for each of the image processing tasks, and discussing the speedup obtained from the serial version by the parallel code for the image filtering task. Remember to include a brief description of the hardware used, including the processor type and number of cores.

Both the report and the source file must clearly identify the two members of the group, including name and student number. Please submit both files to the e-learning (Moodle) platform in the section made available for this purpose (Avaliação → Projecto 1 - OpenMP). 

__Do not submit the test images__.