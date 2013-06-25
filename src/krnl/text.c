#include "text.h"
#include "../periph/display.h"

#define TEXT_CHAR_WIDTH 8
#define TEXT_CHAR_HEIGHT 16

#define TEXT_FONT_SIZE (TEXT_CHAR_HEIGHT * 256)

#define TEXT_AREA_ROWS    4
#define TEXT_AREA_COLUMNS 16

extern unsigned char const font[TEXT_FONT_SIZE];
const unsigned text_edit_factor[] = {1, 10, 100, 1000, 10000};
char text_buff[TEXT_AREA_ROWS * TEXT_AREA_COLUMNS];

unsigned text_row;
unsigned text_col;
unsigned text_pos;
unsigned text_end;

void TextRedrawPixMap(int row, int column, unsigned char const* pixMap)
{
  int offx;
  int offy;
  int x;
  int y;

  offx = column * TEXT_CHAR_WIDTH;
  offy = row * TEXT_CHAR_HEIGHT;
  for(y = 0; y < TEXT_CHAR_HEIGHT; ++y)
  {
    for(x = 0; x < TEXT_CHAR_WIDTH; x++)
    {
      LcdSet(y + offy, TEXT_CHAR_WIDTH - x - 1 + offx, pixMap[y] & (1 << x));
    }
  }
}

void TextPrintChar(unsigned row, unsigned column, char c)
{
  if (c != text_buff[column + row * TEXT_AREA_COLUMNS])
  {
    text_buff[column + row * TEXT_AREA_COLUMNS] = c;
    TextRedrawPixMap(row, column, &font[(unsigned char) c * 16]);
  }
}

void TextPrintString(unsigned row, unsigned column, const char* str)
{
  while(*str)
  {
    if(*str == '\n')
    {
      while(column < TEXT_AREA_COLUMNS)
      {
        TextPrintChar(row, column, ' ');
        column++;
      }
      str++;
      row++;
      column = 0;
      continue;
    }
    TextPrintChar(row, column++, *str++);
  }
}

void TextPrintNumber(unsigned row, unsigned column, unsigned number, unsigned digits)
{
  unsigned pos = digits + column;
  while(pos > column)
  {
    TextPrintChar(row, --pos, '0' + ((char) (number % 10)));
    number /= 10;
  }
}

void TextDrawMark(void)
{
  static unsigned old_pos;
  if(text_pos != old_pos)
  {
    TextPrintChar(text_row + 1, old_pos, ' ');
    old_pos = text_pos;
  }
  TextPrintChar(text_row + 1, text_pos, '^');
}

void TextEditNumber(unsigned row, unsigned column, unsigned number, unsigned digits)
{
  text_row = row;
  text_col = column;
  text_end = text_col + digits - 1;
  text_pos = text_end;

  TextPrintNumber(row, column, number, digits);
  TextDrawMark();
}

unsigned TextGetNumber(void)
{
  unsigned number = 0;
  char* symbol_addr = &text_buff[text_end + text_row * TEXT_COLUMNS];

  unsigned pos = text_end;
  while(1)
  {
    int factor = text_edit_factor[text_end - pos];
    number += (*symbol_addr - '0') * factor;
    if(pos == text_col)
    {
      break;
    }
    symbol_addr--;
    pos--;
  }
  return number;
}

void TextOnKey(text_key_t key)
{
  char symbol = text_buff[text_pos + text_row * TEXT_COLUMNS];

  switch(key)
  {
  case TEXT_KEY_UP:
    if(symbol == '9')
    {
      symbol = '0';
    }
    else
    {
      symbol++;
    }
    goto print_char;

  case TEXT_KEY_DOWN:
    if(symbol == '0')
    {
      symbol = '9';
    }
    else
    {
      symbol--;
    }
    goto print_char;

  case TEXT_KEY_LEFT:
    if(text_pos > text_col)
    {
      text_pos--;
    }
    goto draw_mark;

  case TEXT_KEY_RIGHT:
    if(text_pos < text_end)
    {
      text_pos++;
    }
    goto draw_mark;
  }

  print_char:
  TextPrintChar(text_row, text_pos, symbol);
  goto end;

  draw_mark:
  TextDrawMark();
  end:

  return;
}
