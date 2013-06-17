#ifndef NUMVIEW_H
#define NUMVIEW_H

#include "textarea.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  TextArea* area;
  int row;
  int column;
  int digits;
} NumView;

NumView CreateNumView(TextArea area, int row, int column, int digits);
void NumViewDraw(NumView view, int number);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NUMVIEW_H
