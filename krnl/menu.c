#include "menu.h"
#include "algorithm.h"
#include "textarea.h"
#include "numedit.h"
#include "numview.h"
#include "label.h"

#include "../periph/button.h"

#define MENU_BUTTON_TICKS 20

void (*MenuFunc)();

TextArea menuArea;
NumEdit menuEdit;
NumEdit delayEdit;
NumView menuLineCounter;
NumView menuLinePreset;
NumView menuSpireCounter;
NumView menuSpirePreset;
NumView menuNumView;

Label menuPlus1Label;
//Label menuRunLabel;
//Label menuCutLabel;
Label menuReadyLabel;
Label menuResetLabel;

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

int menuLines;
int menuSpires;

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
void MenuDrawLineStat();
void MenuDrawReadyStat();
void MenuDrawReset();
void MenuDrawResetStat();

void MenuInit()
{
	menuArea = CreateTextArea(0, 0, 128, 64);
	menuEdit = CreateNumEdit(menuArea, 1, 6, 3);
	delayEdit = CreateNumEdit(menuArea, 1, 6, 4);
	menuLineCounter = CreateNumView(menuArea, 0, 7, 3);
	menuLinePreset = CreateNumView(menuArea, 0, 11, 3);
	menuSpireCounter = CreateNumView(menuArea, 1, 7, 3);
	menuSpirePreset = CreateNumView(menuArea, 1, 11, 3);
	menuNumView = CreateNumView(menuArea, 1, 6, 5);
	menuPlus1Label = CreateLabel(menuArea, 1, 14, "+1");
	menuReadyLabel = CreateLabel(menuArea, 2, 0, "  ÊÎÍÅÖ ĞÓËÎÍÀ  ");
//	menuRunLabel = CreateLabel(menuArea, 2, 0,   "     ÍÀÂÈÂÊÀ    ");
//	menuCutLabel = CreateLabel(menuArea, 2, 0,   "      ÎÒĞÅÇ     ");
	menuResetLabel = CreateLabel(menuArea, 3, 0, "ÂÂÎÄ - ÑÁĞÎÑ    ");
	MenuDrawWork();
//	menuArea = CreateTextArea(0, 0, 128, 64);
//	menuEdit = CreateNumEdit(menuArea, 1, 6, 3);
//	delayEdit = CreateNumEdit(menuArea, 1, 6, 4);
//	menuLineCounter = CreateNumView(menuArea, 0, 9, 3);
//	menuLinePreset = CreateNumView(menuArea, 0, 13, 3);
//	menuSpireCounter = CreateNumView(menuArea, 1, 9, 3);
//	menuSpirePreset = CreateNumView(menuArea, 1, 13, 3);
//	menuNumView = CreateNumView(menuArea, 1, 6, 5);
//	menuPlus1Label = CreateLabel(menuArea, 2, 0, "+1");
//	menuReadyLabel = CreateLabel(menuArea, 2, 3, "ÊÎÍÅÖ ĞÓËÎÍÀ");
//	MenuDrawWork();
}

void MenuButtonFilter(int cur, int* prev, int* click)
{
	// *click = 0;
	// if(cur)
	// {
		// if(*prev == 0)
		// {
			// *click = 1;
		// }
		// *prev = MENU_BUTTON_TICKS;
	// }
	// else if(*prev) (*prev)--;

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
	else if(*prev) (*prev)--;
}

void MenuCheckButtons()
{
	MenuButtonFilter(ButtonUp(), &menuPrevUp, &menuUpClick);
	MenuButtonFilter(ButtonDown(), &menuPrevDown, &menuDownClick);
	MenuButtonFilter(ButtonLeft(), &menuPrevLeft, &menuLeftClick);
	MenuButtonFilter(ButtonRight(), &menuPrevRight, &menuRightClick);
	MenuButtonFilter(ButtonOk(), &menuPrevOk, &menuOkClick);
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
//	static char prevCut;
//	static char prevRun;

	if(menuOkClick)
	{
		if(GetLineCount() >= GetLinePreset() && GetLinePreset())
		{
			Reset();
			SetRun(1);
			MenuDrawWork();
		}
		else
		{
			MenuDrawRoot();
		}
		return;
	}
	if(menuLines != GetLineCount())
	{
		menuLines = GetLineCount();
		NumViewDraw(menuLineCounter, menuLines);
	}
	if(menuSpires != GetSpireCount())
	{
		menuSpires = GetSpireCount();
		NumViewDraw(menuSpireCounter, menuSpires);
	}

	if(prevPlus1 != GetPlus1())
	{
		prevPlus1 = GetPlus1();
		if(prevPlus1)
		{
			LabelShow(menuPlus1Label);
		}
		else
		{
			LabelHide(menuPlus1Label);
		}
	}

//	if(prevRun != Run())
//	{
//		prevRun = Run();
//		if(prevRun)
//		{
//			LabelShow(menuRunLabel);
//		}
//		else
//		{
//			LabelHide(menuRunLabel);
//		}
//	}

//	if(prevCut != Cut())
//	{
//		prevCut = Cut();
//		if(prevCut)
//		{
//			LabelShow(menuCutLabel);
//		}
//		else
//		{
//			LabelHide(menuCutLabel);
//		}
//	}

	if(GetLineCount() >= GetLinePreset())
	{
		if(prevReady == 0)
		{
			LabelShow(menuReadyLabel);
			LabelShow(menuResetLabel);
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
		SetLines(NumEditGetNumber(menuEdit));
		MenuDrawSettings();
	}
	else if(menuUpClick) NumEditUp(menuEdit);
	else if(menuDownClick) NumEditDown(menuEdit);
	else if(menuLeftClick) NumEditLeft(menuEdit);
	else if(menuRightClick) NumEditRight(menuEdit);
}

void MenuSpires()
{
	if(menuOkClick)
	{
		SetSpires(NumEditGetNumber(menuEdit));
		MenuDrawSettings();
	}
	else if(menuUpClick) NumEditUp(menuEdit);
	else if(menuDownClick) NumEditDown(menuEdit);
	else if(menuLeftClick) NumEditLeft(menuEdit);
	else if(menuRightClick) NumEditRight(menuEdit);
}

void MenuDelay()
{
	if(menuOkClick)
	{
		SetDelay(NumEditGetNumber(delayEdit));
		MenuDrawSettings();
	}
	else if(menuUpClick) NumEditUp(delayEdit);
	else if(menuDownClick) NumEditDown(delayEdit);
	else if(menuLeftClick) NumEditLeft(delayEdit);
	else if(menuRightClick) NumEditRight(delayEdit);
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
			Reset();
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
			ResetStat();
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
	SetChar(menuArea, 0, 0, index == 0 ? '>' : ' ');
	SetChar(menuArea, 1, 0, index == 1 ? '>' : ' ');
	SetChar(menuArea, 2, 0, index == 2 ? '>' : ' ');
	SetChar(menuArea, 3, 0, index == 3 ? '>' : ' ');
}

void MenuDrawRoot()
{
	SetText(menuArea, " ÑÁĞÎÑ\n ÓÑÒÀÂÊÈ\n ÑÒÀÒÈÑÒÈÊÀ\n ÍÀÇÀÄ");
	MenuDrawMarker(menuRootIndex);
	MenuFunc = MenuRoot;
}

void MenuDrawWork()
{
	SetText(menuArea, "ÑÒĞÎÊ     /\nÂÈÒÊÎÂ    /\n\nÂÂÎÄ - ÌÅÍŞ");
	menuLines = GetLineCount();
	menuSpires = GetSpireCount();
	NumViewDraw(menuLineCounter, menuLines);
	NumViewDraw(menuLinePreset, GetLinePreset());
	NumViewDraw(menuSpireCounter, menuSpires);
	NumViewDraw(menuSpirePreset, GetSpirePreset());
	if(GetPlus1()) LabelShow(menuPlus1Label);
	MenuFunc = MenuWorkFunc;
}

void MenuDrawSettings()
{
	SetText(menuArea, " ÑÒĞÎÊÈ\n ÂÈÒÊÈ\n ÇÀÄÅĞÆÊÀ\n ÍÀÇÀÄ");
	MenuDrawMarker(menuSettingsIndex);
	MenuFunc = MenuSettings;
}

void MenuDrawStat()
{
	SetText(menuArea, " ĞÓËÎÍÎÂ\n\n ÑÁĞÎÑ\n ÍÀÇÀÄ");
	NumViewDraw(menuNumView, GetReadyTotal());
	menuStatIndex = 3;
	MenuDrawMarker(menuStatIndex);
	MenuFunc = MenuStat;
}

void MenuDrawLines()
{
	SetText(menuArea, "ÑÒĞÎÊÈ\n\n\nÂÂÎÄ - ÃÎÒÎÂÎ");
	NumEditDraw(menuEdit, GetLinePreset());
	MenuFunc = MenuLines;
}

void MenuDrawSpires()
{
	SetText(menuArea, "ÂÈÒÊÈ\n\n\nÂÂÎÄ - ÃÎÒÎÂÎ");
	NumEditDraw(menuEdit, GetSpirePreset());
	MenuFunc = MenuSpires;
}

void MenuDrawDelay()
{
	SetText(menuArea, "ÇÀÄÅĞÆÊÀ\n\n\nÂÂÎÄ - ÃÎÒÎÂÎ");
	NumEditDraw(delayEdit, GetDelay());
	MenuFunc = MenuDelay;
}

void MenuDrawLineStat()
{
	SetText(menuArea, "ÑÒĞÎÊÈ\n\n\nÂÂÎÄ - ÍÀÇÀÄ");
	NumViewDraw(menuNumView, GetLineTotal());
	MenuFunc = MenuLineStat;
}

void MenuDrawReadyStat()
{
	SetText(menuArea, "ĞÓËÎÍÛ\n\n\nÂÂÎÄ - ÍÀÇÀÄ");
	NumViewDraw(menuNumView, GetReadyTotal());
	MenuFunc = MenuReadyStat;
}

void MenuDrawReset()
{
	SetText(menuArea, " ÑÁĞÎÑ ÑÒĞÎÊ?\n ÄÀ\n ÍÅÒ");
	menuResetIndex = 2;
	MenuDrawMarker(menuResetIndex);
	MenuFunc = MenuReset;
}

void MenuDrawResetStat()
{
	SetText(menuArea, " ÑÁĞÎÑ\n ÑÒÀÒÈÑÒÈÊÈ?\n ÄÀ\n ÍÅÒ");
	menuResetIndex = 3;
	MenuDrawMarker(menuResetIndex);
	MenuFunc = MenuResetStat;
}
