#ifndef LABEL_H
#define LABEL_H

#include "textarea.h"

struct Label;
typedef struct Label* Label;

Label CreateLabel(TextArea area, int row, int column, char const* text);
void LabelShow(Label const label);
void LabelHide(Label const label);

#endif // LABEL_H
