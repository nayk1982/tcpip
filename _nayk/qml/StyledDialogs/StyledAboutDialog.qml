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
import QtQuick.Window 2.12
import StyledControls 1.0
import Utils 1.0
import StyleSettings 1.0

Dialog {
    id: root
    title: qsTr("About")

    property alias applicationIcon: _img.source
    property alias applicationTitle: _title.text
    property alias applicationVersion: _textVersion.text
    property alias applicationBuildDate: _textDate.text
    property alias applicationAuthor: _textAuthor.text
    property string applicationMail: "sutcedortal@gmail.com"
    property string applicationUrl: "https://nayk1982.github.io"
    property alias applicationDescription: _descriptionEdit.text

    contentItem: Pane {
        id: _rect
        implicitWidth: Math.min( Screen.desktopAvailableWidth - Utils.dp(40), Utils.dp(480) )
        implicitHeight: Math.min( Screen.desktopAvailableHeight - Utils.dp(40), Utils.dp(580) )
        anchors.fill: parent

        Item {
            id: _headerItem
            height: Utils.dp(64)
            width: parent.width

            anchors {
                top: parent.top
                topMargin: Utils.dp(30)
            }

            Image {
                id: _img
                height: parent.height
                width: height

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: (parent.width - width - _title.contentWidth - Utils.dp(40)) / 2
                }

                source: "images/information.png"
                horizontalAlignment: Image.AlignHCenter
                verticalAlignment: Image.AlignVCenter
                visible: source !== ""
                fillMode: Image.Stretch
            }

            StyledText {
                id: _title
                height: parent.height

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: _img.visible ? _img.right : parent.left
                    leftMargin: _img.visible ? Utils.dp(20) : (parent.width - contentWidth) / 2
                }

                text: Qt.application.displayName
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                color: Style.textLightColor

                font {
                    pointSize: Style.bigFontPointSize
                    bold: true
                }
            }
        }

        Row {
            id: _rowVersion

            anchors {
                top: _headerItem.bottom
                left: parent.left
                leftMargin: (parent.width - _labelVersion.width - _textVersion.width) / 2
                topMargin: Utils.dp(30)
            }

            spacing: Utils.dp(10)
            visible: _textVersion.text !== ""

            StyledText {
                id: _labelVersion
                width: _rowVersion.spacing + Math.max( contentWidth, _labelDate.contentWidth, _labelAuthor.contentWidth, _labelMail.contentWidth, _labelUrl.contentWidth )
                height: contentHeight * 1.6
                text: qsTr("Version:")
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                id: _textVersion
                width: _rowVersion.spacing + Math.max( contentWidth, _textDate.contentWidth, _textAuthor.contentWidth, _textMail.contentWidth, _textUrl.contentWidth )
                height: _labelVersion.height
                font.bold: true
                text: Qt.application.version
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                color: Style.textLightColor
            }
        }

        Row {
            id: _rowDate

            anchors {
                top: _rowVersion.visible ? _rowVersion.bottom : _rowVersion.top
                left: _rowVersion.left
            }

            spacing: Utils.dp(10)
            visible: _textDate.text !== ""

            StyledText {
                id: _labelDate
                width: _labelVersion.width
                height: _labelVersion.height
                text: qsTr("Build date:")
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                id: _textDate
                width: _textVersion.width
                height: _labelVersion.height
                font.bold: true
                text: "Unknown"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                color: Style.textLightColor
            }
        }

        Row {
            id: _rowAuthor

            anchors {
                top: _rowDate.visible ? _rowDate.bottom : _rowDate.top
                left: _rowDate.left
            }

            spacing: _rowDate.spacing
            visible: _textAuthor.text !== ""

            StyledText {
                id: _labelAuthor
                width: _labelVersion.width
                height: _labelVersion.height
                text: qsTr("Author:")
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                id: _textAuthor
                width: _textVersion.width
                height: _labelVersion.height
                font.bold: true
                text: qsTr("Evgeniy Teterin")
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                color: Style.textLightColor
            }
        }

        Row {
            id: _rowMail

            anchors {
                top: _rowAuthor.visible ? _rowAuthor.bottom : _rowAuthor.top
                left: _rowAuthor.left
            }

            spacing: _rowDate.spacing
            visible: root.applicationMail !== ""

            StyledText {
                id: _labelMail
                width: _labelVersion.width
                height: _labelVersion.height
                text: qsTr("E-Mail:")
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                id: _textMail
                width: _textVersion.width
                height: _labelVersion.height

                font {
                    bold: true
                    underline: false
                }

                text: "<a href=\"mailto:" + root.applicationMail + "\">" + root.applicationMail + "</a>"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                onLinkActivated: Qt.openUrlExternally(link)
                color: Style.highlightColor
                linkColor: Style.highlightColor

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }

        Row {
            id: _rowUrl

            anchors {
                top: _rowMail.visible ? _rowMail.bottom : _rowMail.top
                left: _rowMail.left
            }

            spacing: _rowDate.spacing
            visible: root.applicationUrl !== ""

            StyledText {
                id: _labelUrl
                width: _labelVersion.width
                height: _labelVersion.height
                text: qsTr("Web site:")
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                id: _textUrl
                width: _textVersion.width
                height: _labelVersion.height

                font {
                    bold: true
                    underline: false
                }

                text: "<a href=\"" + root.applicationUrl + "\">" + root.applicationUrl + "</a>"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                onLinkActivated: Qt.openUrlExternally(link)
                color: Style.highlightColor
                linkColor: Style.highlightColor

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }

        TextArea {
            id: _descriptionEdit

            anchors {
                top: _rowUrl.visible ? _rowUrl.bottom : _rowUrl.top
                horizontalCenter: parent.horizontalCenter
                bottom: _btn.top
                topMargin: Utils.dp(40)
                bottomMargin: Utils.dp(40)
            }

            width: parent.width - Utils.dp(40)
            text: ""
            visible: text !== ""
            readOnly: true
        }

        StyledButtonOk {
            id: _btn

            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: Utils.dp(20)
            }

            onClicked: root.close()
        }
    }
}
