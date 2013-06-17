#ifndef TEXTAREA_H
#define TEXTAREA_H

#ifdef __cplusplus
extern "C"
{
#endif

#define TEXT_AREA_ROWS    4
#define TEXT_AREA_COLUMNS 16

typedef struct
{
  int x;
  int y;
  char text[TEXT_AREA_ROWS * TEXT_AREA_COLUMNS + 1];
} TextArea;

void SetText(TextArea* area, char const* text);
void SetChar(TextArea* area, int row, int column, char c);

#ifdef __cplusplus
} /* extern C */
#endif

#endif // TEXTAREA_H
