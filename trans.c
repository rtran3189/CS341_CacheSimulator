/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i;
    int j;
    int row;
    int column;
    int diagonal = 0;
    int temp = 0;

    // 32 x 32
    if (N == 32)
    {
        // Loops across the blocks
        for (column = 0; column < N; column = column + 8)
        {
            for (row = 0; row < N; row = row + 8)
            {
                // Loops through each block
                for (i = row; i < row + 8; i++) 
                {
                    for (j = column; j < column + 8; j++)
                    {
                        // When the row index and column index are not equal, change
                        // the value in array B to the one we want in A.
                        if (i != j) 
                        {
                            B[j][i] = A[i][j];
                        }
                        // When row index equals column index,  Both arrays A and B are accessed
                        // and have different tags. To avoid misses, used 2 temporary variables,
                        // to store the position, and the other to store the value for below.
                        else
                        {
                            temp = A[i][j];
                            diagonal = i;
                        }
                    }
                    if (row == column)
                    {
                        B[diagonal][diagonal] = temp;
                    }
                }
            }
        }
    }
    // 64 x 64
    else if (N == 64)
    {
        int tempVal_0, tempVal_1, tempVal_2, tempVal_3, tempVal_4, tempVal_5, tempVal_6, tempVal_7;

        // Loop through column and row blocks 
        for (column = 0; column < 64; column = column + 8)
        {
            for(row = 0; row<64; row = row + 8)
            {
                // Loop through columns 0-3
                for(i = 0; i < 4; i++)
                {
                    tempVal_0 = A[column+i][row+0];
                    tempVal_1 = A[column+i][row+1];
                    tempVal_2 = A[column+i][row+2];
                    tempVal_3 = A[column+i][row+3];
                    tempVal_4 = A[column+i][row+4];
                    tempVal_5 = A[column+i][row+5];
                    tempVal_6 = A[column+i][row+6];
                    tempVal_7 = A[column+i][row+7];

                    B[row+0][column+i+0] = tempVal_0;
                    B[row+0][column+i+4] = tempVal_5;
                    B[row+1][column+i+0] = tempVal_1;
                    B[row+1][column+i+4] = tempVal_6;
                    B[row+2][column+i+0] = tempVal_2;
                    B[row+2][column+i+4] = tempVal_7;
                    B[row+3][column+i+0] = tempVal_3;
                    B[row+3][column+i+4] = tempVal_4;
                }

                tempVal_0 = A[column+4][row+4];
                tempVal_1 = A[column+5][row+4];
                tempVal_2 = A[column+6][row+4];
                tempVal_3 = A[column+7][row+4];
                tempVal_4 = A[column+4][row+3];
                tempVal_5 = A[column+5][row+3];
                tempVal_6 = A[column+6][row+3];
                tempVal_7 = A[column+7][row+3];


                B[row+4][column+0] = B[row+3][column+4];
                B[row+4][column+4] = tempVal_0;
                B[row+3][column+4] = tempVal_4;
                B[row+4][column+1] = B[row+3][column+5];
                B[row+4][column+5] = tempVal_1;
                B[row+3][column+5] = tempVal_5;
                B[row+4][column+2] = B[row+3][column+6];
                B[row+4][column+6] = tempVal_2;
                B[row+3][column+6] = tempVal_6;
                B[row+4][column+3] = B[row+3][column+7];
                B[row+4][column+7] = tempVal_3;
                B[row+3][column+7] = tempVal_7;

                for(i = 0; i < 3; i++)
                {
                    tempVal_0 = A[column+4][row+5+i];
                    tempVal_1 = A[column+5][row+5+i];
                    tempVal_2 = A[column+6][row+5+i];
                    tempVal_3 = A[column+7][row+5+i];
                    tempVal_4 = A[column+4][row+i];
                    tempVal_5 = A[column+5][row+i];
                    tempVal_6 = A[column+6][row+i];
                    tempVal_7 = A[column+7][row+i];


                    B[row+5+i][column+0] = B[row+i][column+4];
                    B[row+5+i][column+4] = tempVal_0;
                    B[row+i][column+4] = tempVal_4;
                    B[row+5+i][column+1] = B[row+i][column+5];
                    B[row+5+i][column+5] = tempVal_1;
                    B[row+i][column+5] = tempVal_5;
                    B[row+5+i][column+2] = B[row+i][column+6];
                    B[row+5+i][column+6] = tempVal_2;
                    B[row+i][column+6] = tempVal_6;
                    B[row+5+i][column+3] = B[row+i][column+7];
                    B[row+5+i][column+7] = tempVal_3;
                    B[row+i][column+7] = tempVal_7;
                }
            }
        }
    }
    // Other sizes (61 x 67)
    else
    {

    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

