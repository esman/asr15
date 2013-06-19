#ifndef TEXT_H_
#define TEXT_H_

#define TEXT_ROWS    4
#define TEXT_COLUMNS 16

typedef enum
{
  TEXT_KEY_UP,
  TEXT_KEY_DOWN,
  TEXT_KEY_LEFT,
  TEXT_KEY_RIGHT
} text_key_t;

void TextPrintChar(unsigned row, unsigned column, char c);
void TextPrintString(unsigned row, unsigned column, const char* str);
void TextPrintNumber(unsigned row, unsigned column, unsigned number, unsigned digits);
void TextEditNumber(unsigned row, unsigned column, unsigned number, unsigned digits);
unsigned TextGetNumber(void);
void TextOnKey(text_key_t key);

#endif /* TEXT_H_ */
