# CControl

CControl is a library written in 100% ANSI C (C89) code. No external libraries and 100% platform independent.
The purpose with this library is to fit advanced tools for really small embedded systems or desktop as well.
Here I have focused on practical numerical methods and selected the methods that works best in practice. It has
been a lot of work finding the best methods and best algorithms.

# CLAPACK, Math Kernel Library, FFTPACK support

This library have different types of libraries. First library is the internal lightweight library that suits small embedded systems. The second library is higher lever library for larger matrices. The third library is libraries that are hardware independent. Set this as your math library inside `defines.h` file. The Math Kernel Library is extremely optimized and it's suitable for operative systems such as Windows, Linux, Mac OS meanwhile the lightweight library or CLAPACK and FFTPACK is suitable for an Arduino, STM32, PIC etc because they are 100% portable. The internal library is suited for really small devices.

| Routines  | Function | Comment | 
| ------------- | ------------- | ------------------- |
| `ssyevd` | `svd.c` | Singular value decomposition for symmetric matrices |
| `ssyevd` | `eig.c` | Eigendecomposition for symmetric matrices |
| `sgeev` | `eig.c` | Eigendecomposition for square matrices |
| `sgessd` | `svd.c` | Singular value decomposition for general matrices |
| `spotrf` | `chol.c` | Cholesky facorization for real symmetric matrices |
| `sgetrf` | `lup.c` | LU factorization with pivoting for square matrices |
| `sgetrs` | `linsolve_lup.c` | Solve linear system `Ax = b` with LU factorization |
| `sposv`  | `linsolve_chol.c`| Solve linear system `Ax = b` with Cholesky factorization |
| `sgetri` | `inv.c` | Inverse by using LU factorization for square matrices |
| `sgeqrf` `sorgqr` | `qr.c`  | QR factorization for general matrices|
| `sgemm` `ssymm` `sgemv` `ssymv` | `mul.c` | Multiplication |
| `sgesvd` | `pca.c` | Principal component analysis |
| `sggev` | `eig_generalized.c` | Generalized eigenvalue problem for non-symmetric matrices |
| `ssygvd` | `eig_generalized.c` | Generalized eigenvalue problem for symmetric matrices |
| `rfftf` | `fft.c` | Compute Fast Fourier Transform 1D |
| `cfftb` | `ifft.c` | Compute Inverse Fast Fourier Transform 1D |
| `rfftf` | `fft2.c` | Compute Fast Fourier Transform 2D |
| `cfftb` | `ifft2.c` | Compute Inverse Fast Fourier Transform 2D |
 
If you want to add more, just download `clapack-3.2.1.zip` from `Netlib` and include necessary `.c` files inside the `Lapack` folder of this project, for supporting your routine.

| Type  | Internal | MLK | CLapack | FFTpack | 
| ------------- | ------------- | ------------------- | ------------------- | ------------------- |
| `Speed` | Slow for large data | Fast for large data | Faster than internal | Very fast | 
| `Portability` | Platform independent | Only Windows/Linux/ Mac OS | Platform independent | Platform independent |
| `Missing` | Eigenvectors for general matrices | Nothing | Nothing | Nothing |
| `Memory requirements` | Lightweight | Large | Medium | Lightweight | Lightweight |

# Computer vision support

CControl have sevral algorithms for computer vision. They are chosen and reduced so they will fit embedded systems.

| Name                         | Model                | Multi classification | Localization          | Noise invariant       | Lummination invariant | Rotation invariant | Scale invariant | Translation invariant | Suitable for |
| ---------------------------- | -------------------- | -------------------- | --------------------- | --------------------- | --------------------- | ------------------ | --------------- | --------------------- | ------------ |
| `Fisherfaces`                | `1-Layer NN`         | `Yes`                | `No`                  | `Yes`                 | `Yes`                 | `No`               | `No`            | `No`                  | `Objects`    |
| `Viola Jones`                | `Haar-Likes`         | `No`                 | `No`                  | `Yes`                 | `Yes`                 | `No`               | `No`            | `No`                  | `Objects`    |
| `Generalized Hough Transform`| `Hough model`        | `No`                 | `Yes`                 | `No`                  | `Yes`                 | `Yes`              | `Yes`           | `Yes`                 | `Objects`    |             
| `Hough Transform`            | `Hough model`        | `Yes`                | `Yes`                 | `Yes`                 | `Yes`                 | `Yes`              | `Yes`           | `Yes`                 | `Lines`      |
| `Harris corner detection`    | `No`                 | `Yes`                | `Yes`                 | `No`                  | `Yes`                 | `Yes`              | `Yes`           | `Yes`                 | `Corners`    |
| `Feature detection`    | `No`                 | `Yes`                | `Yes`                 | `No`                  | `Yes`                 | `Yes`              | `Yes`           | `Yes`                 | `Features`    |


Main focuses are:

- Artificial Intelligence
  - Astar algorithm for quick path finding
  - Point-in-polygon algorithm for checking if a point is inside the area

- Machine Learning
  - Density-Based Spatial Clustering of Applications with Noise
  - Support Vector Machine
  - Kernel for PCA and LDA
  - Neural network
  - Kernel Principal Component Analysis
  - K-means clustering
  - AdaBoost classifier
  
- Control Engineering
  - Kalman filter update
  - Linear Quadratic Integral regulator
  - Model Predictive Control (Ongoing)
  - Model Reference Adaptive Control
  - Transfer function to state space
  - Stability check
  - Continuous to discrete

- Computer Vision
  - Generalized Hough Transform for arbitrary shapes
  - Features From Accelerated Segment Test (FAST)
  - Hough Transform for line detection
  - Harris corner detection
  - Fisherfaces
  - Viola Jones for object detection (not only face detection)

- Image Processing
  - Gaussian filtering for images
  - Robust Principal Component Analysis
  - Sobel filter
  - Harris corner score
  - Portable Gray Map (PGM) reader, print, write and free
  - Image resize
  - Pooling Max, Average and Shape
  - Orientation of a matrix by using Intensity Centroid
  - Integral image
  - Local Binary Pattern
  - Haar-Like features

- Signal Processing
  - Fast Fourier Transform Shift
  - Fast Fourier Transform 1D
  - Fast Fourier Transform 2D
  - Inverse Fast Fourier Transform Shift
  - Inverse Fast Fourier Transform 1D
  - Inverse Fast Fourier Transform 2D
  - Particle filter
  - Cluster filter
  - Filtfilt 
  - Square Root Unscented Kalman Filter
  
- Linear Algebra
  - Balance matrix
  - Cholesky decomposition
  - Cholesky update
  - QR decomposition
  - LUP decomposition
  - Determinant
  - Discrete Lyapunov solver
  - Eigenvalues and eigenvectors
  - Generalized eigenvalue problem
  - Hankel matrix
  - Inverse
  - Dot product
  - Pseudo inverse
  - Linear solver
  - Nonlinear solver
  - Multiplication
  - Singular Value Decomposition
  - Transpose
  - Norm
  - Matrix exponential
  - Rank
  - Convolution matrix multiplication
  - Convolution vector multiplication

- Miscellaneous
  - Angle of two vectors
  - Concatenate
  - Cut matrix
  - Find
  - Matrix symmetric check
  - Insert sub matrix into matrix
  - Pdist2
  - Popcount
  - Print matrix or vector
  - Saturation
  - Sign
  - Value min
  - Value max
  - Sort
  - Sum
  - Unit vector
  - Vector rotation 2D
  - Vector scalar
  - Convert linear indices to subscripts
  - Float <-> uint8_t converter
  - Radian vector
  - Column summation
  - Normalize vector
  - Find unique numbers
  - Data shuffle
  - Union converters
  
- Optimization
  - Linear programming
  - Non-Negative Least Squares
  - Quadratic programming

- Statistics
  - Principal Component Analysis
  - Linear Discriminant Analysis
  - Randn
  - Randi
  - Mean
  - Average of the elements inside a form e.g circle
  - Max
  - Standard deviation
  - Variance
  - Covariance
  - Center
  - Ordinary Procrustes Analysis
  - Compute centroid of a cluster
  - Cluster distance
  - Cluster ratio
  - Normal propability density function
  - Gini impurity

- System Identification
  - Observer Kalman Filter identification
  - Eigensystem Realization Algorithm
  - Recursive Least Square with forgetting factor and kalman filter identification
  - Square Root Unscented Kalman Filter for parameter estimation

```c
/*
 ============================================================================
 Name        : Main.c
 Author      : <Your Name Here>
 Version     : 1.0
 Copyright   : MIT
 Description : Initial template
 ============================================================================
 */

#include "CControl/ccontrol.h"

int main() {
	clock_t start, end;
	float cpu_time_used;
	start = clock();

	/* Your ANSI C logic here - All examples can be found at the folder src/CControl/Documents/Examples */

	end = clock();
	cpu_time_used = ((float) (end - start)) / CLOCKS_PER_SEC;
	printf("\nTotal speed  was %f\n", cpu_time_used);


	return EXIT_SUCCESS;
}
```

# Projects made with CControl

## Fan Controller with Linear Quadratic Integral Control

I have created a controller for a fan. The controller works as it read the temperature sensor and it compare the temperature sensor with the potentiometer, which is the reference set point. If the error is large between the temperature sensor and the potentiometer, then the fan is going to turn on high, or low, depending on if the error is negative or positive. 

The goal with this system is that I'm going to implement this on a heat source. The fan is going to blow warm air onto a object and the object is holding the temperature sensor. If the fan is blowing to much warm air, then the controller is going to turn down the speed of the fan so the temperature of the object meets the reference

![a](https://raw.githubusercontent.com/DanielMartensson/CControl/master/Projects/Fan%20Controller/System.jpg)

I have been using Matavecontrol and Mataveid to estimate and creating the kalman gain matrix and the LQR control law with integral action. I'm the author of both Matavecontrol and Mataveid.

Identification process. Here I have using the Observer Kalman Filter Identification and Eigen System Realization methods.

![a](https://raw.githubusercontent.com/DanielMartensson/CControl/master/Projects/Fan%20Controller/Identification%20and%20simulation/Identification.png)

Simulation process:

![a](https://raw.githubusercontent.com/DanielMartensson/CControl/master/Projects/Fan%20Controller/Identification%20and%20simulation/Simulation.png)


## Model Predictive Integral Temperature Controller

This is a Model Predictive Controller, with integral action. It uses linear programming instead of quadratic programming for the optimization. This controller works well.

This predictive controller have a wiring diagram if you want to build the same controller for your temperature project.

![a](https://raw.githubusercontent.com/DanielMartensson/CControl/master/Projects/Temperature%20Controller/IMPC.jpg)

![a](https://raw.githubusercontent.com/DanielMartensson/CControl/master/Projects/Temperature%20Controller/IMPC2.jpg)

Step answer of first order model.

![a](https://raw.githubusercontent.com/DanielMartensson/CControl/master/Projects/Temperature%20Controller/Step%20answer%20-%20First%20order.png)

## Model Reference Adaptive CAN-bus controller

This is a MRAC project, Model Reference Adaptive Controller. This controls the a Sonceboz stepper motor with CAN-bus J1939-21 protocol. The purpose is to control a big wheel with two multivariable hydraulical valves.

See the movie in the project folder. 

Library for SAE J1939 https://github.com/DanielMartensson/Open-SAE-J1939

![a](https://raw.githubusercontent.com/DanielMartensson/CControl/master/Projects/Stepper%20Motor%20Controller/Picture%20CAN-bus.jpg)


## Square Root Uncented Kalman Filter for state estimation and parameter estimation

This is the latest Uncented Kalman Filter. MATLAB is using the same algorithm. A `.m` file is available at the `SR-UKF` folder.

For state estimation

![a](https://raw.githubusercontent.com/DanielMartensson/CControl/master/Projects/SR-UKF/sr-ukf-state-estimation.png)

For parameter estimation

![a](https://raw.githubusercontent.com/DanielMartensson/CControl/master/Projects/SR-UKF/sr-ukf-parameter-estimation.png)

# How to use this library with Eclipse CDT

I have made so both Microsoft Visual Studio users and Eclipce CDT can use this library without conflicts. 
Here is a tutorial how to configure your Eclipse CDT for this repository.

1. I'd recommend `Eclipse IDE` for compiling this software. `Eclipse IDE` is a very boring IDE, but it's robust and trustable. The `Eclipse IDE` is always going to be available for you.

![a](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Eclipse%20installation%20intro.png?raw=true)

2. Once you have `Eclipse IDE` installed. Show Git perspective.

![a](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Eclipse%20git.png?raw=true)

3. Select `Git`

![a](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Select%20git.png?raw=true)

4. Copy ssh 

![a](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Copy%20ssh.png?raw=true)

5. Clone git. Just press next and end with finish

![a](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Clone%20git.png?raw=true)

6. Import project

![a](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Import%20project.png?raw=true)

7. Show view

![a](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Show%20view.png?raw=true)

8. Select `Git staging`

![a](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Git%20staging.png?raw=true)

9. Every time you change a file and you want to update your repository

![a](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Push%20git.png?raw=true)

10. If you don't have a `C-compiler`, here is a [video](https://www.youtube.com/watch?v=D2oAUDPWkkA&ab_channel=DanielGrissom) how to set up a compiler 

11. Begin with your `main.c` file. Then go to `src/CControl/Documents/Examples/` and select a `.txt` file and paste it into `main.c` file and press ![the green circle with a white triangle inside](https://github.com/DanielMartensson/CControl/blob/master/src/CControl/Documents/How%20to%20install/Run.png?raw=true)

12. If you got problem with push your updates to this repository, the you need to configure your authorization. See this [link](https://stackoverflow.com/questions/68790276/pushing-from-eclipse-to-my-github-repository-via-https-stopped-working-git-rec)
 