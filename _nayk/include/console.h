/****************************************************************************
** Copyright (c) 2019 Evgeny Teterin (nayk) <sutcedortal@gmail.com>
** All right reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#ifndef CONSOLE_H
#define CONSOLE_H

#include <QtCore>
#include <QVariant>
#include <QRect>
#include <QSize>

namespace console { //==========================================================

enum Color {
    ColorNone          = -1,
    ColorBlack         = 0,
    ColorRed           = 1,
    ColorGreen         = 2,
    ColorYellow        = 3,
    ColorBlue          = 4,
    ColorMagenta       = 5,
    ColorCyan          = 6,
    ColorWhite         = 7,
    ColorBrightBlack   = 8,
    ColorBrightRed     = 9,
    ColorBrightGreen   = 10,
    ColorBrightYellow  = 11,
    ColorBrightBlue    = 12,
    ColorBrightMagenta = 13,
    ColorBrightCyan    = 14,
    ColorBrightWhite   = 15
};

//==============================================================================
void clear();
void clearUp();
void clearDown();
void setPos(int row, int col);
void setPosUp(int step = 1);
void setPosDown(int step = 1);
void setPosForward(int step = 1);
void setPosBack(int step = 1);
void toNextLine(int col = 1);
void toPrevLine(int col = 1);
void eraseLine();
void eraseLineLeft();
void eraseLineRight();
void savePos();
void restorePos();
void hideCursor();
void showCursor();
void setCursorVisible(bool visible = true);
void write(int row, int col, const QString &text);
void write(const QString &text);
QVariant read(const QVariant &defaultValue = QVariant());
void resetAttributes();
void setColors(Color textColor = ColorWhite, Color bgColor = ColorBlack);
void setTextColor(Color color = ColorWhite);
void setBgColor(Color color = ColorBlack);
void blink(bool on = true);
void frame(int row, int col, int width, int height, bool doubleBorder = false, Color color = ColorNone);
void rectangle(int row, int col, int width, int height, Color color = ColorNone);
void frame(const QRect &rect, bool doubleBorder = false, Color color = ColorNone);
void rectangle(const QRect &rect, Color color = ColorNone);
QSize getSize(const QSize &defaultSize = QSize(80, 25));
int width();
int height();

} // namespace console //=======================================================
#endif // CONSOLE_H
