#include <stdio.h>
#include <memory.h>
#include <time.h>

/*
  * Constants
  * ****************
  *
  * N - Dimension of the boggle board
  *
  * minimumWordLength - boggle has a minumum work length of 3, any word shorter than 3 is worth 0 points
  *
  * N_sq - save the square of the number since it occurs to often
  *
  * dx and dy - representation of the x and y coordinates of the 3x3 kernel
  *
  * lengthOfKernel - length of the kernel instead of having to find the length of the array, since it's a
  *     constant anyways
  *
  *
  * dictionaryFileName - the full path to the dictionary file
  * boardFileName - the full path to the board file
  * readTypeText - the string required for file reading
  *
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
const int N = 6;
const int minimumWordLength = 3;
const int N_sq = N*N;
const int dx[] = {0,1,1,1,0,-1,-1,-1};
const int dy[] = {1,1,0,-1,-1,-1,0,1};
const int lengthOfKernel = 8;
const char *dictionaryFileName = "";
const char *boardFileName = "";
const char *readTypeText = "r";



/**
 * Variables
 * ***************
 *
 * currentWord - storage for words from the dictionary - size limited to the number of characters on the board + 1
 * board - storage for the boggle board
 * previousString - storage of the previous string for
 * memoizationTable -
 * visited - table of
 *
 */
char currentWord[N_sq+1];
char board[N+1][N+2];
char previousString[N_sq+1];
char memoizationTable[N_sq+1][N][N];
char visited[N][N];
int previousWordLength=0;


char checkBoard(char* word, int curIndex, int row, int column, int wordLen)
{
    char ret = 0;
    if (curIndex == wordLen - 1) {
        return 1;
    }
    for (int m = 0; m < lengthOfKernel; m++){
        int newRow = row + dx[m];
        int newColumn = column + dy[m];
        if (newRow >= 0 && newRow < N &&
            newColumn >= 0 && newColumn < N &&
            !visited[newRow][newColumn] &&
            word[curIndex+1] == board[newRow][newColumn]){

            ++curIndex;
            visited[newRow][newColumn] = 1;
            ret = checkBoard(word, curIndex, newRow, newColumn, wordLen);
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
    FILE *boardFile = fopen(boardFileName, readTypeText);
    FILE *dictionaryFile = fopen(dictionaryFileName, readTypeText);
    int rc, firstMismatch;
    int i = 0,j = 0, k = 0, l = 0;
    for(i = 0; i < N; ++i) {
        fgets(board[i], N + 2, boardFile);
    }
    for(i=0;i<N; i++){
        for(j=0; j<N; j++){
            if(j%N==0){
                printf("\n");
            }
            printf("%c ", board[i][j]);
        }
    }
    printf("\n");

    /*
     * Main loop
     */
    while(fgets(currentWord, N_sq+1, dictionaryFile)) {
        int wordLength = strlen(currentWord);
        char flag = 0;
        /*
         * Remove trailing newline with NULL
         */
        if (currentWord[wordLength-1] == '\n'){
            currentWord[wordLength-1] = NULL;
            --wordLength;
        }

        /*
         * If the word length is less than the minimum word length, skip the word
         */
        if(wordLength<minimumWordLength){
            continue;
        }

        /*
         * Check for the index of the first different character
         */
        for(i = 0; i < wordLength && i < previousWordLength; ++i){
            if(previousString[i] != currentWord[i])
                break;
        }

        firstMismatch = i;

        /*
         * Mismatch is at first character
         * check the rest of the board for the starting character of the word
         */
        if(firstMismatch==0){
            for(i = 0; i < N; ++i){
                for(j = 0; j < N; ++j){
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
            for (i = 0; i < N; ++i) {
                for (j = 0; j < N; ++j) {

                    memoizationTable[k][i][j] = 0;

                    if (board[i][j] != currentWord[k]) {
                        continue;
                    }

                    for (l = 0; l < lengthOfKernel && !memoizationTable[k][i][j]; ++l) {
                        int ti = i + dx[l];
                        int tj = j + dy[l];
                        if (ti < 0 || ti >= N || tj < 0 || tj >= N){
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
         * for each entry in the memoization table at wordlength-1 check if true
         */
        for(i = 0; i < N && !flag; ++i){
            for(j = 0; j < N && !flag; ++j){
                if(memoizationTable[wordLength-1][i][j]){
                    flag=1;
                }
            }
        }

        /*
         * if memoization table was true, then check the word against the board
         */
        if(flag){
            for (i = 0; i < N; ++i){
                for (j = 0; j < N; ++j){
                    if (currentWord[0] != board[i][j]){
                        continue;
                    }
                    memset(visited, 0, sizeof(visited));

                    visited[i][j] = 1;

                    if (checkBoard(currentWord, 0, i, j, wordLength)){
                        printf("%s found!\n", currentWord);
                        break;
                    }
                }
            }
        }

        /*
         * Set previous string
         */
        strcpy(previousString, currentWord);
        previousWordLength = wordLength;
        //printf("%s\n", currentWord);
    }
    fclose( dictionaryFile );
    fclose( boardFile );
    return 0;
}


int main(){
    clock_t begin = clock();
    openBoard();
    clock_t end = clock();
    printf("%f", (double) (end-begin)/CLOCKS_PER_SEC );
    return 0;
}
