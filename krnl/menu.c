#include "menu.h"
#include "algorithm.h"
#include "text.h"

#include "../periph/button.h"

#define MENU_BUTTON_TICKS 20

void (*MenuFunc)();

int menuPrevUp;
int menuPrevDown;
int menuPrevLeft;
int menuPrevRight;
int menuPrevOk;

int menuUpClick;
int menuDownClick;
int menuLeftClick;
int menuRightClick;
int menuOkClick;

int menuRootIndex;
int menuSettingsIndex;
int menuStatIndex;
int menuResetIndex;

unsigned menuLines;
unsigned menuSpires;

void MenuWorkFunc();
void MenuRoot();
void MenuSettings();
void MenuLines();
void MenuSpires();
void MenuDelay();
void MenuLineStat();
void MenuReadyStat();
void MenuReset();
void MenuResetStat();

void MenuDrawMarker(int index);
void MenuDrawWork();
void MenuDrawRoot();
void MenuDrawSettings();
void MenuDrawStat();
void MenuDrawLines();
void MenuDrawSpires();
void MenuDrawDelay();
void MenuDrawReadyStat();
void MenuDrawReset();
void MenuDrawResetStat();

#define MENU_LINES_EDIT_ROW 1
#define MENU_LINES_EDIT_COL 6
#define MENU_LINES_EDIT_DIG 3

#define MENU_SPIRES_EDIT_ROW 1
#define MENU_SPIRES_EDIT_COL 6
#define MENU_SPIRES_EDIT_DIG 3

#define MENU_DELAY_EDIT_ROW 1
#define MENU_DELAY_EDIT_COL 6
#define MENU_DELAY_EDIT_DIG 4

#define MENU_LINES_ROW 0
#define MENU_LINES_COL 7
#define MENU_LINES_DIG 3

#define MENU_LINES_PRESET_ROW 0
#define MENU_LINES_PRESET_COL 11
#define MENU_LINES_PRESET_DIG 3

#define MENU_SPIRES_ROW 1
#define MENU_SPIRES_COL 7
#define MENU_SPIRES_DIG 3

#define MENU_SPIRES_PRESET_ROW 1
#define MENU_SPIRES_PRESET_COL 11
#define MENU_SPIRES_PRESET_DIG 3

#define MENU_ROLLS_ROW 1
#define MENU_ROLLS_COL 6
#define MENU_ROLLS_DIG 5

#define MENU_PLUS1_ROW 1
#define MENU_PLUS1_COL 14
#define MENU_PLUS1_TXT "+1"

#define MENU_READY_ROW 2
#define MENU_READY_COL 0
#define MENU_READY_TXT "  йнмеж пскнмю  "

#define MENU_RESET_ROW 3
#define MENU_RESET_COL 0
#define MENU_RESET_TXT "ббнд - яапня    "

void MenuInit()
{
//  menuEdit = CreateNumEdit(menuArea, 1, 6, 3);
//  delayEdit = CreateNumEdit(menuArea, 1, 6, 4);
//  menuLineCounter = CreateNumView(menuArea, 0, 7, 3);
//  menuLinePreset = CreateNumView(menuArea, 0, 11, 3);
//  menuSpireCounter = CreateNumView(menuArea, 1, 7, 3);
//  menuSpirePreset = CreateNumView(menuArea, 1, 11, 3);
//  menuNumView = CreateNumView(menuArea, 1, 6, 5);
//  menuPlus1Label = CreateLabel(menuArea, 1, 14, "+1");
//  menuReadyLabel = CreateLabel(menuArea, 2, 0, "  йнмеж пскнмю  ");
//  menuResetLabel = CreateLabel(menuArea, 3, 0, "ббнд - яапня    ");
  MenuDrawWork();
}

void MenuButtonFilter(int cur, int* prev, int* click)
{

  *click = 0;
  if(cur)
  {
    if(*prev < MENU_BUTTON_TICKS)
    {
      (*prev)++;
      if(*prev == MENU_BUTTON_TICKS)
      {
        *click = 1;
      }
    }
  }
  else
  {
    *prev = 0;
  }
}

void MenuCheckButtons()
{
  MenuButtonFilter(BUTTON_UP,    &menuPrevUp,    &menuUpClick);
  MenuButtonFilter(BUTTON_DOWN,  &menuPrevDown,  &menuDownClick);
  MenuButtonFilter(BUTTON_LEFT,  &menuPrevLeft,  &menuLeftClick);
  MenuButtonFilter(BUTTON_RIGHT, &menuPrevRight, &menuRightClick);
  MenuButtonFilter(BUTTON_OK,    &menuPrevOk,    &menuOkClick);
}

void MenuProc()
{
  MenuCheckButtons();
  MenuFunc();
}

void MenuWorkFunc()
{
  static char prevPlus1;
  static char prevReady;

  if(menuOkClick)
  {
    if(algo_presets.lines_counter >= algo_presets.lines && algo_presets.lines)
    {
      ALGO_RESET;
      algo_run = 1;
      MenuDrawWork();
    }
    else
    {
      MenuDrawRoot();
    }
    return;
  }
  if(menuLines != algo_presets.lines_counter)
  {
    menuLines = algo_presets.lines_counter;
    TextPrintNumber(MENU_LINES_ROW, MENU_LINES_COL, menuLines, MENU_LINES_DIG);
  }
  if(menuSpires != algo_spires_counter)
  {
    menuSpires = algo_spires_counter;
    TextPrintNumber(MENU_SPIRES_EDIT_ROW, MENU_SPIRES_COL, menuSpires, MENU_SPIRES_DIG);
  }

  if(prevPlus1 != algo_plus1)
  {
    prevPlus1 = algo_plus1;
    if(prevPlus1)
    {
      TextPrintString(MENU_PLUS1_ROW, MENU_PLUS1_COL, MENU_PLUS1_TXT);
    }
    else
    {
      TextPrintString(MENU_PLUS1_ROW, MENU_PLUS1_COL, "  ");
    }
  }

  if(algo_presets.lines_counter >= algo_presets.lines)
  {
    if(prevReady == 0)
    {
      TextPrintString(MENU_READY_ROW, MENU_READY_COL, MENU_READY_TXT);
      TextPrintString(MENU_RESET_ROW, MENU_RESET_COL, MENU_RESET_TXT);
      prevReady = 1;
    }
  }
  else prevReady = 0;
}

void MenuRoot()
{
  if(menuUpClick)
  {
    if(menuRootIndex > 0) MenuDrawMarker(--menuRootIndex);
  }
  else if(menuDownClick)
  {
    if(menuRootIndex < 3) MenuDrawMarker(++menuRootIndex);
  }
  else if(menuOkClick)
  {
    switch(menuRootIndex)
    {
    case 0: //reset
      MenuDrawReset();
      break;
    case 1: //settings
      MenuDrawSettings();
      break;
    case 2: //stat
      MenuDrawStat();
      break;
    case 3: //back
      MenuDrawWork();
      break;
    default:
      break;
    }
  }
}

void MenuSettings()
{
  if(menuUpClick)
  {
    if(menuSettingsIndex > 0) MenuDrawMarker(--menuSettingsIndex);
  }
  else if(menuDownClick)
  {
    if(menuSettingsIndex < 3) MenuDrawMarker(++menuSettingsIndex);
  }
  else if(menuOkClick)
  {
    switch(menuSettingsIndex)
    {
    case 0: //lines
      MenuDrawLines();
      break;
    case 1: //spires
      MenuDrawSpires();
      break;
    case 2: //delay
      MenuDrawDelay();
      break;
    case 3: //back
      MenuDrawRoot();
      break;
    default:
      break;
    }
  }
}

void MenuStat()
{
  if(menuUpClick)
  {
    if(menuStatIndex > 2) MenuDrawMarker(--menuStatIndex);
  }
  else if(menuDownClick)
  {
    if(menuStatIndex < 3) MenuDrawMarker(++menuStatIndex);
  }
  else if(menuOkClick)
  {
    switch(menuStatIndex)
    {
    case 2: //reset stat
      MenuDrawResetStat();
      break;
    case 3: //back
      MenuDrawRoot();
      break;
    default:
      break;
    }
  }
}

void MenuLines()
{
  if(menuOkClick)
  {
    algo_presets.lines = TextGetNumber();
    MenuDrawSettings();
  }
  else if(menuUpClick)
  {
    TextOnKey(TEXT_KEY_UP);
  }
  else if(menuDownClick)
  {
    TextOnKey(TEXT_KEY_DOWN);
  }
  else if(menuLeftClick)
  {
    TextOnKey(TEXT_KEY_LEFT);
  }
  else if(menuRightClick)
  {
    TextOnKey(TEXT_KEY_RIGHT);
  }
}

void MenuSpires()
{
  if(menuOkClick)
  {
    algo_presets.spires = TextGetNumber();
    MenuDrawSettings();
  }
  else if(menuUpClick)
  {
    TextOnKey(TEXT_KEY_UP);
  }
  else if(menuDownClick)
  {
    TextOnKey(TEXT_KEY_DOWN);
  }
  else if(menuLeftClick)
  {
    TextOnKey(TEXT_KEY_LEFT);
  }
  else if(menuRightClick)
  {
    TextOnKey(TEXT_KEY_RIGHT);
  }
}

void MenuDelay()
{
  if(menuOkClick)
  {
    algo_presets.delay = TextGetNumber();
    MenuDrawSettings();
  }
  else if(menuUpClick)
  {
    TextOnKey(TEXT_KEY_UP);
  }
  else if(menuDownClick)
  {
    TextOnKey(TEXT_KEY_DOWN);
  }
  else if(menuLeftClick)
  {
    TextOnKey(TEXT_KEY_LEFT);
  }
  else if(menuRightClick)
  {
    TextOnKey(TEXT_KEY_RIGHT);
  }
}

void MenuLineStat()
{
  if(menuOkClick)
  {
    MenuDrawStat();
  }
}

void MenuSpireStat()
{
  if(menuOkClick)
  {
    MenuDrawStat();
  }
}

void MenuReadyStat()
{
  if(menuOkClick)
  {
    MenuDrawStat();
  }
}

void MenuReset()
{
  if(menuUpClick)
  {
    if(menuResetIndex > 1) MenuDrawMarker(--menuResetIndex);
  }
  else if(menuDownClick)
  {
    if(menuResetIndex < 2) MenuDrawMarker(++menuResetIndex);
  }
  else if(menuOkClick)
  {
    switch(menuResetIndex)
    {
    case 1: //yes
      ALGO_RESET;
      /* no break */
    case 2: //no
      MenuDrawRoot();
      break;
    default:
      break;
    }
  }
}

void MenuResetStat()
{
  if(menuUpClick)
  {
    if(menuResetIndex > 2) MenuDrawMarker(--menuResetIndex);
  }
  else if(menuDownClick)
  {
    if(menuResetIndex < 3) MenuDrawMarker(++menuResetIndex);
  }
  else if(menuOkClick)
  {
    switch(menuResetIndex)
    {
    case 2: //yes
      algo_presets.rolls = 0;
      /* no break */
    case 3: //no
      MenuDrawStat();
      break;
    default:
      break;
    }
  }
}

void MenuDrawMarker(int index)
{
  TextPrintChar(0, 0, index == 0 ? '>' : ' ');
  TextPrintChar(1, 0, index == 1 ? '>' : ' ');
  TextPrintChar(2, 0, index == 2 ? '>' : ' ');
  TextPrintChar(3, 0, index == 3 ? '>' : ' ');
}

void MenuDrawRoot()
{
  TextPrintString(0, 0, " яапня\n сярюбйх\n ярюрхярхйю\n мюгюд\n");
  MenuDrawMarker(menuRootIndex);
  MenuFunc = MenuRoot;
}

void MenuDrawWork()
{
  TextPrintString(0, 0, "ярпнй     /\nбхрйнб    /\n\nббнд - лемч\n");
  menuLines = algo_presets.lines_counter;
  menuSpires = algo_spires_counter;
  TextPrintNumber(MENU_LINES_ROW, MENU_LINES_COL, menuLines, MENU_LINES_DIG);
  TextPrintNumber(MENU_LINES_PRESET_ROW, MENU_LINES_PRESET_COL, algo_presets.lines, MENU_LINES_PRESET_DIG);
  TextPrintNumber(MENU_SPIRES_ROW, MENU_SPIRES_COL, menuSpires, MENU_SPIRES_DIG);
  TextPrintNumber(MENU_SPIRES_PRESET_ROW, MENU_SPIRES_PRESET_COL, algo_presets.spires, MENU_SPIRES_PRESET_DIG);
  if(algo_plus1)
  {
    TextPrintString(MENU_PLUS1_ROW, MENU_PLUS1_COL, MENU_PLUS1_TXT);
  }
  MenuFunc = MenuWorkFunc;
}

void MenuDrawSettings()
{
  TextPrintString(0, 0, " ярпнйх\n бхрйх\n гюдепфйю\n мюгюд\n");
  MenuDrawMarker(menuSettingsIndex);
  MenuFunc = MenuSettings;
}

void MenuDrawStat()
{
  TextPrintString(0, 0, " пскнмнб\n\n яапня\n мюгюд\n");
  TextPrintNumber(MENU_ROLLS_ROW, MENU_ROLLS_COL, algo_presets.rolls, MENU_ROLLS_DIG);
  menuStatIndex = 3;
  MenuDrawMarker(menuStatIndex);
  MenuFunc = MenuStat;
}

void MenuDrawLines()
{
  TextPrintString(0, 0, "ярпнйх\n\n\nббнд - цнрнбн\n");
  TextEditNumber(MENU_LINES_EDIT_ROW, MENU_LINES_EDIT_COL, algo_presets.lines, MENU_LINES_EDIT_DIG);
  MenuFunc = MenuLines;
}

void MenuDrawSpires()
{
  TextPrintString(0, 0, "бхрйх\n\n\nббнд - цнрнбн\n");
  TextEditNumber(MENU_SPIRES_EDIT_ROW, MENU_SPIRES_EDIT_COL, algo_presets.spires, MENU_SPIRES_EDIT_DIG);
  MenuFunc = MenuSpires;
}

void MenuDrawDelay()
{
  TextPrintString(0, 0, "гюдепфйю\n\n\nббнд - цнрнбн\n");
  TextEditNumber(MENU_DELAY_EDIT_ROW, MENU_DELAY_EDIT_COL, algo_presets.delay, MENU_DELAY_EDIT_DIG);
  MenuFunc = MenuDelay;
}

void MenuDrawReadyStat()
{
  TextPrintString(0, 0, "пскнмш\n\n\nббнд - мюгюд\n");
  TextPrintNumber(MENU_ROLLS_ROW, MENU_ROLLS_COL, algo_presets.rolls, MENU_ROLLS_DIG);
  MenuFunc = MenuReadyStat;
}

void MenuDrawReset()
{
  TextPrintString(0, 0, " яапня ярпнй?\n дю\n мер\n\n");
  menuResetIndex = 2;
  MenuDrawMarker(menuResetIndex);
  MenuFunc = MenuReset;
}

void MenuDrawResetStat()
{
  TextPrintString(0, 0, " яапня\n ярюрхярхйх?\n дю\n мер\n");
  menuResetIndex = 3;
  MenuDrawMarker(menuResetIndex);
  MenuFunc = MenuResetStat;
}
