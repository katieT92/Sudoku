#include <stdio.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <getopt.h>

/* These are the only two global variables allowed in your program */
static int verbose = 0;
static int use_fork = 0;


// This is a simple function to parse the --fork argument.
// It also supports --verbose, -v
void parse_args(int argc, char *argv[])
{
    int c;
    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       0, 'v'},
            {"fork",    no_argument,       0, 'f'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long (argc, argv, "vf", long_options, &option_index);
        if (c == -1) break;

        switch (c)
        {
            case 'f':
                use_fork = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                exit(1);
        }
    }
}


int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    pthread_t workerThreads[27]; //Amount of threads we will need in total; 
    int currentThread = 0;  //current thread for tracking which one has errors and for sending into the pthread_create function. 


    if (verbose && use_fork) {
        printf("We are forking child processes as workers.\n");


    } else if (verbose) {
        printf("We are using worker threads.\n");

<<<<<<< Updated upstream
        pthread_create(&workerThreads[currentThread++], NULL, isGridValid, infoStruct);  //example of how to call with thread for isGridValid. 


=======
        pid_t pid[27];                                                      // We need 27 forked processes
        char isValid = 'v';                                                 // Tracks if each section is valid.  in child it will pass to the parent. Parent value will be checked to see if Sudoku is Valid or not. 
       for (int i = 0; i < 27; i++) { 
            pid[i] = vfork();                                                //Forks 27 times and adds fork return value to pid array: Changed to vfork to support changing isValid by children.

            if (pid[i] == 0){
                if (i >= 0 && i < 9){                                       
                    if((int*)validateRows(arrayStruct[i%9]) == (void*)'i'){ // 9 forks shold call this function with arrayStruct[0-9]. Compares value returned by the function with invalid, if invalid it will set parent isValid to false.
                        isValid = 'i';
                    }
                    exit(0);
                }
                else if (i >= 9 && i < 17){
                    if((int*) validateCols(arrayStruct[i%9]) == (void*)'i'){ // 9 forks shold call this function with arrayStruct[0-9]. Compares value returned by the function with invalid, if invalid it will set parent isValid to false.
                        isValid = 'i';
                    }                   
                    exit(0);
                }
                else if (i >= 17 && i < 27){    
                   if((int*) validateGrids(arrayStruct[i%9]) == (void*)'i'){  // The last 9 forks shold call this function with arrayStruct[0-9]. Compares value returned by the function with invalid, if invalid it will set parent isValid to false.
                        isValid = 'i';
                    }                  
                    exit(0);
                }  
                else{
                    printf("Error with thread.");
                    exit(1);
                } 
            }
        }
        while(wait(NULL) != -1); //Helps reap zombie children :) - Use for return values

         if(isValid == 'i') printf("The input is not a valid Sudoku.\n"); //Tracks parent isValid which will be adjusted by children forks with vfork().
        else printf("The input is a valid Sudoku.\n");

>>>>>>> Stashed changes
    }


    return 0;
}

bool validateIncommingArray(int inputArray[]) {

    int n = sizeof(inputArray) / sizeof(inputArray[0]);  //Takes array and divides it by the first element to get the number of array elements. 

    if (n == 81) { return true; }; // returns true if the array elements is 81, returns false (Invalid input) if not 81 elements. 
    return false; 

}

void* isGridValid(void *infoStruct) {

    int validArray[9] = 0; //Validation Array to check if number is proper. 

    for (int i = row; i < row + 3; i++) {  //parse through row of information for 3 x 3 grid
        for (int j = col; j < col + 3; j++) { //parse through Col of information for 3 x 3 grid

            if (*What are we looking for here ?  ) { //If not valid entry in sudoku, Check if number is lower than 1 or greater than 9, then check if we already have that number. 

                //If inside here, it isn't valid and we should flag it's col and row, return by closing thread as it wont hit the close thread at the end of the function. 
                pthread_exit(NULL);
            }
            else {  //Is valid entry thus continues loop. 


            }


        }
    }


    pthread_exit(NULL);
}
