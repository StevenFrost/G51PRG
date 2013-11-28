#include <stdio.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 25
#define NORMAL 1
#define ERROR 0

void InitialiseArrayToZero(int a[], int size) {
	int i;
	for (i = 0; i < size; i++)
		a[i] = 0;
}

int main(int argc, char *argv[]) {
	FILE *input;
	int wordLength, maxLength, character, i, state = NORMAL;
	int lengthCount[MAX_WORD_LENGTH + 1];

	InitialiseArrayToZero(lengthCount, MAX_WORD_LENGTH);	// Initialise all array elements
	fopen_s(&input, argv[1], "r");							// Open the file for reading
	maxLength = wordLength = 0;								// Initialise found word lengths

	while ((character = fgetc(input)) != EOF) {
		if (isalpha(character) || character == '-') {
			if (wordLength <= MAX_WORD_LENGTH) {
				wordLength++;						// Only increment wordLength within the bounds of the array
			} else {
				state = ERROR; break;				// Word > 25 characters, break out of the loop to display error
			}
		} else {
			lengthCount[wordLength]++;				// Increment the length count for the previously found word length
			if (wordLength > maxLength)
				maxLength = wordLength;				// If the found length is greater than the previous max, length = max
			wordLength = 0;
		}
	}

	fclose(input);

	switch (state) {
	case NORMAL:
		for (i = 1; i <= maxLength; i++)
			printf("Length %d :\t%d Occurrences\n", i, lengthCount[i]);
		return 0;
	case ERROR:
		fprintf(stderr, "Word > 25 characters encountered.\n");
		return 1;
	default:
		fprintf(stderr, "State error.\n");
		return 2;
	}
}