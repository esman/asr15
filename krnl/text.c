#include "text.h"
#include "font.h"

#define TEXT_AREA_ROWS    4
#define TEXT_AREA_COLUMNS 16

char text_buff[TEXT_AREA_ROWS * TEXT_AREA_COLUMNS];

unsigned text_row;
unsigned text_col;
unsigned text_pos;
unsigned text_end;

void TextPrintChar(unsigned row, unsigned column, char c)
{
  if (c != text_buff[column + row * TEXT_AREA_COLUMNS])
  {
    text_buff[column + row * TEXT_AREA_COLUMNS] = c;
    RedrawPixMap(row, column, &font[(unsigned char) c * 16]);
  }
}

void TextPrintString(unsigned row, unsigned column, const char* str)
{
  while(*str)
  {
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

void NumEditReDraw(NumEdit edit)
{
  int column;
  for(column = edit->digits - 1; column >= 0; --column)
  {
    char c;
    c = edit->number[column];
    SetChar(edit->area, edit->row, column + edit->column, c);
  }
  TextDrawMark();
}

void TextEditNumber(unsigned row, unsigned column, unsigned number, unsigned digits)
{
  text_row = row;
  text_col = column;
  text_end = digits - 1;
  text_pos = digits;

  TextPrintNumber(row, column, number, digits);
  TextDrawMark();
}

unsigned TextGetNumber(void)
{
  unsigned number = 0;
  char* symbol_addr = &text_buff[text_col + text_row * TEXT_COLUMNS];

  unsigned pos = text_end;
  while(pos >= 0)
  {
    int factor = NumEditFactor[pos];
    char symbol = *symbol_addr;
    number += (symbol - '0') * factor;
    symbol_addr--;
    pos--;
  }
  return number;
}

void TextOnKey(text_key_t key)
{
  char* symbol_addr = &text_buff[text_col + text_pos + text_row * TEXT_COLUMNS];
  char symbol = *symbol_addr;

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
    break;

  case TEXT_KEY_DOWN:
    if(symbol == '0')
    {
      symbol = '9';
    }
    else
    {
      symbol--;
    }
    break;

  case TEXT_KEY_LEFT:
    if(text_pos > 0)
    {
      text_pos--;
    }
    break;

  case TEXT_KEY_RIGHT:
    if(text_pos < text_end)
    {
      text_pos++;
    }
    break;
  }

  *symbol_addr = symbol;
  TextDrawMark();
}
