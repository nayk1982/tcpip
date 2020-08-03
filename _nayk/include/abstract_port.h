/****************************************************************************
** Copyright (c) 2020 Evgeny Teterin (nayk) <sutcedortal@gmail.com>
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
#ifndef ABSTRACT_PORT_H
#define ABSTRACT_PORT_H

#include <QObject>
#if !defined (WITHOUT_LOG)
#    include "Log"
#endif

namespace nayk { //=============================================================

//==============================================================================
class AbstractPort : public QObject
{
    Q_OBJECT
    const qint64 maxReadCount {10240};

public:
    explicit AbstractPort(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~AbstractPort() {}
    virtual bool open(bool readOnly = false) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    virtual bool isReady() { return true; }
    inline int write(char byte) { return write(&byte, 1); }
    inline qint64 write(const QByteArray &bytes) { return write( bytes.constData(), bytes.size() ); }
    qint64 write(const char *bytes, qint64 bytesCount) { return writeData(bytes, bytesCount); }
    QString lastError() const { return m_lastError; }
    QByteArray read(qint64 count = -1)
    {
        if(count == 0) return QByteArray();
        else if(count < 0) count = maxReadCount;
        QByteArray buf(count, 0);
        int n = read(buf.data(), count);
        return (n > 0) ? buf.left(n) : QByteArray();
    }
    qint64 read(char *bytes, qint64 count) { return readData(bytes, count); }
    QByteArray readBuffer() const { return m_buffer; }
    bool autoRead() const { return m_autoRead; }
    void setAutoRead(bool autoRead) { m_autoRead = autoRead; }
    virtual bool setPortName(const QString &portName)
    {
        m_portName = portName;
        return true;
    }
    QString portName() const { return m_portName; }

signals:
#if !defined (WITHOUT_LOG)
    void toLog(const QString &text, nayk::Log::LogType logType = nayk::Log::LogInfo);
#endif
    void portError();
    void portOpen();
    void portClose();
    void bytesRead(qint64 count);
    void bytesWrite(qint64 count);
    void readyRead();
    void readyChange(bool ready);

protected:
    bool m_autoRead {true};
    QString m_lastError {""};
    QString m_portName {""};
    QByteArray m_buffer;

    virtual qint64 readData(char *bytes, qint64 count) = 0;
    virtual qint64 writeData(const char *bytes, qint64 bytesCount) = 0;
};
//==============================================================================

} // namespace //===============================================================
#endif // ABSTRACT_PORT_H
