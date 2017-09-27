#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>

/*
  * Constants
  * ****************
  *
  * minimumWordLength - boggle has a minumum work length of 3, any word shorter than 3 is worth 0 points
  *
  * dx and dy - representation of the x and y coordinates of the 3x3 kernel
  *
  * lengthOfKernel - length of the kernel instead of having to find the length of the array, since it's a
  *     constant anyways
  *
  *
  * dictionaryFileName - the full path to the dictionary file
  * boardFileName - the full path to the board file
  *
  *
  *  BOARD EXAMPLE:
  *
  *          ABCDEF
  *          GHIJKL
  *          MNOPQR
  *          STUVWX
  *          YZABCD
  *          EFGHIJ
  *
  *  Dictionary is all in CAPS to board has to be as well.
  */
const int minimumWordLength = 3;
const int dx[] = {0,1,1,1,0,-1,-1,-1};
const int dy[] = {1,1,0,-1,-1,-1,0,1};
const int lengthOfKernel = 8;
const char *dictionaryFileName = "/Users/bstrau202/CLionProjects/untitled1/dict.txt";
const char *boardFileName = "/Users/bstrau202/CLionProjects/untitled2/boggle.txt";
int count=0;


//https://github.com/machineboy2045/fast-boggle-solver/blob/master/boggle_solver.cpp
char* readF( const char fname[] ){
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen ( fname , "r" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

    /* the whole file is now loaded in the memory buffer. */

    // terminate
    fclose (pFile);
    // free (buffer);
    return buffer;
}


/**
 * Variables
 * ***************
 *
 * currentWord - storage for words from the dictionary - size limited to the number of characters on the board + 1
 * board - storage for the boggle board
 * memoizationTable -
 * visited - table of
 *
 */


int previousWordLength=0;

char checkBoard(char* word, int curIndex, int row, int column, int wordLen, int dimension, char visited[dimension][dimension],char board[dimension+1][dimension+2])
{
    char ret = 0;
    if (curIndex == wordLen - 1) {
        return 1;
    }
    for (int m = 0; m < lengthOfKernel; m++){
        int newRow = row + dx[m];
        int newColumn = column + dy[m];
        if (newRow >= 0 && newRow < dimension &&
            newColumn >= 0 && newColumn < dimension &&
            !visited[newRow][newColumn] &&
            word[curIndex+1] == board[newRow][newColumn]){
            ++curIndex;
            visited[newRow][newColumn] = 1;
            ret = checkBoard(word, curIndex, newRow, newColumn, wordLen, dimension, visited, board);
            if (ret) break;
            --curIndex;
            visited[newRow][newColumn] = 0;
        }
    }

    return ret;
}


/*
 * Based off the solution and code proposed by Golam Kawsar
 * http://exceptional-code.blogspot.com/2012/02/solving-boggle-game-recursion-prefix.html
 */
int openBoard() {
    //Initialize variables

    int i = 0, j = 0, k = 0, l = 0;
    int firstMismatch;
    char* boardBuffer = readF(boardFileName);
    printf("size of boardBuffer: %d", (int)sizeof(boardBuffer));
    char* currentLine = strtok(boardBuffer,"\n\t");
    char* previousWord  = "";
    char* lastFoundWord = "";
    int dimension = strlen(currentLine);
    int dimension_squared = dimension*dimension;
    char board[dimension+1][dimension+2];
    char memoizationTable[dimension_squared+1][dimension][dimension];
    char visited[dimension][dimension];


    //Build Board
    while(currentLine!=NULL){
        for(j=0; j<dimension; j++)
            board[i][j]=currentLine[j];
        i++;
        currentLine = strtok (NULL, "\n");
    }
    for(i=0;i<dimension;i++){
        for(j=0; j<dimension; j++){
            if(j%dimension==0){
                printf("\n");
            }
            printf("%c ", board[i][j]);
        }
    }
    printf("\n");
    free(currentLine);
    free(boardBuffer);
    char* dictionaryBuffer = readF(dictionaryFileName);
    char* currentWord = strtok(dictionaryBuffer,"\n\t");

    /*
     * Main loop
     */

    while(currentWord!=NULL) {
        int wordLength = strlen(currentWord);
        char flag = 0;
        /*
         * Remove trailing newline with NULL
         */

        if (currentWord[wordLength-1] == '\n'){
            currentWord[wordLength-1] = '\0';
            --wordLength;
        }

        /*
         * If the word length is less than the minimum word length, skip the word
         */
        if(wordLength<minimumWordLength){
            currentWord = strtok (NULL, "\n\t");
            continue;
        }

        /*
         * Check for the index of the first different character
         */
        for(i = 0; i < wordLength && i < previousWordLength; ++i){
            if(previousWord[i] != currentWord[i])
                break;
        }

        firstMismatch = i;

        /*
         * Mismatch is at first character
         * check the rest of the board for the starting character of the word
         */
        if(firstMismatch==0){
            for(i = 0; i < dimension; ++i){
                for(j = 0; j < dimension; ++j){
                    if(board[i][j] == currentWord[0]) {
                        memoizationTable[0][i][j] = 1;
                    }
                    else {
                        memoizationTable[0][i][j] = 0;
                    }
                }
            }
            firstMismatch = 1;
        }

        /*
         * Check the board from the first mismatched character through the number of
         */
        for(k = firstMismatch; k < wordLength; ++k) {
            for (i = 0; i < dimension; ++i) {
                for (j = 0; j < dimension; ++j) {

                    memoizationTable[k][i][j] = 0;
                    if (board[i][j] != currentWord[k]) {
                        continue;
                    }

                    for (l = 0; l < lengthOfKernel && !memoizationTable[k][i][j]; ++l) {
                        int ti = i + dx[l];
                        int tj = j + dy[l];
                        if (ti < 0 || ti >= dimension || tj < 0 || tj >= dimension){
                            continue;
                        }
                        if (memoizationTable[k - 1][ti][tj] == 1) {
                            memoizationTable[k][i][j] = 1;
                        }
                    }
                }
            }
        }

        /*
         * for each entry in the memoization table at wordLength-1 check if true
         */
        for(i = 0; i < dimension && !flag; ++i){
            for(j = 0; j < dimension && !flag; ++j){
                if(memoizationTable[wordLength-1][i][j]){
                    flag=1;
                }
            }
        }

        /*
         * if memoization table was true, then check the word against the board
         */
        if(flag){
            for (i = 0; i < dimension; ++i){
                for (j = 0; j < dimension; ++j){
                    if (currentWord[0] != board[i][j]){
                        continue;
                    }
                    memset(visited, 0, sizeof(visited));
                    visited[i][j] = 1;

                    if (checkBoard(currentWord, 0, i, j, wordLength, dimension, visited, board)){
                        if(strcmp(lastFoundWord, currentWord)==0){
                            break;
                        }
                        lastFoundWord=currentWord;
                        printf("%s\n", currentWord);
                        count++;

                        break;
                    }
                }
            }
        }

        /*
         * Set previous string
         */
        previousWord = currentWord;
        previousWordLength = wordLength;
        currentWord = strtok (NULL, "\n\t");

    }
    free(dictionaryBuffer);
    return 0;
}


int main(){

    clock_t begin = clock();
    openBoard();
    clock_t end = clock();
    printf("Time took to run: %f\n", (double) (end-begin)/CLOCKS_PER_SEC );
    printf("Found %d words", count);
    return 0;
}
