#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "textarea.h"
#include "font.h"

#include "../periph/display.h"

struct TextArea
{
	int x;
	int y;
	int rows;
	int columns;
	char text[16 * 4 + 1];
};

static struct TextArea textarea_array[1];

TextArea CreateTextArea(int x, int y, int width, int height)
{
	TextArea area = textarea_array;

	assert(x >= 0 && y >= 0 && width >= 0 && height >= 0);

	area->x = x;
	area->y = y;
	area->rows = height / CHAR_HEIGHT;
	area->columns = width / CHAR_WIDTH;

	return area;
}

void RedrawPixMap(TextArea area, int row, int column, unsigned char const* pixMap)
{
	int offx;
	int offy;
	int x;
	int y;

	offx = area->x + column * CHAR_WIDTH;
	offy = area->y + row * CHAR_HEIGHT;
	for(y = 0; y < CHAR_HEIGHT; ++y)
	{
		for(x = 0; x < CHAR_WIDTH; x++)
		{
			LcdSet(y + offy, CHAR_WIDTH - x - 1 + offx, pixMap[y] & (1 << x));
		}
	}
//	LcdRedraw();
}

void SetChar(TextArea area, int row, int column, char c)
{
	char* old = &area->text[column + row * area->columns];
	if (*old != c)
	{
		*old = c;
		RedrawPixMap(area, row, column, &font[(unsigned char) c * 16]);
	}
}

void ResetTextArea(TextArea area, char c)
{
	int row;
	int column;
	for(row = 0; row < area->rows; ++row)
	{
		for(column = 0; column < area->columns; ++column)
		{
			SetChar(area, row, column, c);
		}
	}
}

void SetText(TextArea area, char const* text)
{
	int row = 0;
	int column = 0;

	assert(text);

	ResetTextArea(area, ' ');

	for(;*text && row < area->rows; ++text)
	{
		if(*text == '\n')
		{
			++row;
			column = 0;
			continue;
		}

		SetChar(area, row, column, *text);

		if(++column == area->columns)
		{
			++row;
			column = 0;
		}
	}
}
