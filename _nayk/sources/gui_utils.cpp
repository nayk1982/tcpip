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
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QSpacerItem>

#include "AppCore"
#include "FileSys"
#include "ImagesConst"
#include "gui_utils.h"

namespace gui_utils { // =======================================================

using namespace nayk;

//==============================================================================
void moveToCenterScreen(QWidget *widget, int screenNumber)
{
    if(!widget) return;
    if(screenNumber >= QApplication::screens().size() ) return;

    QScreen* screen = (screenNumber < 0)
            ? QApplication::primaryScreen()
            : QApplication::screens().at(screenNumber);
    widget->move( screen->availableGeometry().left()
                  + (screen->availableGeometry().width() - widget->geometry().width()) / 2,
                  screen->availableGeometry().top()
                  + (screen->availableGeometry().height() - widget->geometry().height()) / 2
                  );
}
//==============================================================================
void messageError(QWidget *parent, const QString &text)
{
    QMessageBox::critical(parent, QObject::tr("Error"), text, QMessageBox::Ok);
}
//==============================================================================
void messageWarning(QWidget *parent, const QString &text)
{
    QMessageBox::warning(parent, QObject::tr("Warning"), text, QMessageBox::Ok);
}
//==============================================================================
void messageInfo(QWidget *parent, const QString &text)
{
    QMessageBox::information(parent, QObject::tr("Information"), text, QMessageBox::Ok);
}
//==============================================================================
bool messageConfirm(QWidget *parent, const QString &text)
{
    return QMessageBox::question(parent,
                                 QObject::tr("Confirmation"),
                                 text,
                                 QMessageBox::Yes, QMessageBox::Cancel
                                 )
            == QMessageBox::Yes;
}
//==============================================================================
int messageDialog(QWidget *parent, const QString &text)
{
    return QMessageBox::question(parent,
                                 QObject::tr("Question"),
                                 text,
                                 QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel
                                 );
}
//==============================================================================
void messageError(const QString &text)
{
    messageError(mainWindowWidget(), text);
}
//==============================================================================
void messageWarning(const QString &text)
{
    messageWarning(mainWindowWidget(), text);
}
//==============================================================================
void messageInfo(const QString &text)
{
    messageInfo(mainWindowWidget(), text);
}
//==============================================================================
bool messageConfirm(const QString &text)
{
    return messageConfirm(mainWindowWidget(), text);
}
//==============================================================================
int messageDialog(const QString &text)
{
    return messageDialog(mainWindowWidget(), text);
}
//==============================================================================
QWidget *mainWindowWidget()
{
    QWidgetList list = QApplication::topLevelWidgets();

    for ( QWidget *widget : list ) {
        if ( QString(widget->metaObject()->className()) == QString("MainWindow"))
            return widget;
    }

    return list.isEmpty() ? nullptr : list.first();
}
//==============================================================================
void showAboutDialog(const QString &title, const QString &developer,
                     const QString &description, bool showOrganization)
{
    QWidget *mainWidget = mainWindowWidget();

    if(!mainWidget)
        return;

    QDialog *dialogAbout = mainWidget->findChild<QDialog*>("dialogAbout");

    if(dialogAbout)
        return;

    dialogAbout = new QDialog(mainWidget);
    dialogAbout->setObjectName("dialogAbout");
    dialogAbout->setAttribute(Qt::WA_DeleteOnClose);
    dialogAbout->setWindowTitle( QObject::tr("About") );
    dialogAbout->setFixedSize(560, 560);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(dialogAbout);
    vBoxLayout->setAlignment(Qt::AlignCenter);
    vBoxLayout->setMargin(0);
    vBoxLayout->setSpacing(10);

    if(QFile::exists(imageMainTopBanner)) {

        QLabel *labelTopBanner = new QLabel(dialogAbout);
        labelTopBanner->setStyleSheet("background: black;");
        labelTopBanner->setPixmap( QPixmap(imageMainTopBanner) );
        labelTopBanner->setMinimumHeight( labelTopBanner->pixmap()->height() );
        labelTopBanner->setMaximumHeight( labelTopBanner->pixmap()->height() );
        labelTopBanner->setAlignment( Qt::AlignCenter );
        vBoxLayout->addWidget(labelTopBanner);
        dialogAbout->setFixedWidth( labelTopBanner->pixmap()->width() );
    }

    vBoxLayout->addStretch(10);
    QLabel *titleLabel = new QLabel(
                title.isEmpty()
                ? QApplication::applicationDisplayName()
                : title, dialogAbout );
    titleLabel->setStyleSheet("font-size: 18pt;");
    titleLabel->setAlignment( Qt::AlignCenter );
    titleLabel->sizePolicy().setHorizontalPolicy( QSizePolicy::Minimum );

    if(QFile::exists(imageMainIcon)) {

        QHBoxLayout *titleLayout = new QHBoxLayout;
        QLabel *labelIcon = new QLabel(dialogAbout);
        labelIcon->setPixmap( QPixmap(imageMainIcon) );
        labelIcon->setMinimumHeight( labelIcon->pixmap()->height() );
        labelIcon->setMaximumHeight( labelIcon->pixmap()->height() + 40 );
        labelIcon->setMinimumWidth( labelIcon->pixmap()->width() + 30 );
        labelIcon->setMaximumWidth( labelIcon->pixmap()->width() + 40 );
        labelIcon->setAlignment( Qt::AlignCenter );

        titleLayout->addStretch();
        titleLayout->addWidget(labelIcon);
        titleLayout->addWidget(titleLabel);
        titleLayout->addStretch();
        vBoxLayout->addLayout(titleLayout);
    }
    else {
        titleLabel->setMinimumHeight( 40 );
        titleLabel->setMaximumHeight( 80 );
        vBoxLayout->addWidget(titleLabel);
    }

    vBoxLayout->addStretch(20);
    QGridLayout *gridLayout = new QGridLayout;
    int row = 0;

    const QString labelStyle = "font-size: 11pt;";
    const QString valueStyle = "font-size: 11pt; font-weight: bold; padding-left: 10px;";

    QLabel *label = new QLabel( QObject::tr("Version") + ":", dialogAbout );
    label->setStyleSheet(labelStyle);
    gridLayout->addWidget(label, row, 0);
    label = new QLabel( QApplication::applicationVersion(), dialogAbout );
    label->setStyleSheet(valueStyle);
    gridLayout->addWidget(label, row, 1);
    row++;

#if defined (APP_BUILD_DATE)
    label = new QLabel( QObject::tr("Release date") + ":", dialogAbout );
    label->setStyleSheet(labelStyle);
    gridLayout->addWidget(label, row, 0);
    label = new QLabel( QString(APP_BUILD_DATE), dialogAbout );
    label->setStyleSheet(valueStyle);
    gridLayout->addWidget(label, row, 1);
    row++;
#endif

    if(showOrganization) {
        label = new QLabel( QObject::tr("Company") + ":", dialogAbout );
        label->setStyleSheet(labelStyle);
        gridLayout->addWidget(label, row, 0);
        label = new QLabel( QApplication::organizationName(), dialogAbout );
        label->setStyleSheet(valueStyle);
        gridLayout->addWidget(label, row, 1);
        row++;
    }

    if(!developer.isEmpty()) {
        label = new QLabel( QObject::tr("Developer") + ":", dialogAbout );
        label->setStyleSheet(labelStyle);
        gridLayout->addWidget(label, row, 0);
        label = new QLabel( developer, dialogAbout );
        label->setStyleSheet(valueStyle);
        gridLayout->addWidget(label, row, 1);
        row++;
    }

    QHBoxLayout *infoLayout = new QHBoxLayout;
    infoLayout->addStretch();
    infoLayout->addLayout(gridLayout);
    infoLayout->addStretch();
    vBoxLayout->addLayout(infoLayout);

    if(!description.isEmpty()) {

        QPlainTextEdit *plainText = new QPlainTextEdit( description, dialogAbout );
        plainText->setReadOnly(true);
        plainText->sizePolicy().setHorizontalPolicy( QSizePolicy::Expanding );
        plainText->sizePolicy().setVerticalPolicy( QSizePolicy::Expanding );
        plainText->setWordWrapMode( QTextOption::WordWrap );

        QHBoxLayout *layout = new QHBoxLayout;
        layout->addSpacerItem( new QSpacerItem(20, 20) );
        layout->addWidget(plainText);
        layout->addSpacerItem( new QSpacerItem(20, 20) );
        vBoxLayout->addSpacerItem( new QSpacerItem(20, 20) );
        vBoxLayout->addLayout(layout);
    }
    else {
        vBoxLayout->addStretch(100);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok, Qt::Horizontal, dialogAbout );
    buttonBox->setCenterButtons(true);
    dialogAbout->connect( buttonBox, &QDialogButtonBox::accepted, dialogAbout, &QDialog::close );
    vBoxLayout->addWidget(buttonBox);
    vBoxLayout->addSpacerItem( new QSpacerItem(20, 20) );
    dialogAbout->setLayout( vBoxLayout );
    dialogAbout->show();
}
//==============================================================================
void updateApplicationStyle()
{
    QWidget *mainWnd = mainWindowWidget();
    QString qss = mainWnd ? mainWnd->styleSheet() : "";
    QString qssFileName = app_core::applicationRootPath() + "style.qss";
    QByteArray data;
    QString err;

    if(file_sys::fileExists( qssFileName )
            && file_sys::readFile(qssFileName, data, err)) {

        qss = QString(data);
    }

    if(mainWnd) mainWnd->setStyleSheet("");
    qApp->setStyleSheet(qss);
}
//==============================================================================

} // namespace nayk ============================================================
