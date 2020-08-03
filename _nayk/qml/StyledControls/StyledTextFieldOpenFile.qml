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
import QtQuick.Dialogs 1.2
import StyledControls 1.0

StyledTextField {
    id: root
    buttonVisible: true
    buttonText: "..."
    onButtonClicked: _fileDialog.open()
    property alias dialogTitle: _fileDialog.title
    property alias folder: _fileDialog.folder
    property alias defaultSuffix: _fileDialog.defaultSuffix
    property alias nameFilters: _fileDialog.nameFilters
    property alias nameFilterIndex: _fileDialog.selectedNameFilterIndex
    property alias selectedNameFilter: _fileDialog.selectedNameFilter
    property alias selectExisting: _fileDialog.selectExisting
    property alias selectFolder: _fileDialog.selectFolder

    FileDialog {
        id: _fileDialog
        title: qsTr("Please choose a file")
        nameFilters: [ qsTr("All files (*)") ]
        folder: shortcuts.home
        selectMultiple: false
        selectExisting: true
        selectFolder: false
        onAccepted: root.text = _fileDialog.fileUrl
    }
}
