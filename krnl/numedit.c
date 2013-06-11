#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "numedit.h"

struct NumEdit
{
	TextArea area;
	int row;
	int column;
	int digits;
	char* number;
	int pos;
};

int const NumEditFactor[] = {1, 10, 100, 1000, 10000};

NumEdit CreateNumEdit(TextArea area, int row, int column, int digits)
{
	static size_t const size = sizeof(struct NumEdit);

	NumEdit edit;

	edit = malloc(size);
	assert(edit);
	memset(edit, 0, size);

	edit->area = area;
	edit->row = row;
	edit->column = column;
	edit->digits = digits;

	edit->number = malloc(digits + 1);
	assert(edit->number);
	memset(edit->number, '0', digits);
	edit->number[digits] = 0;

	return edit;
}

void NumEditDrawMark(NumEdit edit)
{
	int column;
	for(column = edit->digits - 1; column >= 0; --column)
	{
		char c;
		c = column == edit->pos ? '^' : ' ';
		SetChar(edit->area, edit->row + 1, column + edit->column, c);
	}
}

void NumEditReDraw(NumEdit edit)
{
	int column;
	for(column = edit->digits - 1; column >= 0; --column)
	{
		char c;
		c = edit->number[column];
		SetChar(edit->area, edit->row, column + edit->column, c);
	}
	NumEditDrawMark(edit);
}

void NumEditDraw(NumEdit edit, int number)
{
	int column;
	for(column = edit->digits - 1; column >= 0; --column)
	{
		int digit;
		digit = number % 10;
		edit->number[column] = '0' + digit;
		number /= 10;
	}
	edit->pos = edit->digits - 1;

	NumEditReDraw(edit);
}

int NumEditGetNumber(NumEdit edit)
{
	int column;
	int number = 0;

	for(column = edit->digits - 1; column >= 0; --column)
	{
		int factor = NumEditFactor[edit->digits - column - 1];
		number += (edit->number[column] - '0') * factor;
	}
	return number;
}

void NumEditUp(NumEdit edit)
{
	char* number = &edit->number[edit->pos];
	(*number)++;
	if(*number > '9') *number = '0';
	NumEditReDraw(edit);
}

void NumEditDown(NumEdit edit)
{
	char* number = &edit->number[edit->pos];
	(*number)--;
	if(*number < '0') *number = '9';
	NumEditReDraw(edit);
}

void NumEditLeft(NumEdit edit)
{
	if(edit->pos > 0) edit->pos--;
	NumEditReDraw(edit);
}

void NumEditRight(NumEdit edit)
{
	if(edit->pos < edit->digits - 1) edit->pos++;
	NumEditReDraw(edit);
}
