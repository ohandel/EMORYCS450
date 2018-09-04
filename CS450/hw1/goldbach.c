// Collaboration Statement:
// THIS CODE WAS MY OWN WORK , IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS.
// Owen Handel
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BITSPERSEG  (8*256*sizeof(int))

typedef struct _seg {  /* definition of new type "seg"  */
    int bits[256];
    struct _seg *next, *prev;
}seg;

seg *head, *pt;
seg *whichseg(int j); //methods needed for prime bitmap
int whichint(int j);
int whichbit(int j);
int marknonprime(int j);
int testprime(int j);

int main(int argc, char *argv[]) {
    int i, j, k; //variables
    int howmany;
    int N;
    int time = 100;
    if (argc == 2) {
        howmany = (int) strtol(argv[1], (char **)NULL, 10);
    } else {
        printf("Generate the odd primes less than the command line input: "); //input message
        scanf("%d", &howmany);
    }
    N = howmany;
    howmany = (howmany + BITSPERSEG - 1) / BITSPERSEG;
    head = (seg *) malloc(sizeof(seg)); //allocating memory
    pt = head;
    for (i = 1; i <= howmany; i++) { //Just Forward Links for Now
        pt->next = (seg *) malloc(sizeof(seg)); //allocating memory for number of nodes needed
        pt = pt->next;
    }
    pt->next = NULL;
    marknonprime(0); //set bit 0 to be not prime
    marknonprime(1); //set bit 1 to be not prime
    k = 2; //init k
    while (k <= N) { //loop to run through bit array starting at k
        for (i = k; i <= N; i++) { //loop runs from bit k to bit N
            if (testprime(i) == 0) { //tests bit
                break; //if bit is 0 breaks as bit is prime
            }
        }
        for (j = 2 * i; j <= N; j = j + i) { //sieve part of loop finds any multiples of prime
            marknonprime(j); //sets bit to be not prime
        } //iterates through loop by increasing by the found prime each time
        k = i + 1; //increase k by 1
    }
    int count = 0;
    for (i = 3; i <= N; i++) { //counting primes
        if (testprime(i) == 0) { //testing bit if prime count iterates
            count++;
        }
    }
    printf("Found %d odd primes below %d \n", count, N); //prints count
    int input;
    printf("Enter even numbers that are between 5 and N for the goldbach decomposition \n");
    while ((scanf("%d", & input) != -1)) { //program will continue to receive user input
        if (input % 2 != 0) { //must not be even
            printf("Number must be even"); //user error message
            continue;
        }
        if (input > N) { //must not be bigger than N
            printf("Number is too large"); //user error message
            continue;
        }
        int z; //variables
        int znext;
        int difference = input;
        for (i = 2; i <= input; i++) { //runs through bits
            if (testprime(i) == 0) { //must be prime
                z = input - i;
                if (testprime(z) == 0) { //must be prime
                    znext = input - z; //znext is difference between input and z will be a prime
                    if ((znext - z) < difference &&
                        (znext - z) >= 0) { //if difference is greater than zero execute below statement
                        printf("The Goldbach decomposition of %d = %d + %d \n", input, z, znext); //prints decomp
                        break; //breaks loop once decomp has been printed
                    }
                }
            }
        }
    }
    exit(0);
    return 0;
}

int whichint(int j) {
    return (j % BITSPERSEG) / 32; //which int (int is 32 bits or 4 bytes)
}

int whichbit(int j) {
    return (j % BITSPERSEG) % 32; //which bit similar to prev method
}

seg *whichseg(int j) { //returns segment of list
    int i = 0;
    seg *ptr = head;
    while (i < (j / BITSPERSEG)) {
        ptr = ptr->next;
        i += 1;
    }
    return ptr;
}

int marknonprime(int j) { //marks bit non prime
    return (whichseg(j)->bits[whichint(j)] |= 1 << (whichbit(j)));
}

int testprime(int j) { //tests bit
    return (whichseg(j)->bits[whichint(j)] >> whichbit(j) & 1);
}
