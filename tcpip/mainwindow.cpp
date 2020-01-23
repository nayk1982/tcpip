#include <QApplication>
#include <QScreen>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <QRegExpValidator>
#include <QDialogButtonBox>
#include <QPushButton>
#include "GuiUtils"
#include "AppCore"
#include "FileSys"
#include "SystemUtils"
#include "mainwindow.h"
#include "ui_mainwindow.h"
//==============================================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags( Qt::Dialog );
    QScreen* screen = QApplication::primaryScreen();
    int width = geometry().width();
    int height = geometry().height();
    int left = screen->geometry().left() + (screen->geometry().width() - width) / 2;
    int top = screen->geometry().top() + (screen->geometry().height() - height) / 2;
    setGeometry(left, top, width, height);

    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);

    ui->lineEditIp->setValidator(ipValidator);
    ui->lineEditMask->setValidator(ipValidator);
    ui->lineEditGateway->setValidator(ipValidator);
    ui->lineEditDns1->setValidator(ipValidator);
    ui->lineEditDns2->setValidator(ipValidator);

    connect(ui->lineEditMask, &QLineEdit::textChanged, this, &MainWindow::on_lineEditIp_textChanged);
    connect(ui->lineEditGateway, &QLineEdit::textChanged, this, &MainWindow::on_lineEditIp_textChanged);
    connect(ui->lineEditDns1, &QLineEdit::textChanged, this, &MainWindow::on_lineEditIp_textChanged);
    connect(ui->lineEditDns2, &QLineEdit::textChanged, this, &MainWindow::on_lineEditIp_textChanged);

    connect(ui->groupBoxDns, &QGroupBox::toggled, this, &MainWindow::on_groupBoxAddress_toggled);

    readParameters();
    updateControls();
    m_actionEnabled = true;
}
//==============================================================================
MainWindow::~MainWindow()
{
    delete ui;
}
//==============================================================================
bool MainWindow::readParameters()
{
    QString err;
    if(!network_client::hostSettings( m_host, err )) {

        m_lastError = tr("Не удалось получить данные:\n%1").arg(err);
        return false;
    }

    ui->labelTitle->setText(m_host.name);
    ui->lineEditIp->setText(m_host.addr);
    ui->lineEditMask->setText(m_host.mask);
    ui->lineEditGateway->setText(m_host.gateway);
    ui->lineEditDns1->setText(m_host.dns1);
    ui->lineEditDns2->setText(m_host.dns2);
    ui->groupBoxAddress->setChecked(!m_host.dhcp);
    ui->groupBoxDns->setCheckable( m_host.dhcp );
    if(m_host.dhcp) ui->groupBoxDns->setChecked(!m_host.autoDns);

    return true;
}
//==============================================================================
void MainWindow::updateControls()
{
    bool ok {true};

    if(ui->groupBoxAddress->isChecked()) {

        if(ui->groupBoxDns->isCheckable()) {
            ui->groupBoxDns->setChecked(true);
            ui->groupBoxDns->setCheckable(false);
            ui->groupBoxDns->setEnabled(true);
        }

        ok = ok && network_client::isCorrectIp(ui->lineEditIp->text())
            && network_client::isCorrectIp(ui->lineEditMask->text())
            && network_client::isCorrectIp(ui->lineEditGateway->text());
    }
    else if (!ui->groupBoxDns->isCheckable()) {
        ui->groupBoxDns->setCheckable(true);
    }

    if(!ui->groupBoxDns->isCheckable() || ui->groupBoxDns->isChecked()) {

        if(!ui->lineEditDns1->text().isEmpty())
            ok = ok && network_client::isCorrectIp(ui->lineEditDns1->text());
        if(!ui->lineEditDns2->text().isEmpty())
            ok = ok && network_client::isCorrectIp(ui->lineEditDns2->text());
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
//==============================================================================
void MainWindow::on_lineEditIp_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    if(m_actionEnabled) updateControls();
}
//==============================================================================
void MainWindow::on_buttonBox_accepted()
{
    network_client::TcpIp4Struct host = m_host;

    host.dhcp = !ui->groupBoxAddress->isChecked();

    if(!host.dhcp) {
        host.addr = ui->lineEditIp->text();
        host.mask = ui->lineEditMask->text();
        host.gateway = ui->lineEditGateway->text();
    }

    host.autoDns = ui->groupBoxDns->isCheckable() && !ui->groupBoxDns->isChecked();

    if(!host.autoDns) {
        host.dns1 = ui->lineEditDns1->text();
        host.dns2 = ui->lineEditDns2->text();
    }

    QString result;

    if(network_client::setHostSettings(host, result)) {

        QString fileName = app_core::applicationFullPath() + "tcpip.dat";
        QString err;
        file_sys::writeFile( fileName, result.toLocal8Bit(), err );
        close();
    }
    else {
        gui_utils::messageError(tr("Ошибка выполнения:\n%1").arg(result));
    }
}
//==============================================================================
void MainWindow::on_buttonBox_rejected()
{
    close();
}
//==============================================================================
void MainWindow::on_groupBoxAddress_toggled(bool arg1)
{
    Q_UNUSED(arg1)
    if(m_actionEnabled) {
        m_actionEnabled = false;
        updateControls();
        m_actionEnabled = true;
    }
}
//==============================================================================
