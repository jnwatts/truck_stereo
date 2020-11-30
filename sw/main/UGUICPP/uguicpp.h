#ifndef UGUICPP_H_
#define UGUICPP_H_

#include <string>

extern "C" {
#include "../UGUI/ugui.h"
}

#include "utils.h"

namespace UGUI {

class Gui
{
public:
    Gui(void (*p)(UG_S16,UG_S16,UG_COLOR), UG_S16 x, UG_S16 y) { UG_Init(&g, p, x, y); }
    void select(void) { UG_SelectGUI(&g); }

    static void fontSelect(const UG_FONT* font) { return UG_FontSelect(font); }
    static void fillScreen(UG_COLOR c) { return UG_FillScreen(c); }
    static void fillFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) { return UG_FillFrame(x1, y1, x2, y2, c); }
    static void fillRoundFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 r, UG_COLOR c) { return UG_FillRoundFrame(x1, y1, x2, y2, r, c); }
    static void drawMesh(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) { return UG_DrawMesh(x1, y1, x2, y2, c); }
    static void drawFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) { return UG_DrawFrame(x1, y1, x2, y2, c); }
    static void drawRoundFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 r, UG_COLOR c) { return UG_DrawRoundFrame(x1, y1, x2, y2, r, c); }
    static void drawPixel(UG_S16 x0, UG_S16 y0, UG_COLOR c) { return UG_DrawPixel(x0, y0, c); }
    static void drawCircle(UG_S16 x0, UG_S16 y0, UG_S16 r, UG_COLOR c) { return UG_DrawCircle(x0, y0, r, c); }
    static void fillCircle(UG_S16 x0, UG_S16 y0, UG_S16 r, UG_COLOR c) { return UG_FillCircle(x0, y0, r, c); }
    static void drawArc(UG_S16 x0, UG_S16 y0, UG_S16 r, UG_U8 s, UG_COLOR c) { return UG_DrawArc(x0, y0, r, s, c); }
    static void drawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) { return UG_DrawLine(x1, y1, x2, y2, c); }
    static void putString(UG_S16 x, UG_S16 y, std::string str) { return UG_PutString(x, y, str.c_str()); }
    static void putText(UG_TEXT* txt) { return UG_PutText(txt); }
    static void putChar(char chr, UG_S16 x, UG_S16 y, UG_COLOR fc, UG_COLOR bc) { return UG_PutChar(chr, x, y, fc, bc); }
    static void consolePutString(std::string str) { return UG_ConsolePutString(str.c_str()); }
    static void consoleSetArea(UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye) { return UG_ConsoleSetArea(xs, ys, xe, ye); }
    static void consoleSetForecolor(UG_COLOR c) { return UG_ConsoleSetForecolor(c); }
    static void consoleSetBackcolor(UG_COLOR c) { return UG_ConsoleSetBackcolor(c); }
    static void setForecolor(UG_COLOR c) { return UG_SetForecolor(c); }
    static void setBackcolor(UG_COLOR c) { return UG_SetBackcolor(c); }
    static UG_S16 getXDim(void) { return UG_GetXDim(); }
    static UG_S16 getYDim(void) { return UG_GetYDim(); }
    static void fontSetHSpace(UG_U16 s) { return UG_FontSetHSpace(s); }
    static void fontSetVSpace(UG_U16 s) { return UG_FontSetVSpace(s); }
    static void waitForUpdate(void) { return UG_WaitForUpdate(); }
    static void update(void) { return UG_Update(); }
    static void drawBMP(UG_S16 xp, UG_S16 yp, UG_BMP* bmp) { return UG_DrawBMP(xp, yp, bmp); }
    static void touchUpdate(UG_S16 xp, UG_S16 yp, UG_U8 state) { return UG_TouchUpdate(xp, yp, state); }

protected:
    UG_GUI g;
};

}

#endif