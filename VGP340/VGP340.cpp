// VGP340.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <omp.h>
#include <iostream>
#include <chrono>

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

//
int main()
{
	//Exercises1();
	//Exercises2();
    //Exercises3();
	//Exercises4();
	//Assignment1();

    int steps[] = { 10000, 100000, 1000000 };
    omp_set_num_threads(4); // puedes cambiar esto

    for (int i = 0; i < 3; ++i) {
        int n = steps[i];

        // SERIAL
        auto start1 = std::chrono::steady_clock::now();
        double pi_serial = SerialPI_Integration(n);
        auto end1 = std::chrono::steady_clock::now();

        double time_serial = std::chrono::duration<double, std::milli>(end1 - start1).count();

        // PARALLEL
        auto start2 = std::chrono::steady_clock::now();
        double pi_parallel = ParallelPI_Integration(n);
        auto end2 = std::chrono::steady_clock::now();

        double time_parallel = std::chrono::duration<double, std::milli>(end2 - start2).count();

        // SPEEDUP
        double speedup = time_serial / time_parallel;

        std::cout << "n = " << n << "\n";
        std::cout << "Serial PI     = " << pi_serial     << "\n";
        std::cout << "Parallel PI   = " << pi_parallel   << "\n";
        std::cout << "Serial Time   = " << time_serial   << " \n";
        std::cout << "Parallel Time = " << time_parallel << "\n";
        std::cout << "Speedup       = " << speedup       << "\n\n";
    }

    return 0;


	return 0;
}


