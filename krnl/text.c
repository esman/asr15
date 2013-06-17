#include "text.h"
#include "font.h"

#define TEXT_AREA_ROWS    4
#define TEXT_AREA_COLUMNS 16

char text_buff[TEXT_AREA_ROWS * TEXT_AREA_COLUMNS + 1];

unsigned text_row;
unsigned text_col;
unsigned text_pos;
unsigned text_dig;

void TextPrintChar(unsigned row, unsigned column, char c)
{
  if (c != text_buff[column + row * TEXT_AREA_COLUMNS])
  {
    text_buff[column + row * TEXT_AREA_COLUMNS] = c;
    RedrawPixMap(row, column, &font[(unsigned char) c * 16]);
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

void TextEditNumber(unsigned row, unsigned column, unsigned number, unsigned digits)
{
  text_row = row;
  text_col = column;
  text_dig = digits;
  text_pos = digits - 1;

  TextPrintNumber(row, column, number, digits);

  NumEditReDraw(edit);
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
