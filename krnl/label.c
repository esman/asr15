#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "label.h"

struct Label
{
	TextArea area;
	uint8_t row;
	uint8_t column;
	char const* text;
};

static struct Label labels_array[3];
static int labels_count;

Label CreateLabel(TextArea area, int row, int column, char const* text)
{
	Label label = &labels_array[labels_count++];

	label->area = area;
	label->row = row;
	label->column = column;
	label->text = text;

	return label;
}

void LabelShow(Label const label)
{
	int column = label->column;
	char const* text = label->text;
	while(*text)
	{
		SetChar(label->area, label->row, column, *text);
		++column;
		++text;
	}
}

void LabelHide(Label const label)
{
	int column = label->column;
	char const* text = label->text;
	while(*text)
	{
		SetChar(label->area, label->row, column, ' ');
		++column;
		++text;
	}
}
