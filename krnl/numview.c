#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "numview.h"

struct NumView
{
	TextArea area;
	int row;
	int column;
	int digits;
};

int const NumViewFactor[] = {1, 10, 100, 1000, 10000};

NumView CreateNumView(TextArea area, int row, int column, int digits)
{
	static size_t const size = sizeof(struct NumView);

	NumView view;

	view = malloc(size);
	assert(view);
	memset(view, 0, size);

	view->area = area;
	view->row = row;
	view->column = column;
	view->digits = digits;

	return view;
}

void NumViewDraw(NumView view, int number)
{
	int column;
	for(column = view->digits - 1; column >= 0; --column)
	{
		int digit;
		int c;
		digit = number % 10;
		c = '0' + digit;
		SetChar(view->area, view->row, column + view->column, c);
		number /= 10;
	}
}
