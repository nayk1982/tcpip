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

    connect(ui->lineEditMask, &QLineEdit::textChanged, this, &MainWindow::on_lineEditIp_textChanged);
    connect(ui->lineEditGateway, &QLineEdit::textChanged, this, &MainWindow::on_lineEditIp_textChanged);

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

    return true;
}
//==============================================================================
void MainWindow::updateControls()
{
    bool ok = network_client::isCorrectIp(ui->lineEditIp->text())
            && network_client::isCorrectIp(ui->lineEditMask->text())
            && network_client::isCorrectIp(ui->lineEditGateway->text());

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

    host.addr = ui->lineEditIp->text();
    host.mask = ui->lineEditMask->text();
    host.gateway = ui->lineEditGateway->text();

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
