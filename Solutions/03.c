#include <stdio.h>

#define THRESHOLD 50
#define ARRAY_WIDTH 10
#define ARRAY_LENGTH 6
#define PAIR_1 0
#define PAIR_2 1
#define LARGEST_SUM 2
#define LINE_NUMBER 3

void findLineLargestPairSum(FILE *input, int line[], int info[]);

int main(int argc, char *argv[]) {
	FILE *input;							// Pointer to a FILE structure
	int line[ARRAY_LENGTH][ARRAY_WIDTH];	// Array to store a maximum of 6 lines containing 10 numbers
	int info[6][4] = {0, 0, 0, 0};			// { Pair #1, Pair #2, Largest Sum, Line number }
	int i, j, k = 0;

	input = fopen(argv[1], "r");			// Open the user-specified file for reading

	for (i = 0; i < 6; i++) {
		for (j = 0; j < 10; j++)
			fscanf(input, "%d ,", &line[i][j]);

		info[i][3] = i;
		findLineLargestPairSum(input, line[i], info[i]);

		if (k < info[i][LARGEST_SUM])
			k = info[i][LARGEST_SUM];
	}

	for (i = 0; i < 6; i++) {
		if (k == info[i][LARGEST_SUM])
			printf("Line %d: largest pair is %d and %d, with a total of %d\n", info[i][LINE_NUMBER], info[i][PAIR_1], info[i][PAIR_2], info[i][LARGEST_SUM]);
	}

	fclose(input);
	return 0;
}

void findLineLargestPairSum(FILE *input, int line[], int info[]) {
	int i, j, pairSum;

	info[LARGEST_SUM] = 0;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			if (i != j) {
				pairSum = (line[i] + line[j]);

				if (pairSum <= THRESHOLD && pairSum > info[LARGEST_SUM]) {
					info[LARGEST_SUM] = pairSum;
					info[PAIR_1] = line[i];
					info[PAIR_2] = line[j];
				}
			}
		}
	}

	info[LINE_NUMBER]++;
}