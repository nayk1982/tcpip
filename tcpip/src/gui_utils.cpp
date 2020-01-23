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
#include "gui_utils.h"

namespace gui_utils { // =======================================================

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
    QMessageBox::critical(parent, QObject::tr("Ошибка"), text, QMessageBox::Ok);
}
//==============================================================================
void messageWarning(QWidget *parent, const QString &text)
{
    QMessageBox::warning(parent, QObject::tr("Внимание"), text, QMessageBox::Ok);
}
//==============================================================================
void messageInfo(QWidget *parent, const QString &text)
{
    QMessageBox::information(parent, QObject::tr("Информация"), text, QMessageBox::Ok);
}
//==============================================================================
bool messageConfirm(QWidget *parent, const QString &text)
{
    return QMessageBox::question(parent,
                                 QObject::tr("Подтверждение"),
                                 text,
                                 QMessageBox::Yes, QMessageBox::Cancel
                                 )
            == QMessageBox::Yes;
}
//==============================================================================
int messageDialog(QWidget *parent, const QString &text)
{
    return QMessageBox::question(parent,
                                 QObject::tr("Вопрос"),
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
    for ( QWidget *widget : QApplication::topLevelWidgets() ) {
        if ( QString(widget->metaObject()->className()) == QString("MainWindow"))
            return widget;
    }
    return nullptr;
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

} // namespace =================================================================
