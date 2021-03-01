#include <stdio.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h> 

// Function Prototypes
bool validateIncommingArray(int arraySize);
void* validateRows(void *infoStruct);
void* validateCols(void *infoStruct);
void* validateGrids(void *infoStruct);

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

struct arg_struct {
    int puzzleIdx;
    int puzzleArg[9][9];
}; 


int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    if (verbose && use_fork) {
        printf("We are forking child processes as workers.\n");
    } else if (verbose) {
        printf("We are using worker threads.\n");
    }

    // Define puzzle and the dimensions for the puzzle we will read input into
    int puzzleRows = 9;
    int puzzleCols = 9;
    int puzzle[puzzleRows][puzzleCols];
    int puzzleSize = (sizeof(puzzle) / sizeof(*puzzle)) * (sizeof(puzzle[0]) / sizeof(*puzzle[0])); // = 81


    // Read input into the puzzle matrix
    for (int r = 0; r < puzzleRows; r++) {
        for (int c = 0; c < puzzleCols; c++) {
        scanf("%d",&puzzle[r][c]);
        }
    }


    // Validate successs of input into puzzle matrix.
   if (validateIncommingArray(puzzleSize)){
        
        int numGroupsToValidate = 9; // each row group, col group, and grid group will have 9 threads
        int numThreads = 27;
        pthread_t tids[numThreads];
        
        // TO DO: Define an array large enough for each element to contain one of our 9 struct pointers.
        struct arg_struct *args[9];
        // Should we be doing size 9, for each group; or size 27 for each thread?
        for (int x = 0; x < numGroupsToValidate; x++){
            args[x] = malloc(sizeof(*args)); // or malloc(numGroupsToValidate*sizeof(*args))
            memcpy(args[x]->puzzleArg, puzzle, sizeof args[x]->puzzleArg);
            args[x]->puzzleIdx = x;
        }


        for ( int g = 0; g < numGroupsToValidate; g++ ) { 
            //struct arg_struct *args = malloc( sizeof( *args ) );       // Pointer to struct of arguments for passing multiple parameters to row/col/grid functions
            //memcpy( args->puzzleArg, puzzle, sizeof args->puzzleArg ); 
            //args->puzzleIdx = g;
            // TO DO: Add the struct pointer to our array that will be defined right before this loop.
            printf( "...Creating Threads...\n" );
            pthread_create( &tids[g*3], NULL, validateRows, args[g]);
            pthread_create( &tids[g*3+1], NULL, validateCols, args[g]);
            pthread_create( &tids[g*3+2], NULL, validateGrids, args[g]);

        }

        for ( int g = 0; g < numThreads; g++ ) { 
            pthread_join( tids[g], NULL );
            printf( "...Threads Destroyed...\n" );
        }    

        // TO DO: Create a for loop that frees all the struct pointers in the array we create before the pthread_create for loop
        /*
        for(int i=0; i<numGroupsToValidate; i++){
            free(args[i]->puzzleArg);
            free(args[i]->puzzleIdx);
        }  
        free(args);
        */
        for(int i=0; i<sizeof(*args); i++){
            free(args[i]);
        }  
    }
    else{
       printf( "Failed to populate matrix with input file data." );
   }

    return 0;
}




// Takes the size of our 2D array as argument and checks for correct size.
bool validateIncommingArray(int arrSize) {
    return arrSize == 81;
}






void* validateRows(void *infoStruct){
    printf("Thread entered 'validateRows' function.\n");
    struct arg_struct *args = (struct arg_struct *)infoStruct; // Casts a struct we can reference from the param
    //Begin validating
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
            printf("Row %d NOT VALID\n", row);
            // WILL NOT RETURN SO WE CAN CHECK EVERYHWERE IT IS NOT VALID
            //return NULL;
        }
    }
    printf("Row %d VALID!\n", row);
    pthread_exit(NULL); // Should we return v for valid, i for invalid?
}





void* validateCols(void *infoStruct){
    //printf("Thread entered 'validateCols' function.\n");
    //Begin validating
    struct arg_struct *args = (struct arg_struct *)infoStruct; // Casts a struct we can reference from the param
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
            printf("Col %d NOT VALID\n", col);
            // WILL NOT RETURN SO WE CAN CHECK EVERYHWERE IT IS NOT VALID
            //return NULL; 
        }
    }
    printf("Col %d VALID!\n", col);
    pthread_exit(NULL);
}






void* validateGrids(void *infoStruct) {
    printf("Thread entered 'validateGrids' function.\n");
    struct arg_struct *args = (struct arg_struct *)infoStruct; // Casts a struct we can reference from the param
    printf("%d\n", args->puzzleIdx); // Just checking the values are correct for each call. Should be 0-8

    //Begin validating
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
            switch (row) {
                case 0:
                    switch (col) {
                        case 0:
                            printf("The top left subgrid doesn't have the required values.");
                            break;
                        case 3:
                            printf("The top middle subgrid doesn't have the required values.");
                            break;
                        case 6:
                        printf("The top right subgrid doesn't have the required values.");
                    }
                    break;
                case 3:
                    switch (col) {
                        case 0:
                            printf("The middle left subgrid doesn't have the required values.");
                            break;
                        case 3:
                            printf("The middle subgrid doesn't have the required values.");
                            break;
                        case 6:
                            printf("The middle right subgrid doesn't have the required values.");
                    }
                    break;
                case 6:
                    switch (col) {
                        case 0:
                            printf("The bottom left subgrid doesn't have the required values.");
                            break;
                        case 3:
                            printf("The bottom middle subgrid doesn't have the required values.");
                            break;
                        case 6:
                            printf("The bottom right subgrid doesn't have the required values.");

                    }
            
            }
        }
    }
    pthread_exit(NULL); // Should we return v for valid, i for invalid?
}