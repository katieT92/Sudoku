#include <stdio.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h> //used for threads
#include <string.h> //Removes error given when trying to use 'memcpy' around line 85

// Needed for use of bool data type.
// There is a note in the google doc to ask if this counts as a prohibited "special dependency"
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
    /* int puzzleSize = sizeof(puzzle) / sizeof(puzzle[0]); = 9 */
    int puzzleSize = (sizeof(puzzle) / sizeof(*puzzle)) * (sizeof(puzzle[0]) / sizeof(*puzzle[0])); // = 81

    // Read input into the puzzle matrix
    for (int r = 0; r < puzzleRows; r++) {
        for (int c = 0; c < puzzleCols; c++) {
        scanf("%d",&puzzle[r][c]);
        }
    }

    // Validate successs of input into puzzle matrix.
    // Continue with thread declarations and creation.
    //If else statement commented out in order to test thread creation/deletion
   if (validateIncommingArray(puzzleSize)){
        
        // Preparing struct of arguments for passing multiple parameters to our row/col/grid validating functions
        struct arg_struct {
            int puzzleIdx;
            int puzzleArg[9][9];
        };
        struct arg_struct args;
        memcpy(args.puzzleArg, puzzle, sizeof args.puzzleArg);

        int numGroupsToValidate = 9;
        int numThreads = 27;
        pthread_t tids[numThreads];

        for (int g = 0; g < numGroupsToValidate; g++) {
            args.puzzleIdx = g;
            // Note that range tids[0-8] will be row threads, tids[9-17] = col threads, tids[18-26] = grid threads
            // the argument list is being passed as a single structure "args" as per advice on stackoverflow
            printf("...Creating Threads...\n");
            pthread_create(&tids[g], NULL, validateRows, (void *)&args);
            pthread_create(&tids[g+9], NULL, validateCols, (void *)&args);
            pthread_create(&tids[g+9+9], NULL, validateGrids, (void *)&args);
        }

        for (int g = 0; g < numGroupsToValidate; g++) { 
            pthread_join(tids[g], NULL);
            printf("...Threads Destroyed...\n");
        }
    }
    else{
       printf("Failed to populate matrix with input file data.");
   }

    return 0;
}



// Takes the size of our 2D array we created as an argument and checks that it is the right size.
// This function was simplified to avoid errors with passing a 2d array as argument
bool validateIncommingArray(int arrSize) {
    if (arrSize == 81) { return true; } // returns true if the array elements is 81, returns false (Invalid input) if not 81 elements. 
    return false;
}


void* validateRows(void *infoStruct){
    printf("Thread entered 'validateRows' function.\n");
    return NULL;
}

void* validateCols(void *infoStruct){
    printf("Thread entered 'validateCols' function.\n");
    return NULL;
}

// I commented this function out for now to test the above function.
void* validateGrids(void *infoStruct) {
    printf("Thread entered 'validateGrids' function.\n");
    return NULL;

    /* Since we will be passing in parameters 0-8, when 
        When 0 is passed in, we will start checking element at [0][0] <-- the top left element of the top left square
        When 1 is passed in, we will start checking element at [0][3] <-- the top left element of the top middle square
        When 2 is passed in, we will start checking element at [0][6] <-- the top left element of the top right square
        
        When 3 is passed in, we will start checking element at [3][0] <-- the top left element of the middle left square
        When 4 is passed in, we will start checking element at [3][3] <-- the top left element of the middle middle square
        When 5 is passed in, we will start checking element at [3][6] <-- the top left element of the middle right square

        When 6 is passed in, we will start checking element at [6][0] <-- the top left element of the middle left square
        When 7 is passed in, we will start checking element at [6][3] <-- the top left element of the middle middle square
        When 8 is passed in, we will start checking element at [6][6] <-- the top left element of the middle right square

        Therefore, the parameter, lets call it, i, can be manipulated as 
        0 = [0][0]
        1 = [0][3]
        2 = [0][6]

        3 = [3][0] 
        4 = [3][3]
        5 = [3][6]

        6 = [6][0]
        7 = [6][3]
        8 = [6][6]

        i / 3 * 3 will give us the row we should start checking at
        i % 3 * 3 will give us the col we should start checking at 

        so we can have variables in this function like
        
        int row = i / 3 * 3;
        int col = i % 3 * 3;
        int maxRow = row + 3;
        int maxCol = col + 3;

        int neededValues[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

        while (row < maxRow){
            while (col < maxCol){
                do our checking of inputArray[row][col] against needeValues
                col++;
            }
            row++;
        }

        -Katie 
    */
/*
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
    */
}

//This is a test for github
