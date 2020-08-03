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
import QtQuick 2.13
import QtQuick.Controls 2.12
import Utils 1.0

TextField {
    id: root
    rightPadding: _toolButton.visible ? _toolButton.width + leftPadding : leftPadding
    renderType: Utils.isMobile ? Text.QtRendering : Text.NativeRendering

    property alias buttonVisible: _toolButton.visible
    property alias buttonText: _toolButton.text
    signal buttonClicked()

    ToolButton {
        id: _toolButton
        height: root.height - 4
        width: root.height

        anchors {
            right: parent.right
            top: parent.top
            topMargin: 2
            rightMargin: 2
        }

        text: "..."
        onClicked: root.buttonClicked()
        visible: false
    }
}
