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
#ifndef DIALOG_LOG_H
#define DIALOG_LOG_H

#include <QObject>
#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

#include "Log"

namespace nayk { //=============================================================

const QString clDefault[2]   { "#111416", "#fdfdfd" };
const QString clLogDate[2]   { "#929292", "#333333" };
const QString clLogPrefix[2] { "#c88dee", "#562873" };
const QString clLogInf[2]    { "#ffffff", "#000000" };
const QString clLogWrn[2]    { "#ff9c54", "#8d3c00" };
const QString clLogErr[2]    { "#ff4040", "#a50000" };
const QString clLogIn[2]     { "#55d864", "#003706" };
const QString clLogOut[2]    { "#dd69bb", "#53003b" };
const QString clLogTxt[2]    { "#d8d8d8", "#1f1f1f" };
const QString clLogDbg[2]    { "#00ddc6", "#006766" };
const QString clLogOther[2]  { "#8f8f8f", "#888888" };

//==============================================================================
class DialogLog: public QDialog
{
    Q_OBJECT
    static const int defaultMaximumBlockCount {5000};

public:
    explicit DialogLog(QWidget *parent = nullptr);
    explicit DialogLog(int maximumBlockCount = defaultMaximumBlockCount, QWidget *parent = nullptr);
    ~DialogLog();
    int maximumBlockCount() const;
    void setMaximumBlockCount(int maximumBlockCount);
    bool openLogDirButtonVisible() const;
    void setOpenLogDirButtonVisible(bool openLogDirButtonVisible);
    static QString highlight(const QString &text, bool dark = false);
    static QString highlight(const QString &text, Log::LogType logType, bool dark = false);

signals:
    void openLogDirButtonClicked();

private:
    int m_maximumBlockCount {defaultMaximumBlockCount};
    bool m_openLogDirButtonVisible {false};
    bool m_actionEnable {false};
    bool m_dark {true};
    QString m_filtrStr {""};
    QStringList logList;
    QTextEdit *textEditLog {nullptr};
    QLineEdit *lineEditFilter {nullptr};
    QPushButton *pushButtonOpenLogDir {nullptr};

    void initializeDialog();
    void checkBlockCount();
    void applyFilter();

public slots:
    void saveToLog(const QString &text, Log::LogType logType = Log::LogInfo);
    void write(const QString &text);

private slots:
    void lineEditFilter_editingFinished();
    void pushButtonClear_clicked();
    void checkBoxDark_toggled(bool checked);
};
//==============================================================================

} // namespace nayk //==========================================================
#endif // DIALOG_LOG_H
