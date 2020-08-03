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
#ifndef NAYK_TELEGRAM_H
#define NAYK_TELEGRAM_H
//==============================================================================
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QHttpMultiPart>
#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <QDateTime>
//
#include "Log"
#include "HttpClient"
//==============================================================================
namespace nayk {

//==============================================================================
class Telegram : public QObject
{
    Q_OBJECT
public:
    const QString telegram_api_url = "https://api.telegram.org/";
    //
    enum DocType { Doc_Unknown = 0, Doc_Photo, Doc_Document, Doc_Audio, Doc_Video };
    Q_ENUM(DocType)

    enum ChatType { Chat_Unknown = 0, Chat_Private, Chat_Group, Chat_Supergroup, Chat_Channel };
    Q_ENUM(ChatType)

    enum UserType { User_Unknown = 0, User_User, User_Admin, User_Bot };
    Q_ENUM(UserType)

    enum MsgType { Msg_Unknown = 0, Msg_Text, Msg_Command, Msg_Photo, Msg_Document };
    Q_ENUM(MsgType)

    //
    typedef struct ChatStruct {
        qint64 id {0};
        QString title {""};
        ChatType type {Chat_Unknown};
    } ChatStruct;
    //
    typedef struct UserStruct {
        qint64 id {0};
        QString name {""};
        QString firstName {""};
        QString lastName {""};
        UserType type {User_Unknown};
    } UserStruct;
    //
    typedef struct MsgStruct {
        qint64 id {0};
        QDateTime date {QDateTime::fromMSecsSinceEpoch(-1)};
        QString text {""};
        MsgType type {Msg_Unknown};
    } MsgStruct;
    //
    typedef struct PhotoStruct {
        QString file_id {""};
        QString caption {""};
        qint32 width {0};
        qint32 height {0};
        qint64 file_size {0};
    } PhotoStruct;
    //
    typedef struct DocumentStruct {
        QString file_id {""};
        QString caption {""};
        DocType docType {Doc_Unknown};
    } DocumentStruct;
    //
    typedef struct CallbackStruct {
        QString id {""};
        QString chat_instance {""};
        QString data {""};
        qint64 from_msg_id {0};
    } CallbackStruct;
    //
    explicit Telegram(QObject *parent = nullptr, const QString &token = QString(),
                      const QString &name = QString());
    ~Telegram();
    void setToken(const QString &token);
    void setName(const QString &name);
    bool readRequest();
    QString lastError() const;
    QString token() const;
    QJsonObject requestObject() const;
    ChatStruct chat() const;
    UserStruct user() const;
    MsgStruct message() const;
    PhotoStruct photo() const;
    DocumentStruct document() const;
    CallbackStruct callback() const;
    qint64 update_id() const;
    bool is_callback() const;
    bool is_reply() const;
    bool is_reply_to_me() const;
    bool is_chat_title_change() const;
    bool is_chat_migrate() const;
    UserStruct reply_user() const;
    MsgStruct reply_message() const;
    qint64 new_chat_id() const;
    qint64 old_chat_id() const;
    QString new_chat_title() const;
    //
    bool sendMessageMarkdown(qint64 chatId, const QString &text,
                             const QJsonObject &replyMarkup = QJsonObject());
    bool sendMessageMarkdown(const QString &text,
                             const QJsonObject &replyMarkup = QJsonObject());
    bool sendMessageHTML(qint64 chatId, const QString &text,
                         const QJsonObject &replyMarkup = QJsonObject());
    bool sendMessageHTML(const QString &text,
                         const QJsonObject &replyMarkup = QJsonObject());
    bool sendMessage(qint64 chatId, const QString &text,
                     const QString &parseMode = QString(""),
                     const QJsonObject &replyMarkup = QJsonObject());
    bool sendMessage(const QString &text, const QString &parseMode = QString(""),
                     const QJsonObject &replyMarkup = QJsonObject());
    bool sendMessage(qint64 chatId, const MsgStruct &message);
    bool sendMessage(const MsgStruct &message);
    //
    bool sendReplyMessage(qint64 chatId, qint64 reply_message_id,
                          const QString &text, const QString &parseMode);
    bool sendReplyMessage(qint64 chatId, qint64 reply_message_id,
                          const QString &text);
    bool sendReplyMessageHTML(qint64 chatId, qint64 reply_message_id,
                              const QString &text);
    bool sendReplyMessageMarkdown(qint64 chatId, qint64 reply_message_id,
                                  const QString &text);
    //
    bool sendPhotoFile(qint64 chatId, const QByteArray &data,
                       const QString &caption = QString(""),
                       const QString &imgType = QString("jpeg"));
    bool sendPhotoFile(const QByteArray &data, const QString &caption = QString(""),
                       const QString &imgType = QString("jpeg"));
    bool sendPhoto(qint64 chatId, const QString &file_id,
                   const QString &caption = QString(""),
                   const QString &parseMode = QString(""),
                   const QJsonObject &replyMarkup = QJsonObject());
    bool sendPhoto(const QString &file_id, const QString &caption = QString(""),
                   const QString &parseMode = QString(""),
                   const QJsonObject &replyMarkup = QJsonObject());
    bool sendPhotoHTML(qint64 chatId, const QString &file_id,
                       const QString &caption = QString(""),
                       const QJsonObject &replyMarkup = QJsonObject());
    bool sendPhotoHTML(const QString &file_id, const QString &caption = QString(""),
                       const QJsonObject &replyMarkup = QJsonObject());
    bool sendPhoto(qint64 chatId, const PhotoStruct &photo);
    bool sendPhoto(const PhotoStruct &photo);
    //
    bool sendAudio(qint64 chatId, const QString &file_id,
                   const QString &caption = QString(""),
                   const QString &parseMode = QString(""),
                   const QJsonObject &replyMarkup = QJsonObject());
    bool sendAudio(const QString &file_id, const QString &caption = QString(""),
                   const QString &parseMode = QString(""),
                   const QJsonObject &replyMarkup = QJsonObject());
    bool sendAudioHTML(qint64 chatId, const QString &file_id,
                       const QString &caption = QString(""),
                       const QJsonObject &replyMarkup = QJsonObject());
    bool sendAudioHTML(const QString &file_id, const QString &caption = QString(""),
                       const QJsonObject &replyMarkup = QJsonObject());
    //
    bool sendVideo(qint64 chatId, const QString &file_id,
                   const QString &caption = QString(""),
                   const QString &parseMode = QString(""),
                   const QJsonObject &replyMarkup = QJsonObject());
    bool sendVideo(const QString &file_id, const QString &caption = QString(""),
                   const QString &parseMode = QString(""),
                   const QJsonObject &replyMarkup = QJsonObject());
    bool sendVideoHTML(qint64 chatId, const QString &file_id,
                       const QString &caption = QString(""),
                       const QJsonObject &replyMarkup = QJsonObject());
    bool sendVideoHTML(const QString &file_id, const QString &caption = QString(""),
                       const QJsonObject &replyMarkup = QJsonObject());
    //
    bool sendDocument(qint64 chatId, const QString &file_id,
                      const QString &caption = QString(""), DocType docType = Doc_Document,
                      const QString &parseMode = QString(""),
                      const QJsonObject &replyMarkup = QJsonObject());
    bool sendDocument(const QString &file_id, const QString &caption = QString(""),
                      DocType docType = Doc_Document,
                      const QString &parseMode = QString(""),
                      const QJsonObject &replyMarkup = QJsonObject());
    bool sendDocumentHTML(qint64 chatId, const QString &file_id,
                          const QString &caption = QString(""),
                          DocType docType = Doc_Document,
                          const QJsonObject &replyMarkup = QJsonObject());
    bool sendDocumentHTML(const QString &file_id, const QString &caption = QString(""),
                          DocType docType = Doc_Document,
                          const QJsonObject &replyMarkup = QJsonObject());
    bool sendDocument(qint64 chatId, const DocumentStruct &document);
    bool sendDocument(const DocumentStruct &document);
    //
    bool deleteMessage(qint64 message_id);
    bool deleteMessage(qint64 chat_id, qint64 message_id);
    bool deleteMessage(const QString &user_name, qint64 message_id);
    //
    bool sendChatAction(qint64 chat_id, const QString &action);
    bool sendChatAction(const QString &action);
    bool sendChatActionTyping(qint64 chat_id);
    bool sendChatActionTyping();
    bool sendChatActionDocument(qint64 chat_id, DocType docType = Doc_Document);
    bool sendChatActionDocument(DocType docType = Doc_Document);
    //
    bool sendSticker(qint64 chat_id, const QString &file_id);
    bool sendSticker(const QString &file_id);
    //
    QJsonObject lastAnswer();
    QString userName();
    static QString getChatTypeText(ChatType chatType);

signals:
    void toLog(const QString &text, Log::LogType logType = Log::LogInfo);

private:
    bool m_is_reply {false};
    qint64 m_update_id {0};
    qint64 m_migrate_to_chat_id {0};
    qint64 m_migrate_from_chat_id {0};
    QString m_token {""};
    QString m_name {""};
    QString m_lastError {""};
    QString m_new_chat_title {QString()};
    QJsonObject m_requestObj;
    ChatStruct m_chat;
    UserStruct m_user;
    MsgStruct m_msg;
    PhotoStruct m_photo;
    DocumentStruct m_document;
    CallbackStruct m_callback;
    UserStruct m_reply_user;
    MsgStruct m_reply_msg;
    nayk::HttpClient *m_http {nullptr};

    //
    bool parseRequest();
    bool parseUserObject(const QJsonObject &obj, UserStruct &user);
    bool parseChatObject(const QJsonObject &obj, ChatStruct &chat);
    void checkParseChatMigrate(const QJsonObject &obj);
    bool sendToTelegram(const QString &url, const QJsonObject &obj);
    bool sendToTelegram(const QString &url, QHttpMultiPart *multiPart);

public slots:
};
//==============================================================================
} // namespace nayk

Q_DECLARE_METATYPE(nayk::Telegram::DocType)

Q_DECLARE_METATYPE(nayk::Telegram::ChatType)

Q_DECLARE_METATYPE(nayk::Telegram::UserType)

Q_DECLARE_METATYPE(nayk::Telegram::MsgType)

Q_DECLARE_METATYPE(nayk::Telegram::ChatStruct)

Q_DECLARE_METATYPE(nayk::Telegram::UserStruct)

Q_DECLARE_METATYPE(nayk::Telegram::MsgStruct)

Q_DECLARE_METATYPE(nayk::Telegram::PhotoStruct)

Q_DECLARE_METATYPE(nayk::Telegram::DocumentStruct)

Q_DECLARE_METATYPE(nayk::Telegram::CallbackStruct)

#endif // NAYK_TELEGRAM_H
