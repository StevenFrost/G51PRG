#include <stdio.h>

// Function signatures
void calculateVolume(double radius);
double volumeForRadius(double radius);
double cube(double singular);
void calculateSmallestMoney(int fullQuantity);
void printNumberOfNotes(int value, int number);
void addTimes(int startTime, int duration);

int main(int argc, char *argv[]) {
	printf("Question 1 : Volume of a sphere\n");
	calculateVolume(1.0);		// 1cm
	calculateVolume(4.657);		// 4.657cm
	calculateVolume(10.0);		// 10cm
	calculateVolume(42.0);		// 42cm

	printf("\n\nQuestion 2 : Smallest money\n");
	calculateSmallestMoney(20);	// £20
	calculateSmallestMoney(79);	// £79
	calculateSmallestMoney(10);	// £10
	calculateSmallestMoney(42);	// £42

	printf("\n\nQuestion 3 : Adding times\n");
	addTimes(1045, 345);		// 10:45 @ 03:45
	addTimes(800, 435);			// 08:00 @ 04:35
	addTimes(2300, 300);		// 23:00 @ 03:00

	return 0;
}

/*
 * Question 1
 */
void calculateVolume(double radius) {
	printf("The volume of a sphere with radius %2.3f is %f\n", radius, volumeForRadius(radius));
}

double volumeForRadius(double radius) {
	const double pi = 3.14159265;
	const double coefficient = (4.0 / 3.0);

	return coefficient * pi * cube(radius);
}

double cube(double d) {
	return (d * d * d);
}

/*
 * Question 2
 */
void calculateSmallestMoney(int fullQuantity) {
	int remainder = 0;			// Remainder after diving by each note/coin value in turn
	int previousRemainder = 0;	// The previous remainder, used for finding the number of notes/coins
	int loopValue = 20;			// The current note value in the loop, starting at £20
	int numNotes = 0;			// Number of notes needed

	// Print the start of the line
	printf("$%d consists of: ", fullQuantity);
	previousRemainder = fullQuantity;

	for (loopValue = 20; loopValue >= 5; loopValue /= 2) {
		if (previousRemainder % loopValue != 0) {
			remainder = (previousRemainder % loopValue);
			numNotes = ((previousRemainder - remainder) / loopValue);
			previousRemainder = remainder;
		} else {
			numNotes = (previousRemainder / loopValue);
			previousRemainder = 0;
		}

		printNumberOfNotes(loopValue, numNotes);
	}

	if (previousRemainder > 1)
		printf("%d $1 coins", previousRemainder);
	else if (previousRemainder == 1)
		printf("%d $1 coin", previousRemainder);

	printf("\n");
}

void printNumberOfNotes(int value, int number) {
	if (number > 1)
		printf("%d $%d notes  ", number, value);
	else if (number == 1)
		printf("%d $%d note  ", number, value);
}

/*
 * Question 3
 */
void addTimes(int startTime, int duration) {
	int startMinutes = (startTime % 100);
	int startHours = ((startTime - startMinutes) / 100);
	int durationMinutes = (duration % 100);
	int durationHours = ((duration - durationMinutes) / 100);
	int finishHours, finishMinutes, finishTime;

	// Add the duration to the start time
	finishHours = startHours + durationHours;
	finishMinutes = startMinutes + durationMinutes;

	if (finishHours > 23) {
		finishHours -= 24;
	}
	if (finishMinutes > 59) {
		finishMinutes -= 60;
		finishHours++;
	}

	// Find the finishing time and print it
	finishTime = ((finishHours * 100) + finishMinutes);
	printf("Start time is %04d. Duration is %04d. End time is %04d\n", startTime, duration, finishTime);
}