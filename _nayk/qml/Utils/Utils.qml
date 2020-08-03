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
pragma Singleton
import QtQuick 2.13
import QtQuick.Window 2.12

Item {
    property int dpi: Screen.pixelDensity * 25.4
    property bool isMobile: (Qt.platform.os === "android") || (Qt.platform.os === "ios")

    function dp(x) {
        return (dpi < 120) ? x : x * (dpi/160);
    }

    function centeredWindow(window, width, height) {
        window.width = Math.min( width, Screen.desktopAvailableWidth - dp(40) );
        window.height = Math.min( height, Screen.desktopAvailableHeight - dp(40) );
        window.x = (Screen.desktopAvailableWidth - window.width) / 2;
        window.y = (Screen.desktopAvailableHeight - window.height) / 2;
    }
}
