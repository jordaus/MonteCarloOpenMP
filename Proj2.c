#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include "sys/types.h"
#include "sys/sysinfo.h"

typedef struct {
        uint32_t virtualMem;
        uint32_t physicalMem;
} processMem_t;

double count_pt(int n){
    /*int local_seed[n];
    for(int x = 0; x < n; x++){
        local_seed[x] = n;
    }*/
    double x, y;
    int i = 0;
    double count_in = 0;
    // int nthreads = omp_get_num_threads();

    #pragma omp parallel private(x, y, i) reduction(+ : count_in) 
        srand48(time(0));
    	#pragma omp for collapse(1)
	 for(i = 0; i < n; i++){
            //double x = (double)erand48(local_seed[i]);
            //double y = (double)erand48(local_seed[i]);
            x = (double)drand48();
            y = (double)drand48();
	    
            if((x*x) + (y*y) <= 1){
		 count_in++;
            }
	} 

        return count_in;
    
}

double pi_cal(int n){
    double pi = 4 * count_pt(n)/n;
    printf("Final Pi Answer: %f\n", pi);
    return pi;
} 

double pi_cal2(int n){
	double pi = 4 * count_pt(n)/n;
	return pi;
}

int parseLine(char *line) {
        // This assumes that a digit will be found and the line ends in " Kb".
        int i = strlen(line);
        const char *p = line;
        while (*p < '0' || *p > '9') p++;
        line[i - 3] = '\0';
        i = atoi(p);
        return i;
}

void GetProcessMemory(processMem_t* processMem) {
        FILE *file = fopen("/proc/self/status", "r");
        char line[128];

        while (fgets(line, 128, file) != NULL) {
                //printf("%s", line);
                if (strncmp(line, "VmSize:", 7) == 0) {
                        processMem->virtualMem = parseLine(line);
                }

                if (strncmp(line, "VmRSS:", 6) == 0) {
                        processMem->physicalMem = parseLine(line);
                }
        }
        fclose(file);
}

int myTime() {
        int i, j=0;
        struct timeval t1, t2;
        double elapsedTime;
        int numSlots, myVersion = 1;

        gettimeofday(&t1, NULL);


        printf("DEBUG: starting loop on %s\n", getenv("HOSTNAME"));

        pi_cal2(10000000);

        gettimeofday(&t2, NULL);


        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
        printf("DATA, %d, %s, %f\n", myVersion, getenv("SLURM_NTASKS"),  elapsedTime);

}

int checkMem(){
    int i;
	processMem_t myMem;

    GetProcessMemory(&myMem);

    printf("Memory: vMem %u KB, pMem %u KB\n", myMem.virtualMem, myMem.physicalMem);
}

main() {
       // init_arrays();
       // count_array();
       // print_results();
	int N = 10000000;
       // count_array2(); //repeat using serial algorithm for debugging - comment out for perf modeling
	 pi_cal(N);
	myTime();
	checkMem();
}

