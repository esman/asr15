#ifndef TEXTAREA_H
#define TEXTAREA_H

#ifdef __cplusplus
extern "C"
{
#endif

struct TextArea;
typedef struct TextArea* TextArea;

TextArea CreateTextArea(int x, int y, int width, int height);

void SetText(TextArea area, char const* text);
void SetChar(TextArea area, int row, int column, char c);

#ifdef __cplusplus
} /* extern C */
#endif

#endif // TEXTAREA_H
