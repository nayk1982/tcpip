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
#include <QTextStream>
#include <QTextCodec>

#include "console.h"
#include "convert.h"

#ifdef Q_OS_WIN32
#   include <windows.h>
#else
#   include <sys/ioctl.h>
#   include <stdio.h>
#   include <unistd.h>
#endif

namespace console { //==========================================================

void clear()
{
    QTextStream out(stdout);
    out << QString("\033[2J");
}
//==============================================================================
void clearUp()
{
    QTextStream out(stdout);
    out << QString("\033[1J");
}
//==============================================================================
void clearDown()
{
    QTextStream out(stdout);
    out << QString("\033[0J");
}
//==============================================================================
void savePos()
{
    QTextStream out(stdout);
    out << QString("\033[s");
}
//==============================================================================
void restorePos()
{
    QTextStream out(stdout);
    out << QString("\033[u");
}
//==============================================================================
void setPos(int row, int col)
{
    QTextStream out(stdout);
    out << QString("\033[%1;%2H").arg(row).arg(col);
}
//==============================================================================
void setPosUp(int step)
{
    QTextStream out(stdout);
    out << QString("\033[%1A").arg(step);
}
//==============================================================================
void setPosDown(int step)
{
    QTextStream out(stdout);
    out << QString("\033[%1B").arg(step);
}
//==============================================================================
void setPosBack(int step)
{
    QTextStream out(stdout);
    out << QString("\033[%1D").arg(step);
}
//==============================================================================
void setPosForward(int step)
{
    QTextStream out(stdout);
    out << QString("\033[%1C").arg(step);
}
//==============================================================================
void toNextLine(int col)
{
    QTextStream out(stdout);
    out << QString("\033[%1E").arg(col);
}
//==============================================================================
void toPrevLine(int col)
{
    QTextStream out(stdout);
    out << QString("\033[%1F").arg(col);
}
//==============================================================================
void eraseLine()
{
    QTextStream out(stdout);
    out << QString("\033[2K");
}
//==============================================================================
void eraseLineLeft()
{
    QTextStream out(stdout);
    out << QString("\033[1K");
}
//==============================================================================
void eraseLineRight()
{
    QTextStream out(stdout);
    out << QString("\033[0K");
}
//==============================================================================
void write(int row, int col, const QString &text)
{
    setPos(row, col);
    write(text);
}
//==============================================================================
void write(const QString &text)
{
    QTextStream out(stdout);
    out << text;
}
//==============================================================================
QVariant read(const QVariant &defaultValue)
{
    QTextStream in(stdin);
    QString str = in.readLine();
    return (str.isEmpty() ? defaultValue : str);
}
//==============================================================================
void hideCursor()
{
    QTextStream out(stdout);
    out << QString("\033[?25l");
}
//==============================================================================
void showCursor()
{
    QTextStream out(stdout);
    out << QString("\033[?25h");
}
//==============================================================================
void setCursorVisible(bool visible)
{
    if(visible)
        showCursor();
    else
        hideCursor();
}
//==============================================================================
void resetAttributes()
{
    QTextStream out(stdout);
    out << QString("\033[0m");
}
//==============================================================================
void setColors(Color textColor, Color bgColor)
{
    QTextStream out(stdout);
    bool bright = false;
    int textCl = qBound(0, static_cast<int>(textColor), 15);
    int bgCl = qBound(0, static_cast<int>(bgColor), 15);

    if(textCl > 7) {
        bright = true;
        textCl = textCl - 8;
    }

    if(bgCl > 7) {
        bright = true;
        bgCl = bgCl - 8;
    }

    out << QString("\033[%1;%2%3m")
           .arg(40+bgCl)
           .arg(30+textCl)
           .arg(bright ? ";1" : "");
}
//==============================================================================
void setTextColor(Color color)
{
    QTextStream out(stdout);
    bool bright = false;
    int cl = qBound(0, static_cast<int>(color), 15);

    if(cl > 7) {
        bright = true;
        cl = cl - 8;
    }

    out << QString("\033[%1%2m").arg(30+cl).arg(bright ? ";1" : "");
}
//==============================================================================
void setBgColor(Color color)
{
    QTextStream out(stdout);
    bool bright = false;
    int cl = qBound(0, static_cast<int>(color), 15);

    if(cl > 7) {
        bright = true;
        cl = cl - 8;
    }

    out << QString("\033[%1%2m").arg(40+cl).arg(bright ? ";1" : "");
}
//==============================================================================
void blink(bool on)
{
    QTextStream out(stdout);
    out << QString("\033[%1m").arg(on ? 5 : 25);
}
//==============================================================================
void frame(int row, int col, int width, int height, bool doubleBorder, Color color)
{
    if(color != ColorNone) setTextColor(color);

    QTextStream out(stdout);
    out << QString("\033[%1;%2H").arg(row).arg(col);
    out << QString(doubleBorder ? "╔" : "┌");

    for (int i=0; i<width-2; ++i) {
        out << QString(doubleBorder ? "═" : "─");
    }

    out << QString(doubleBorder ? "╗" : "┐");

    for (int i = 0; i < height - 2; ++i) {
        out << QString("\033[%1;%2H").arg(row+1+i).arg(col);
        out << QString(doubleBorder ? "║" : "│");
        out << QString("\033[%1;%2H").arg(row+1+i).arg(col+width-1);
        out << QString(doubleBorder ? "║" : "│");
    }

    out << QString("\033[%1;%2H").arg(row+height-1).arg(col);
    out << QString(doubleBorder ? "╚" : "└");

    for (int i = 0; i < width - 2; ++i) {
        out << QString(doubleBorder ? "═" : "─");
    }

    out << QString(doubleBorder ? "╝" : "┘");
}
//==============================================================================
void rectangle(int row, int col, int width, int height, Color color)
{
    if(color != ColorNone) setBgColor(color);

    QTextStream out(stdout);

    for (int y = row; y < row + height; ++y) {
        out << QString("\033[%1;%2H").arg(y).arg(col);

        for (int i = 0; i < width; ++i) {
            out << QString(" ");
        }
    }
}
//==============================================================================
void frame(const QRect &rect, bool doubleBorder, Color color)
{
    frame(rect.top(), rect.left(), rect.width(), rect.height(), doubleBorder, color);
}
//==============================================================================
void rectangle(const QRect &rect, Color color)
{
    rectangle(rect.top(), rect.left(), rect.width(), rect.height(), color);
}
//==============================================================================
QSize getSize(const QSize &defaultSize)
{
    int columns = 0, rows = 0;
#ifdef Q_OS_WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    columns = w.ws_col;
    rows = w.ws_row;
#endif

    if(columns <= 0)
        columns = defaultSize.width();

    if(rows <= 0)
        rows = defaultSize.height();

    return QSize(columns, rows);
}
//==============================================================================
int width()
{
    return getSize().width();
}
//==============================================================================
int height()
{
    return getSize().height();
}
//==============================================================================

} // namespace console //=======================================================

