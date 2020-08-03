#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NetworkClient"
//==============================================================================
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
//==============================================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString lastError() const { return m_lastError; }

private slots:
    void on_lineEditIp_textChanged(const QString &arg1);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_groupBoxAddress_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    bool m_actionEnabled {false};
    QString m_lastError {""};
    network_client::TcpIp4Struct m_host;
    bool readParameters();
    void updateControls();
};
//==============================================================================
#endif // MAINWINDOW_H
