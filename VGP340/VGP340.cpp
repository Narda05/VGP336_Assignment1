// VGP340.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <omp.h>
#include <iostream>
#include <chrono>
#include <numeric>
#include <random>
#include <iomanip> 

#include <memory>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <assert.h>


void Exercises1()
{
	int testValue = 0;
	//omp_set_num_threads(4);
	#pragma omp parallel num_threads(4)
	{
		int  id = omp_get_thread_num();
		++testValue;
		printf(" Hello(%d, %d)", id, testValue);
		++testValue;
		printf(" World(%d, %d)\n", id, testValue);
	}
}

//Serial PI Program
#define NUM_THREADS 4
static long numSteps = 1000000;
double step = 0.0;
void Exercises2()
{
	int numThreads = 0;
    double x = 0.0;
    double pi = 0.0;
    double sum[NUM_THREADS] = { 0.0 };
    step = 1.0 / (double)numSteps;
	omp_set_num_threads(NUM_THREADS); // sets the number of parallel threads
    #pragma omp parallel // copies the code for each thread
    {
        int i = 0;
        int id = omp_get_thread_num();
		int nThreads = omp_get_num_threads();
        if ( id == 0)
        {
            numThreads = nThreads;
        }
        for (i = id; i < numSteps; i += nThreads)
        {
            x = (i + 0.5) * step;
            sum[id] = sum[id] + (4.0 / (1.0 + x * x));
        }
    }
    for (int i = 0; i < numThreads; ++i)
    {
		std::cout << "ID: " << i << " Sum: " << sum[i] << "\n";
        pi += step * sum[i];
    }
    std::cout << "PI: " << pi << "\n";

    return;
}


// SPMD Pi wothout false sharing
void Exercises3()
{
    double pi = 0.0;
    //double sum[NUM_THREADS] = { 0.0 }; // This causes "false sharing"
    step = 1.0 / (double)numSteps;
    omp_set_num_threads(NUM_THREADS); // sets the number of parallel threads
#pragma omp parallel // copies the code for each thread
    {
        double x = 0.0;
        double sum = 0.0;
        int i = 0;
        int id = omp_get_thread_num();
        int nThreads = omp_get_num_threads();
        for (i = id; i < numSteps; i += nThreads)
        {
            x = (i + 0.5) * step;
            sum += (4.0 / (1.0 + x * x)); // no array, so no false sharing
        }
        #pragma omp critical // letting thread know a shared value is being updated
        {
            std::cout << "ID: " << i << " Sum: " << sum << "\n";
            pi += sum * step;
            std::cout << "ID: " << i << " current PI: " << pi << "\n";
        }
    }
    std::cout << "PI: " << pi << "\n";

    return;
}


// Solution 
void Exercises4()
{
    double x = 0.0;
    double pi = 0.0;
	double sum = 0.0;
    step = 1.0 / (double)numSteps;
    omp_set_num_threads(NUM_THREADS); // sets the number of parallel threads
    #pragma omp parallel for private (x) reduction(+:sum) // copies the code for each thread, copies the x, combines the sun at the end.
        for (int i = 0; i < numSteps; ++i)
        {
            x = (i + 0.5) * step;
            sum += (4.0 / (1.0 + x * x)); // no array, so no false sharing
        }
  
    pi = sum * step;
    std::cout << "PI: " << pi << "\n";

    return; 

}


// Assignment 1 allso Serial PI Program
void Assignment1()
{
	std::cout << "Enter Num Steps: ";
	std::cin >> numSteps;

    double x = 0.0;
    double pi = 0.0;
    double sum = 0.0;
    step = 1.0 / (double)numSteps;
    // the current cpu time in  utc
	std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    for (int i = 0; i < numSteps; ++i)
    {
        x = (i + 0.5) * step;
        sum += (4.0 / (1.0 + x * x)); // no array, so no false sharing
    }

    pi = sum * step;
	std::cout << "PI: " << pi << "\n";
	std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
	int duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
	std::cout << "Duration: " << duration << " ms\n";

    return;
}

// Serial PI Program
double SerialPI_Integration(int n)
{
    double step = 1.0 / (double)n;
    double sum = 0.0;

    for (int i = 0; i < n; ++i) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    return sum * step;
}

//OpenMP to parallelize 
double ParallelPI_Integration(int n)
{
    double step = 1.0 / (double)n;
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; ++i) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    return sum * step;
}

// Add this function above main() or at the top of your file
double RandRange(double min, double max)
{
    return min + (max - min) * (static_cast<double>(std::rand()) / RAND_MAX);
}

//Synchronization: Atomic
void Exercises5()
{ 
    std::cout << "Enter Num Steps: ";
    std::cin >> numSteps;

    double x = 0.0;
    double pi = 0.0;
    double sum = 0.0;
    step = 1.0 / (double)numSteps;

    // the current cpu time in utc
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    for (int i = 0; i < numSteps; ++i)
    {
        x = (i + 0.5) * step;
        sum += (4.0 / (1.0 + x * x));
    }

    pi = sum * step;
    std::cout << "PI: " << pi << "\n";
    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
    int duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    std::cout << "Duration: " << duration << "\n";

}

// all the int main() 
 
// Synchronization: Atomic main
//int main()
//{
//    double pi = 0.0;
//    //double sum[NUM_THREADS] = { 0.0 }; // This causes "false sharing"
//    step = 1.0 / (double)numSteps;
//    omp_set_num_threads(NUM_THREADS); // sets the number of parallel threads
//#pragma omp parallel // copies the code for each thread
//    {
//        double x = 0.0;
//        double sum = 0.0;
//        int i = 0;
//        int id = omp_get_thread_num();
//        int nThreads = omp_get_num_threads();
//        for (i = id; i < numSteps; i += nThreads)
//        {
//            x = (i + 0.5) * step;
//            sum += (4.0 / (1.0 + x * x)); // no array, so no false sharing
//        }
//        std::cout << "ID: " << i << " Sum: " << sum << "\n";
//        #pragma omp atomic // prevent other threads from updating "pi" value
//            pi += sum * step;
//        std::cout << "ID: " << i << " current PI: " << pi << "\n";
//    }
//    std::cout << "PI: " << pi << "\n";
//
//    return 0;
//}

//
//int main()
//{
//	//Exercises1();
//	//Exercises2();
//  //Exercises3();
//	//Exercises4();
//	//Assignment1();
//
//    int steps[] = { 10000, 100000, 1000000 };
//    omp_set_num_threads(4); // puedes cambiar esto
//
//    for (int i = 0; i < 3; ++i) {
//        int n = steps[i];
//
//        // SERIAL
//        auto start1 = std::chrono::steady_clock::now();
//        double pi_serial = SerialPI_Integration(n);
//        auto end1 = std::chrono::steady_clock::now();
//
//        double time_serial = std::chrono::duration<double, std::milli>(end1 - start1).count();
//
//        // PARALLEL
//        auto start2 = std::chrono::steady_clock::now();
//        double pi_parallel = ParallelPI_Integration(n);
//        auto end2 = std::chrono::steady_clock::now();
//
//        double time_parallel = std::chrono::duration<double, std::milli>(end2 - start2).count();
//
//        // SPEEDUP
//        double speedup = time_serial / time_parallel;
//
//        std::cout << "n = " << n << "\n";
//        std::cout << "Serial PI     = " << pi_serial     << "\n";
//        std::cout << "Parallel PI   = " << pi_parallel   << "\n";
//        std::cout << "Serial Time   = " << time_serial   << " \n";
//        std::cout << "Parallel Time = " << time_parallel << "\n";
//        std::cout << "Speedup       = " << speedup       << "\n\n";
//    }
//
//    return 0;
//
//}


// ---------------- Assignment 2 (Matrix) -----------------

struct Matrix
{
    int nRows, nCols;
    long long** data;

    Matrix(int r, int c) : nRows{ r }, nCols{ c }
    {
        data = new long long* [nRows];
        for (int i = 0; i < nRows; ++i)
        {
            data[i] = new long long[nCols];
        }
    }
    ~Matrix()
    {
        for (int i = 0; i < nRows; ++i)
            delete[] data[i];
        delete[] data;
    }
    // initialize the matrix by random numbers
    void init()
    {
        std::srand(std::time(0));
        for (int i = 0; i < nRows; ++i)
            for (int j = 0; j < nCols; ++j)
                data[i][j] = std::rand() % 20 - 10;
    }

};
//Serial
void MultSerial(const Matrix& A, const Matrix& B, Matrix& R)
{
    //checking if A's num of Col is equal to B's num of Row
    assert(A.nCols == B.nRows);

    // do the multiplication
    for (int i = 0; i < A.nRows; ++i)
    {
        for (int j = 0; j < B.nCols; ++j)
        {
            long long sum = 0;
            for (int k = 0; k < A.nCols; ++k)
                sum += A.data[i][k] * B.data[k][j];

            R.data[i][j] = sum;
        }
    }   


}

// parallel
void MultParallel(const Matrix& A, const Matrix& B, Matrix& R)
{
    assert(A.nCols == B.nRows);

#pragma omp parallel for
    for (int i = 0; i < A.nRows; ++i)
    {
        for (int j = 0; j < B.nCols; ++j)
        {
            long long sum = 0;
            for (int k = 0; k < A.nCols; ++k)
                sum += A.data[i][k] * B.data[k][j];
    
            R.data[i][j] = sum;
        }
    }
}

// First 5 rows and 5 columns 
void print5x5(const Matrix& M)
{
    for (int i = 0; i < 5 && i < M.nRows; ++i)
    {
        for (int j = 0; j < 5 && j < M.nCols; ++j)
            std::cout << M.data[i][j] << "\t";

        std::cout << "\n";
    }
    std::cout << "\n";
}

bool compare(const Matrix& A, const Matrix& B)
{
    for (int i = 0; i < A.nRows; ++i)
        for (int j = 0; j < A.nCols; ++j)
            if (A.data[i][j] != B.data[i][j])
                return false;

    return true;
}

void ExercisesMatrix()
{
    int size = 1000;
    Matrix A(size, size);
    Matrix B(size, size);
    Matrix C1(size, size);
    Matrix C2(size, size);

    A.init();
    B.init();

    // serial 
    double start = omp_get_wtime();
    MultSerial(A, B, C1);
    double end = omp_get_wtime();

    std::cout << "Serial Time: " << (end - start) << " sec\n";

    // parallel
    start = omp_get_wtime();
    MultParallel(A, B, C2);
    end = omp_get_wtime();

    std::cout << "Parallel Time: " << (end - start) << " sec\n";

    // print 5x5 
    std::cout << "\nSerial (5x5):\n";
    print5x5(C1);

    std::cout << "Parallel (5x5):\n";
    print5x5(C2);

    // Verify
    if (compare(C1, C2))
        std::cout << "Results MATCH \n";
    else
        std::cout << "Results DO NOT MATCH \n";

}
int main()
{
    ExercisesMatrix();
    return 0;
}



// Multiparallel Monte Carlo PI estimation

//int main()
//{
//	long numTrials = 1000000;
//    long i = 0;
//	long nCirc = 0;
//	double pi = 0.0;
//	double x = 0.0;
//	double y = 0.0;
//	double r = 1.0; //7 radius of circle Side of square is 2*r.
//	std::srand(time(0));
//
//#pragma omp parallel for private(x, y) reduction(+:nCirc)
//    for(i = 0; i < numTrials; ++i)
//    {
//        x = RandRange(-r, r);
//		y = RandRange(-r, r);
//        if((x * x + y * y) <= r * r)
//        {
//            ++nCirc;
//		}
//	}
//
//	pi = 4.0 * (static_cast<double>(nCirc) / static_cast<double>(numTrials));
//    std::cout << std::setprecision(20) << "PI: " << pi << "NumHits: " << nCirc << "\n";
//
//    return 0;
//}

//int main()
//{
//	omp_set_num_threads(4);
//#pragma op parallel
//    {
//        #pragma omp sections
//        {
//            int id = omp_get_thread_num();
//            for (int i = 0; i < 10; ++i)
//            {
//				std::cout << "Section 1 for ID: " << id << " index: " << i << "\n";
//            }
//
//        }
//        #pragma omp section
//        {
//			int id = omp_get_thread_num();
//            for (int i = 0; i < 10; ++i)
//            {
//                std::cout << "Section 2 for ID: " << id << " index: " << i << "\n";
//			}
//        }
//    }
//}


