#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// The code below simulates how RMS algorithm works with processe A and B.
// Please read it in detail and understand how it works.
// Add your own code to make it be able to handle preemptive situations.
// Realize EDF algorithm based on this implementation.

int main() {
    int periodA, periodB;           // period (relative deadline) of A and B
    int execTimeA, execTimeB;       // worst-case execution time of A and B
    int absDeadlineA, absDeadlineB; // absolute deadline of A and B
    float cpuUtil, u_a, u_b, n = 0, uA = 0, uB = 0;                 // CPU utilization
    int jA = -1, jB = -1;           // indices of jobs
    int doA = 0, doB = 0;           // switches between A and B (e.g. doA == 1 && doB == 0, run A)
    int tA = 0, tB = 0;             // accumulated execution time
    int T;                          // simulated time

    // Input handling and reading
    printf("\t\t\t---------------------------------------\n");
    printf("\t\t\tRate Monotonic Schedule (RMS) Algorithm\n");
    printf("\t\t\t---------------------------------------\n");
    printf("please input period and execution for A process\ndefault: 25, 10: ");
    scanf("%d%d", &periodA, &execTimeA);
    printf("please input period and execution for B process\ndefault: 60, 15: ");
    scanf("%d%d", &periodB, &execTimeB);

    // Your code here to calculate & print CPU utilization
    // Hint: use the definition of utilization
    u_a = (float)execTimeA / periodA;
    u_b = (float)execTimeB / periodB;
    cpuUtil = u_a + u_b;
 
    printf("\nCPU Utilization: %f", cpuUtil);
    // End of your code
    
    absDeadlineA = periodA, absDeadlineB = periodB;
    printf("\nsimulation started\n");
    int numJobs = -1;
    for (T = 0; T <= 200; T++) {
        // Your code here to check if CPU can schedule the task set
        // Hint: exit if deadline is missed

        n = (float)(numJobs);
        float ub = n * ((pow(2.0, 1/n)) - 1);

        uA = (float)tA/periodA;   
        uB = (float)tB/periodB;
        float u = uA + uB;
       
        if(!doA && u > ub) {
            printf("\nProcess A missed deadline. Not schedulable\n");
            return EXIT_FAILURE;
        }
        if(!doB && u > ub) {
            printf("\nProcess B missed deadline. Not schedulable\n");
            return EXIT_FAILURE;
        }

        // End of your code

        // process A is done
        if (tA == execTimeA && doA == 1) {
            printf("when T=%d, ", T);
            printf("process A%d is done\n", jA);
            doA = 0;
            // resume possibly suspended process B
            if (tB < execTimeB) {
                printf("when T=%d, ", T);
                printf("program switched to run process B%d!\n", jB);
                doB = 1;
            }
        }

        // process B is done 
        if (tB == execTimeB && doB == 1) {
            printf("when T=%d, ", T);
            printf("process B%d is done\n", jB);
            doB = 0;
            // resume possibly suspended process A
            if (tA < execTimeA) {
                printf("when T=%d, ", T);
                printf("program switched to run process A%d!\n", jA);
                doA = 1;
            }
        }

        // new instances of process A and B are generated together
        if (T % periodA == 0 && T % periodB == 0) {
            printf("when T=%d, process A%d and B%d are generated together\n", T, ++jA, ++jB);
            ++numJobs;
            ++numJobs;
            absDeadlineA = T + periodA;
            absDeadlineB = T + periodB;
            // RMS: higher rate (1/period) ==> higher priority
            if (periodA <= periodB) {
                printf("when T=%d, program switched to run process A%d!\n", T, jA);
                doA = 1;
                doB = 0;
            } else {
                printf("when T=%d, program switched to run process B%d!\n", T, jB);
                doA = 0;
                doB = 1;
            }
            tA = 0;
            tB = 0;
        }

        // a new instance of process A is generated
        if (T % periodA == 0 && T % periodB != 0) {
            printf("when T=%d, process A%d is generated\n", T, ++jA);
            ++numJobs;
            absDeadlineA = T + periodA;
            tA = 0;
            if (tB < execTimeB) { // process B is unfinished yet
                // Your code here to handle preemption
                // Please print out the decision
                printf("when T=%d, program switched to run process A%d!\n", T, jA);
                doB = 0;
                doA = 1;
                // End of your code
            } else { // process B is done, just run A
                printf("when T=%d, program switched to run process A%d!\n", T, jA);
                doA = 1;
            }
        }

        // a new instance of process B is generated
        if (T % periodA != 0 && T % periodB == 0) {
            printf("when T=%d, process B%d is generated\n", T, ++jB);
            ++numJobs;
            absDeadlineB = T + periodB;
            tB = 0;
            if (tA < execTimeA) { // process A is unfinished yet
                // Your code here to handle preemption
                // Please print out the decision
                printf("when T=%d, program switched to run process B%d!\n", T, jB);
                doA = 0;
                doB = 1;
                // End of your code
            } else { // process A is done, just run B
                printf("when T=%d, program switched to run process B%d!\n", T, jB);
                doB = 1;
            }
        }
        
        // accumulate running time of process A or B
        if (doA) {
            tA++;
        }
        if (doB) {
            tB++;
        }
    }
}