#include <stdio.h>

#define FINDING_ITEM 0		// Finding <item>
#define FINDING_TITLE 1		// In <item>, finding <title>
#define PRINTING_TITLE 2	// In <title>, printing text

int isItemTag(int *tag);
int isTitleTag(int *tag);

int main(int argc, char *argv[]) {
	int character;				// Current character being read from the standard input
	int tag[3];					// Array containing the first three characters of a tag
	int state = FINDING_ITEM;	// Program state

	while ((character = getchar()) != EOF) {
		if (character == '<') {
			// Get the first three tag characters
			tag[0] = getchar();
			tag[1] = getchar();
			tag[2] = getchar();

			if ((state == FINDING_ITEM) && (isItemTag(tag)))
				state = FINDING_TITLE;
			else if ((state == FINDING_TITLE) && (isTitleTag(tag)))
				state = PRINTING_TITLE;
		} else if (character == '>' && state == PRINTING_TITLE) {
			while ((character = getchar()) != '<')
				putchar(character);

			printf("\n");
			state = FINDING_ITEM;
		}
	}

	return 0;
}

int isItemTag(int *tag) {
	if (tag[0] == 'i' && tag[1] == 't' && tag[2] == 'e')
		return 1;
	else
		return 0;
}

int isTitleTag(int *tag) {
	if (tag[0] == 't' && tag[1] == 'i' && tag[2] == 't')
		return 1;
	else
		return 0;
}