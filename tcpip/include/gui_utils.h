#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <QWidget>

namespace gui_utils { //========================================================

void moveToCenterScreen(QWidget *widget, int screenNumber = -1);
void messageError(QWidget *parent, const QString &text);
void messageWarning(QWidget *parent, const QString &text);
void messageInfo(QWidget *parent, const QString &text);
bool messageConfirm(QWidget *parent, const QString &text);
int messageDialog(QWidget *parent, const QString &text);
void messageError(const QString &text);
void messageWarning(const QString &text);
void messageInfo(const QString &text);
bool messageConfirm(const QString &text);
int messageDialog(const QString &text);
QWidget *mainWindowWidget();
void updateApplicationStyle();

} // namespace gui_utils =======================================================
#endif // GUI_UTILS_H
