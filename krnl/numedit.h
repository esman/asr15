#ifndef NUMEDIT_H
#define NUMEDIT_H

#include "textarea.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NumEdit;
typedef struct NumEdit* NumEdit;

NumEdit CreateNumEdit(TextArea area, int row, int column, int digits);
void NumEditDraw(NumEdit edit, int number);
int NumEditGetNumber(NumEdit edit);

void NumEditUp(NumEdit edit);
void NumEditDown(NumEdit edit);
void NumEditLeft(NumEdit edit);
void NumEditRight(NumEdit edit);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NUMEDIT_H
