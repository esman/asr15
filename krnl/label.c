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

Label CreateLabel(TextArea area, int row, int column, char const* text)
{
	static size_t const size = sizeof(struct Label);

	Label label;

	label = malloc(size);
	assert(label);
	memset(label, 0, size);

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
