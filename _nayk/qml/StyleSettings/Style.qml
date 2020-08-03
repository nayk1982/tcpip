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

QtObject {
    readonly property int fontPointSize           : 13
    readonly property int smallFontPointSize      : Math.max(8, fontPointSize - 4)
    readonly property int bigFontPointSize        : fontPointSize + 4
    readonly property int buttonWidth             : 130
    readonly property int buttonHeight            : 56
    readonly property real disabledControlOpacity : 0.4
    readonly property color accent                : "#00BCD4"
    readonly property color primary               : "#2f4649"
    readonly property color foreground            : "#cddee2"
    readonly property color background            : "#2c2f30"
    readonly property color bgColor               : background
    readonly property color bgDarkColor           : "#212728"
    readonly property color bgLightColor          : primary
    readonly property color bgAlternateColor      : bgDarkColor
    readonly property color textColor             : foreground
    readonly property color textDarkColor         : "#647174"
    readonly property color textLightColor        : "white"
    readonly property color highlightColor        : "#00BCD4"
}
