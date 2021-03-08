#include <stdio.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h> 
#include <inttypes.h> //Using to return char for void* functions. 
#include <sys/wait.h> 


// Function Prototypes
bool validateIncommingArray(int arraySize);
void* validateRows(void *infoStruct);
void* validateCols(void *infoStruct);
void* validateGrids(void *infoStruct);


/* These are the only two global variables allowed in your program */
static int verbose = 0;
static int use_fork = 0;
struct arg_struct {
    int puzzleIdx;
    int puzzleArg[9][9];
}; 



// This is a simple function to parse the --fork argument. It also supports --verbose, -v
void parse_args(int argc, char *argv[]) {
    int c;
    while (1) {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       0, 'v'},
            {"fork",    no_argument,       0, 'f'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long (argc, argv, "vf", long_options, &option_index);
        if (c == -1) break;

        switch (c) {
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



int main(int argc, char *argv[]) {

    parse_args(argc, argv);

    // Define puzzle and the dimensions for the puzzle we will read input into
    int numGroupsToValidate = 9; // each row group, col group, and grid group will have 9 threads/children
    int puzzleRows = numGroupsToValidate;
    int puzzleCols = numGroupsToValidate;
    int puzzle[puzzleRows][puzzleCols];
    int puzzleSize = (sizeof(puzzle) / sizeof(*puzzle)) * (sizeof(puzzle[0]) / sizeof(*puzzle[0])); // = 81
    struct arg_struct *arrayStruct[numGroupsToValidate];


    // Read input into the puzzle matrix
    for (int r = 0; r < puzzleRows; r++) {
        for (int c = 0; c < puzzleCols; c++) {
            scanf("%d",&puzzle[r][c]);
        }
    }


    // Validate successs of input into puzzle matrix.
    if (!validateIncommingArray(puzzleSize)){
        printf( "Failed to populate matrix with input file data." );
        exit(1);
    }


    /***************************************************************************************
    *                                         FORKS                                        *
    ***************************************************************************************/

    if (/*verbose && */use_fork) {

        for ( int g = 0; g < numGroupsToValidate; g++ ) { 
            struct arg_struct *args = malloc( sizeof( *args ) );       // *Struct of args for passing functions multiple params
            memcpy( args->puzzleArg, puzzle, sizeof args->puzzleArg ); // Copies the array to the arg struct's array
            args->puzzleIdx = g;                                       // Index to be checked in functions
            arrayStruct[g] = args;                                     // Add args to an array
        }

        int numProcesses = 27;
        pid_t pid[numProcesses];


        for (int i = 0; i < numProcesses; i++) {
            
            pid[i] = fork();       

            if (pid[i] == 0){
                if (i >= 0 && i < 9)  
                    exit(validateRows(arrayStruct[i%9]) == (void *)'v' ? 1 : 0);                                   
                else if (i >= 9 && i < 17)
                    exit(validateCols(arrayStruct[i%9]) == (void *)'v' ? 1 : 0);
                else if (i >= 17 && i < 27)   
                   exit(validateGrids(arrayStruct[i%9]) == (void *)'v' ? 1 : 0);
                else
                    printf("Error with thread.");
                    exit(0);
            }
        }

        int status = 0, childValidityIndex = 0;
        char childValidities[numProcesses];

        while(wait(&status) > 0){ // Reap zombie children :) - Use for return values
            childValidities[childValidityIndex] = status > 0 ? 'v' : 'i'; 
            childValidityIndex++;
        }

        for (int i = 0; i < numProcesses; i++){
            if (childValidities[i] == 'i'){
                printf("The input is not a valid Sudoku.\n");
                exit(0);
            }
        }
        printf("The input is a valid Sudoku.\n");

    }

    /***************************************************************************************
    *                                       Threads                                        *
    ***************************************************************************************/

    else /*if (verbose)*/ {

        int numThreads = 27;
        pthread_t tids[numThreads];

        for ( int g = 0; g < numGroupsToValidate; g++ ) { 

            struct arg_struct *args = malloc( sizeof( *args ) );       // Pointer to struct of arguments for passing multiple parameters to row/col/grid functions
            memcpy( args->puzzleArg, puzzle, sizeof args->puzzleArg ); 
            args->puzzleIdx = g;
            arrayStruct[g] = args;
            // TO DO: Add the struct pointer to our array that will be defined right before this loop.
            //printf( "...Creating Threads...\n" );
            pthread_create( &tids[g*3], NULL, validateRows, arrayStruct[g]);
            pthread_create( &tids[g*3+1], NULL, validateCols, arrayStruct[g]);
            pthread_create( &tids[g*3+2], NULL, validateGrids, arrayStruct[g]);

        }

        intptr_t validness;
        char validChar = 'v';

        for ( int g = 0; g < numThreads; g++ ) { 
            pthread_join( tids[g], (void **) &validness );
            if(validness == 'i') validChar = 'i';
            //printf( "...Threads Destroyed...\n" );
        }   

        if(validChar == 'i') printf("The input is not a valid Sudoku.\n");
        else printf("The input is a valid Sudoku.\n");

    }


    // Free memory for dynamic array of structs
    for(int i=0; i<numGroupsToValidate; i++){
        free(arrayStruct[i]);
    }  

    return 0;
}




// Takes the size of our 2D array as argument and checks for correct size.
bool validateIncommingArray(int arrSize) {
    return arrSize == 81;
}




void* validateRows(void *infoStruct){

    struct arg_struct *args = (struct arg_struct *)infoStruct; // Casts a struct we can reference from the param
    char isValid = 'v';
    int row = args->puzzleIdx;
    int col = 0;
    int neededValues[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    for ( int i = 0; i < 9; i++ ) { // for each element count the nunmber of times it occurs in the row.
        int elementCount = 0;
        
        for ( int c = 0; c < 9; c++ ) {
            if ( neededValues[i] == args->puzzleArg[row][c] ) {
                elementCount++;
            }
        }

        if ( elementCount != 1 ) {
            printf("Row %d doesn't have the required values.\n", row+1);
            isValid = 'i';
            // WILL NOT RETURN SO WE CAN CHECK EVERYHWERE IT IS NOT VALID
            break;
        }
    }

    intptr_t i = isValid;
    return *(void **)&i;
    //pthread_exit(isValid); // pthead_exit should be used for early termination only https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_exit.html
}




void* validateCols(void *infoStruct){

    struct arg_struct *args = (struct arg_struct *)infoStruct; // Casts a struct we can reference from the param
    char isValid = 'v';
    int col = args->puzzleIdx;
    int row = 0;
    int neededValues[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    for ( int i = 0; i < 9; i++ ) { // for each element count the nunmber of times it occurs in the row.
        int elementCount = 0;
        
        for ( int r = 0; r < 9; r++ ) {
            if ( neededValues[i] == args->puzzleArg[r][col] ) {
                elementCount++;
            }
        }

        if ( elementCount != 1 ) {
            printf("Column %d doesn't have the required values.\n", col+1);
            isValid = 'i';
            // WILL NOT RETURN SO WE CAN CHECK EVERYHWERE IT IS NOT VALID
            //return NULL; 
            break;
        }
    }

    //printf("Col %d VALID!\n", col);
    intptr_t i = isValid;
    return *(void **)&i;
}






void* validateGrids(void *infoStruct) {
    
    char isValid = 'v';
    struct arg_struct *args = (struct arg_struct *)infoStruct; // Casts a struct we can reference from the param
    int row = args->puzzleIdx / 3 * 3;
    int col = args->puzzleIdx % 3 * 3;
    int maxRow = row + 3;
    int maxCol = col + 3;
    int neededValues[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    for ( int i = 0; i < 9; i++ ) { // for each element count the nunmber of times it occurs in the 3x3 square.
        int elementCount = 0;
        for ( int r = row; r < maxRow; r++ ) {
            for ( int c = col; c < maxCol; c++ ) {
                if ( neededValues[i] == args->puzzleArg[r][c] ) {
                    elementCount++;
                }
            }
        }

        if ( elementCount != 1 ) {
            isValid = 'i';
            switch (row) {
                case 0:
                    switch (col) {
                        case 0:
                            printf("The top left subgrid doesn't have the required values.\n");
                            break;
                        case 3:
                            printf("The top middle subgrid doesn't have the required values.\n");
                            break;
                        case 6:
                        printf("The top right subgrid doesn't have the required values.\n");
                    }
                    break;
                case 3:
                    switch (col) {
                        case 0:
                            printf("The middle left subgrid doesn't have the required values.\n");
                            break;
                        case 3:
                            printf("The middle subgrid doesn't have the required values.\n");
                            break;
                        case 6:
                            printf("The middle right subgrid doesn't have the required values.\n");
                    }
                    break;
                case 6:
                    switch (col) {
                        case 0:
                            printf("The bottom left subgrid doesn't have the required values.\n");
                            break;
                        case 3:
                            printf("The bottom middle subgrid doesn't have the required values.\n");
                            break;
                        case 6:
                            printf("The bottom right subgrid doesn't have the required values.\n");

                    }
            }
            break;
        }
    }
    
    intptr_t i = isValid;
    return *(void **)&i;
}