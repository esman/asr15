#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "textarea.h"
#include "font.h"

#include "../periph/display.h"

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
}

void SetChar(TextArea area, int row, int column, char c)
{
  char* old = &area->text[column + row * TEXT_AREA_COLUMNS];
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
  for(row = 0; row < TEXT_AREA_ROWS; ++row)
  {
    for(column = 0; column < TEXT_AREA_COLUMNS; ++column)
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

  for(;*text && row < TEXT_AREA_ROWS; ++text)
  {
    if(*text == '\n')
    {
      ++row;
      column = 0;
      continue;
    }

    SetChar(area, row, column, *text);

    if(++column == TEXT_AREA_COLUMNS)
    {
      ++row;
      column = 0;
    }
  }
}
