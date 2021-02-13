// C recap exercises

#include <stdio.h>
#include <stdlib.h>


// συμπληρώστε τα παρακάτω
struct point_2d {
	float x;
	float y;
};
typedef struct point_2d* Point2d;


// διορθώστε τη swap
void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}
	
//όπως και προηγουμένως, τροποποιήστε το .vscode/settings.json για να κάνετε compile το recap
void test_swap() {
	int a = 1;
	int b = 2;

	swap(&a, &b);		// η γραμμή αυτή θέλει αλλαγή

	if(a < b) {
		printf("Not swapped!\n");
	}
}

void test_struct() {
	// Αλλάξτε το struct και typedef στην αρχή του αρχείου, ΟΧΙ τα παρακάτω
	Point2d point = malloc(sizeof(*point));
	point->x = 1.2;
	point->y = 1.2;

	printf("point with coordinates: (%f, %f)\n", point->x, point->y);

	free(point);
}

int main() {
	test_swap();
	test_struct();
}