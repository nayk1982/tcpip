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
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QUrl>
//
#include "Convert"
#include "SystemUtils"
#include "HttpConst"
#include "HttpServer"
//
#include "telegram.h"

namespace nayk { //=============================================================

//==============================================================================
Telegram::Telegram(QObject *parent, const QString &token, const QString &name)
    : QObject(parent)
{
    setToken(token);
    setName(name);
    m_http = new HttpClient(this);
}
//==============================================================================
Telegram::~Telegram()
{
    if(m_http) delete m_http;
}
//==============================================================================
void Telegram::setToken(const QString &token)
{
    m_token = token;
    emit toLog(tr("Установка токена: %1").arg(m_token), Log::LogDbg);
}
//==============================================================================
void Telegram::setName(const QString &name)
{
    m_name = name;
    emit toLog(tr("Установка имени: %1").arg(m_name), Log::LogDbg);
}
//==============================================================================
bool Telegram::readRequest()
{
    m_requestObj = QJsonObject();
    m_chat = ChatStruct();
    m_user = UserStruct();
    m_msg = MsgStruct();
    m_photo = PhotoStruct();
    m_document = DocumentStruct();
    m_update_id = 0;
    m_is_reply = false;
    m_reply_user = UserStruct();
    m_reply_msg = MsgStruct();
    m_callback = CallbackStruct();

    HttpServer *server = new HttpServer(this);
    connect(server, &HttpServer::toLog, this, &Telegram::toLog);

    bool ok = server->readRequest();

    if(ok) {

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson( server->requestContent(), &err );
        if(err.error == QJsonParseError::NoError) {

            m_requestObj = doc.object();
            emit toLog( doc.toJson(QJsonDocument::Indented), Log::LogDbg );
        }
        else {
            m_lastError = err.errorString();
            ok = false;
        }
    }
    else {
        m_lastError = server->lastError();
    }

    if(ok) {
        ok = parseRequest();
    }
    else {
        emit toLog(m_lastError, Log::LogError);
    }

    QJsonObject obj;
    obj["ok"] = true;

    server->setResponseHeader( headerContentType, contentTypeJSON );
    server->setResponseContent( QJsonDocument(obj).toJson(QJsonDocument::Compact) );

    if(!server->writeResponse()) {
        emit toLog(server->lastError(), Log::LogError);
    }

    delete server;
    return ok;
}
//==============================================================================
bool Telegram::parseRequest()
{
    emit toLog(tr("Parse Json request."), Log::LogDbg);

    m_update_id = m_requestObj.value("update_id").toVariant().toLongLong();

    if(m_requestObj.contains("callback_query")) {

        QJsonObject callback_query = m_requestObj.value("callback_query").toObject();

        m_callback.id = callback_query.value("id").toString("");
        m_callback.chat_instance = callback_query.value("chat_instance").toString("");
        m_callback.data = callback_query.value("data").toString("");

        if(callback_query.contains("from"))
            parseUserObject(callback_query.value("from").toObject(), m_user);

        if(callback_query.contains("message")) {
            QJsonObject message = callback_query.value("message").toObject();
            m_callback.from_msg_id = message.value("message_id").toVariant().toLongLong();

            if(message.contains("chat")) {
                parseChatObject(message.value("chat").toObject(), m_chat);
            }
        }
    }
    else if(m_requestObj.contains("message")) {
        QJsonObject message = m_requestObj.value("message").toObject();
        m_msg.type = Msg_Text;

        if(message.contains("chat")) {

            QJsonObject chatObj = message.value("chat").toObject();
            parseChatObject(chatObj, m_chat);

            if(chatObj.contains("first_name")) {
                m_user.firstName = chatObj.value("first_name").toString();
            }
            if(chatObj.contains("last_name")) {
                m_user.lastName = chatObj.value("last_name").toString();
            }
            if(chatObj.contains("username")) {
                m_user.name = chatObj.value("username").toString();
            }
        }

        if(message.contains("from")) {

            parseUserObject(message.value("from").toObject(), m_user);
        }
        if(message.contains("date")) {

            m_msg.date = QDateTime::fromMSecsSinceEpoch(
                        1000 * message.value("date").toVariant().toLongLong(), QTimeZone(0) );
            emit toLog( "Message Date = "
                        + m_msg.date.toString("yyyy-MM-dd HH:mm:ss.zzz"), Log::LogDbg);

        }
        if(message.contains("text")) {

            m_msg.text = message.value("text").toString();
        }
        if(message.contains("message_id")) {

            m_msg.id = message.value("message_id").toVariant().toLongLong();
        }
        if(message.contains("photo") && message.value("photo").isArray()) {
            QJsonArray photoArr = message.value("photo").toArray();
            for(int i=0; i<photoArr.size(); i++) {
                if(!photoArr.at(i).isObject()) continue;
                QJsonObject photoObj = photoArr.at(i).toObject();
                if( (photoObj.value("width").toInt() > m_photo.width)
                        && (photoObj.value("height").toInt() > m_photo.height)) {
                    m_photo.width = photoObj.value("width").toInt();
                    m_photo.height = photoObj.value("height").toInt();
                    m_photo.file_size = photoObj.value("file_size").toVariant().toLongLong();
                    m_photo.file_id = photoObj.value("file_id").toString("");
                }
            }
            m_photo.caption = message.value("caption").toString("");

            if(!m_photo.file_id.isEmpty()) {
                m_document.caption = m_photo.caption;
                m_document.docType = Doc_Photo;
                m_document.file_id = m_photo.file_id;
            }
        }
        if(message.contains("document") && message.value("document").isObject()) {
            QJsonObject docObj = message.value("document").toObject();
            m_document.caption = message.value("caption").toString("");
            m_document.docType = Doc_Document;
            m_document.file_id = docObj.value("file_id").toString("");
        }
        if(message.contains("audio") && message.value("audio").isObject()) {
            QJsonObject docObj = message.value("audio").toObject();
            m_document.caption = message.value("caption").toString("");
            m_document.docType = Doc_Audio;
            m_document.file_id = docObj.value("file_id").toString("");
        }
        if(message.contains("video") && message.value("video").isObject()) {
            QJsonObject docObj = message.value("video").toObject();
            m_document.caption = message.value("caption").toString("");
            m_document.docType = Doc_Video;
            m_document.file_id = docObj.value("file_id").toString("");
        }

        if(message.contains("reply_to_message")
                && message.value("reply_to_message").isObject()) {
            m_is_reply = true;
            QJsonObject reply_message = message.value("reply_to_message").toObject();
            if(reply_message.contains("from")) {

                parseUserObject(reply_message.value("from").toObject(), m_reply_user);
            }
            if(reply_message.contains("date")) {

                m_reply_msg.date = QDateTime::fromMSecsSinceEpoch(
                            1000 * reply_message.value("date").toVariant().toLongLong(), QTimeZone(0) );
                emit toLog("Reply Message Date = "
                           + m_reply_msg.date.toString("yyyy-MM-dd HH:mm:ss.zzz"), Log::LogDbg);

            }
            if(reply_message.contains("text")) {

                m_reply_msg.text = reply_message.value("text").toString();
            }
            if(reply_message.contains("message_id")) {

                m_reply_msg.id = reply_message.value("message_id").toVariant().toLongLong();
            }
        }

        checkParseChatMigrate(message);
    }

    if(m_requestObj.contains("edited_channel_post")) {

        checkParseChatMigrate(m_requestObj.value("edited_channel_post").toObject());
    }

    if(!m_photo.file_id.isEmpty() && (m_photo.width > 0) && (m_photo.height > 0)) m_msg.type = Msg_Photo;
    else if(!m_document.file_id.isEmpty()) m_msg.type = Msg_Document;
    else if(!m_msg.text.isEmpty() && (m_msg.text.left(1) == "/")) m_msg.type = Msg_Command;

    return true;
}
//==============================================================================
void Telegram::checkParseChatMigrate(const QJsonObject &obj)
{
    if(obj.contains("new_chat_title"))
        m_new_chat_title = obj.value("new_chat_title").toString();
    if(obj.contains("migrate_to_chat_id"))
        m_migrate_to_chat_id = obj.value("migrate_to_chat_id").toVariant().toLongLong();
    if(obj.contains("migrate_from_chat_id"))
        m_migrate_from_chat_id = obj.value("migrate_from_chat_id").toVariant().toLongLong();
}
//==============================================================================
bool Telegram::parseUserObject(const QJsonObject &obj, UserStruct &user)
{
    user = UserStruct();

    if(obj.contains("first_name") && user.firstName.isEmpty()) {
        user.firstName = obj.value("first_name").toString();
    }
    if(obj.contains("last_name") && user.lastName.isEmpty()) {
        user.firstName = obj.value("last_name").toString();
    }
    if(obj.contains("username") && user.name.isEmpty()) {
        user.name = obj.value("username").toString();
    }
    if(obj.contains("is_bot") && obj.value("is_bot").toBool()) {
        user.type = User_Bot;
    }
    if(obj.contains("id")) {
        user.id = obj.value("id").toVariant().toLongLong();
    }

    return true;
}
//==============================================================================
bool Telegram::parseChatObject(const QJsonObject &obj, ChatStruct &chat)
{
    chat = ChatStruct();

    chat.id = obj.value("id").toVariant().toLongLong();
    if(obj.contains("type")) {
        QString typeStr = obj.value("type").toString();
        if(typeStr == "private") {
            chat.type = Chat_Private;
        }
        else if(typeStr == "group") {
            chat.type = Chat_Group;
        }
        else if(typeStr == "supergroup") {
            chat.type = Chat_Supergroup;
        }
        else if(typeStr == "channel") {
            chat.type = Chat_Channel;
        }
    }
    if(obj.contains("title")) {
        chat.title = obj.value("title").toString();
    }

    return true;
}
//==============================================================================
bool Telegram::sendMessageMarkdown(const QString &text,
                                   const QJsonObject &replyMarkup)
{
    return sendMessageMarkdown(m_chat.id, text, replyMarkup);
}
//==============================================================================
bool Telegram::sendMessageMarkdown(qint64 chatId, const QString &text,
                                   const QJsonObject &replyMarkup)
{
    return sendMessage( chatId, text, "Markdown", replyMarkup );
}
//==============================================================================
bool Telegram::sendMessageHTML(const QString &text, const QJsonObject &replyMarkup)
{
    return sendMessageHTML( m_chat.id, text, replyMarkup );
}
//==============================================================================
bool Telegram::sendMessageHTML(qint64 chatId, const QString &text,
                               const QJsonObject &replyMarkup)
{
    return sendMessage( chatId, text, "HTML", replyMarkup );
}
//==============================================================================
bool Telegram::sendMessage(const QString &text, const QString &parseMode,
                           const QJsonObject &replyMarkup)
{
    return sendMessage(m_chat.id, text, parseMode, replyMarkup);
}
//==============================================================================
bool Telegram::sendMessage(qint64 chatId, const MsgStruct &message)
{
    return sendMessage(chatId, message.text);
}
//==============================================================================
bool Telegram::sendMessage(const MsgStruct &message)
{
    return sendMessage(m_chat.id, message);
}
//==============================================================================
bool Telegram::sendMessage(qint64 chatId, const QString &text,
                           const QString &parseMode, const QJsonObject &replyMarkup)
{
    if(chatId == 0) {

        emit toLog(tr("Не определен chatId"), Log::LogError);
        return false;
    }

    QString url = telegram_api_url + "bot" + m_token + "/sendMessage";
    QJsonObject obj;
    obj["chat_id"] = chatId;
    obj["text"] = text;
    if(!parseMode.isEmpty()) obj["parse_mode"] = parseMode;
    if(!replyMarkup.isEmpty()) obj["reply_markup"] = replyMarkup;

    return sendToTelegram(url, obj);
}
//==============================================================================
bool Telegram::sendReplyMessage(qint64 chatId, qint64 reply_message_id,
                                const QString &text, const QString &parseMode)
{
    if(chatId == 0) {

        emit toLog(tr("Не определен chatId"), Log::LogError);
        return false;
    }

    QString url = telegram_api_url + "bot" + m_token + "/sendMessage";
    QJsonObject obj;
    obj["chat_id"] = chatId;
    obj["text"] = text;
    obj["reply_to_message_id"] = reply_message_id;
    if(!parseMode.isEmpty()) obj["parse_mode"] = parseMode;

    return sendToTelegram(url, obj);
}
//==============================================================================
bool Telegram::sendReplyMessage(qint64 chatId, qint64 reply_message_id,
                                const QString &text)
{
    return sendReplyMessage(chatId, reply_message_id, text, "");
}
//==============================================================================
bool Telegram::sendReplyMessageHTML(qint64 chatId, qint64 reply_message_id,
                                    const QString &text)
{
    return sendReplyMessage(chatId, reply_message_id, text, "HTML");
}
//==============================================================================
bool Telegram::sendReplyMessageMarkdown(qint64 chatId, qint64 reply_message_id,
                                        const QString &text)
{
    return sendReplyMessage(chatId, reply_message_id, text, "Markdown");
}
//==============================================================================
bool Telegram::sendSticker(qint64 chat_id, const QString &file_id)
{
    if(chat_id == 0) {

        emit toLog(tr("Не определен chatId"), Log::LogError);
        return false;
    }

    QString url = telegram_api_url + "bot" + m_token + "/sendSticker";
    QJsonObject obj;
    obj["chat_id"] = chat_id;
    obj["sticker"] = file_id;

    return sendToTelegram(url, obj);
}
//==============================================================================
bool Telegram::sendSticker(const QString &file_id)
{
    return sendSticker(m_chat.id, file_id);
}
//==============================================================================
bool Telegram::sendChatAction(qint64 chat_id, const QString &action)
{
    if(chat_id == 0) {

        emit toLog(tr("Не определен chatId"), Log::LogError);
        return false;
    }

    QString url = telegram_api_url + "bot" + m_token + "/sendChatAction";
    QJsonObject obj;
    obj["chat_id"] = chat_id;
    obj["action"] = action;

    return sendToTelegram(url, obj);
}
//==============================================================================
bool Telegram::sendChatAction(const QString &action)
{
    return sendChatAction(m_chat.id, action);
}
//==============================================================================
bool Telegram::sendChatActionTyping(qint64 chat_id)
{
    return sendChatAction(chat_id, "typing");
}
//==============================================================================
bool Telegram::sendChatActionTyping()
{
    return sendChatActionTyping(m_chat.id);
}
//==============================================================================
bool Telegram::sendChatActionDocument(qint64 chat_id, DocType docType)
{
    QString action = "typing";
    switch (docType) {
    case Doc_Audio:
        action = "upload_audio";
        break;
    case Doc_Video:
        action = "upload_video";
        break;
    case Doc_Photo:
        action = "upload_photo";
        break;
    case Doc_Document:
        action = "upload_document";
        break;
    default:
        break;
    }
    return sendChatAction(chat_id, action);
}
//==============================================================================
bool Telegram::sendChatActionDocument(DocType docType)
{
    return sendChatActionDocument(m_chat.id, docType);
}
//==============================================================================
bool Telegram::sendPhotoFile(const QByteArray &data, const QString &caption,
                             const QString &imgType )
{
    return sendPhotoFile( m_chat.id, data, caption, imgType );
}
//==============================================================================
bool Telegram::sendPhoto(qint64 chatId, const QString &file_id,
                         const QString &caption, const QString &parseMode,
                         const QJsonObject &replyMarkup)
{
    return sendDocument(chatId, file_id, caption, Doc_Photo, parseMode, replyMarkup);
}
//==============================================================================
bool Telegram::sendPhoto(const QString &file_id, const QString &caption,
                         const QString &parseMode, const QJsonObject &replyMarkup)
{
    return sendPhoto(m_chat.id, file_id, caption, parseMode, replyMarkup);
}
//==============================================================================
bool Telegram::sendPhotoHTML(qint64 chatId, const QString &file_id,
                             const QString &caption, const QJsonObject &replyMarkup)
{
    return sendPhoto(chatId, file_id, caption, "HTML", replyMarkup);
}
//==============================================================================
bool Telegram::sendPhotoHTML(const QString &file_id, const QString &caption,
                             const QJsonObject &replyMarkup)
{
    return sendPhoto(file_id, caption, "HTML", replyMarkup);
}
//==============================================================================
bool Telegram::sendPhoto(qint64 chatId, const PhotoStruct &photo)
{
    return sendPhoto(chatId, photo.file_id, photo.caption);
}
//==============================================================================
bool Telegram::sendPhoto(const PhotoStruct &photo)
{
    return sendPhoto(m_chat.id, photo);
}
//==============================================================================
bool Telegram::sendPhotoFile(qint64 chatId, const QByteArray &data,
                             const QString &caption, const QString &imgType )
{
    if(chatId == 0) {

        emit toLog(tr("Не определен chatId"), Log::LogError);
        return false;
    }

    QString url = telegram_api_url + "bot" + m_token + "/sendPhoto?chat_id="
            + QString::number(chatId);
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart captionPart;
    captionPart.setHeader(QNetworkRequest::ContentTypeHeader,
                          QVariant("text/plain"));
    captionPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                          QVariant("form-data; name=\"caption\""));
    captionPart.setBody(caption.toUtf8());

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader,
                        QVariant("image/" + imgType));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"photo\"; filename=\"photo."
                                 + imgType +"\""));
    imagePart.setRawHeader("Content-Transfer-Encoding", "binary");
    imagePart.setBody(data);

    multiPart->append(imagePart);
    multiPart->append(captionPart);

    return sendToTelegram(url, multiPart);
}
//==============================================================================
bool Telegram::sendAudio(qint64 chatId, const QString &file_id,
                         const QString &caption, const QString &parseMode,
                         const QJsonObject &replyMarkup)
{
    return sendDocument(chatId, file_id, caption, Doc_Audio, parseMode, replyMarkup);
}
//==============================================================================
bool Telegram::sendAudio(const QString &file_id, const QString &caption,
                         const QString &parseMode, const QJsonObject &replyMarkup)
{
    return sendAudio(m_chat.id, file_id, caption, parseMode, replyMarkup);
}
//==============================================================================
bool Telegram::sendAudioHTML(qint64 chatId, const QString &file_id,
                             const QString &caption, const QJsonObject &replyMarkup)
{
    return sendAudio(chatId, file_id, caption, "HTML", replyMarkup);
}
//==============================================================================
bool Telegram::sendAudioHTML(const QString &file_id, const QString &caption,
                             const QJsonObject &replyMarkup)
{
    return sendAudio(file_id, caption, "HTML", replyMarkup);
}
//==============================================================================
bool Telegram::sendVideo(qint64 chatId, const QString &file_id,
                         const QString &caption, const QString &parseMode,
                         const QJsonObject &replyMarkup)
{
    return sendDocument(chatId, file_id, caption, Doc_Video, parseMode, replyMarkup);
}
//==============================================================================
bool Telegram::sendVideo(const QString &file_id, const QString &caption,
                         const QString &parseMode, const QJsonObject &replyMarkup)
{
    return sendVideo(m_chat.id, file_id, caption, parseMode, replyMarkup);
}
//==============================================================================
bool Telegram::sendVideoHTML(qint64 chatId, const QString &file_id,
                             const QString &caption, const QJsonObject &replyMarkup)
{
    return sendVideo(chatId, file_id, caption, "HTML", replyMarkup);
}
//==============================================================================
bool Telegram::sendVideoHTML(const QString &file_id, const QString &caption,
                             const QJsonObject &replyMarkup)
{
    return sendVideo(file_id, caption, "HTML", replyMarkup);
}
//==============================================================================
bool Telegram::sendDocument(qint64 chatId, const DocumentStruct &document)
{
    return sendDocument(chatId, document.file_id, document.caption, document.docType);
}
//==============================================================================
bool Telegram::sendDocument(const DocumentStruct &document)
{
    return sendDocument(m_chat.id, document);
}
//==============================================================================
bool Telegram::sendDocument(qint64 chatId, const QString &file_id,
                            const QString &caption, DocType docType,
                            const QString &parseMode, const QJsonObject &replyMarkup)
{
    if(chatId == 0) {

        emit toLog(tr("Не определен chatId"), Log::LogError);
        return false;
    }

    QString sendStr = "/sendDocument";
    QString keyName = "document";
    switch (docType) {
    case Doc_Photo:
        sendStr = "/sendPhoto";
        keyName = "photo";
        break;
    case Doc_Audio:
        sendStr = "/sendAudio";
        keyName = "audio";
        break;
    case Doc_Video:
        sendStr = "/sendVideo";
        keyName = "video";
        break;
    default:
        break;
    }

    QString url = telegram_api_url + "bot" + m_token + sendStr;
    QJsonObject obj;
    obj["chat_id"] = chatId;
    obj[keyName] = file_id;
    if(!caption.isEmpty()) obj["caption"] = caption;
    if(!parseMode.isEmpty()) obj["parse_mode"] = parseMode;
    if(!replyMarkup.isEmpty()) obj["reply_markup"] = replyMarkup;

    return sendToTelegram(url, obj);
}
//==============================================================================
bool Telegram::sendDocument(const QString &file_id, const QString &caption,
                            DocType docType, const QString &parseMode,
                            const QJsonObject &replyMarkup)
{
    return sendDocument(m_chat.id, file_id, caption, docType, parseMode, replyMarkup);
}
//==============================================================================
bool Telegram::sendDocumentHTML(qint64 chatId, const QString &file_id,
                                const QString &caption, DocType docType,
                                const QJsonObject &replyMarkup)
{
    return sendDocument(chatId, file_id, caption, docType, "HTML", replyMarkup);
}
//==============================================================================
bool Telegram::sendDocumentHTML(const QString &file_id, const QString &caption,
                                DocType docType, const QJsonObject &replyMarkup)
{
    return sendDocument(file_id, caption, docType, "HTML", replyMarkup);
}
//==============================================================================
bool Telegram::sendToTelegram(const QString &url, const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    emit toLog(QString("Отправка сообщения на сервер %1").arg(url), Log::LogDbg);
    emit toLog(QString(doc.toJson()), Log::LogDbg);

    m_http->setURL(url);
    m_http->setContentType(contentTypeJSON);

    if(!m_http->sendRequest( doc.toJson(QJsonDocument::Compact) )) {

        emit toLog(m_http->lastError(), Log::LogError);
        return false;
    }

    QJsonParseError err;
    doc = QJsonDocument::fromJson( m_http->replyData(), &err );

    if(err.error != QJsonParseError::NoError) {

        emit toLog(err.errorString(), Log::LogError);
        return false;
    }

    if(doc.isNull() || !doc.isObject()) {

        emit toLog(tr("Ответ не объект Json"), Log::LogError);
        return false;
    }
    else {
        emit toLog( "Ответ:", Log::LogDbg);
        emit toLog( QString(doc.toJson()), Log::LogDbg);
    }
    return true;
}
//==============================================================================
bool Telegram::sendToTelegram(const QString &url, QHttpMultiPart *multiPart)
{
    emit toLog(tr("Отправка сообщения."), Log::LogDbg);

    m_http->setURL(url);
    if(!m_http->sendRequestMultipart( multiPart )) {

        multiPart->deleteLater();
        emit toLog(m_http->lastError(), Log::LogError);
        return false;
    }

    multiPart->deleteLater();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson( m_http->replyData(), &err );

    if(err.error != QJsonParseError::NoError) {

        emit toLog(err.errorString(), Log::LogError);
        return false;
    }

    if(doc.isNull() || !doc.isObject()) {

        emit toLog(tr("Ответ не объект Json"), Log::LogError);
        return false;
    }
    else {
        emit toLog( "Ответ:", Log::LogDbg);
        emit toLog( QString(doc.toJson()), Log::LogDbg);
    }
    return true;
}
//==============================================================================
QJsonObject Telegram::lastAnswer()
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson( m_http->replyData(), &err );

    if((err.error == QJsonParseError::NoError) && doc.isObject()) {

        return doc.object();
    }

    return QJsonObject();
}
//==============================================================================
QString Telegram::userName()
{
    if(!m_user.firstName.isEmpty()) return m_user.firstName;
    if(!m_user.name.isEmpty()) return m_user.name;
    if(!m_user.lastName.isEmpty()) return m_user.lastName;
    return tr("User");
}
//==============================================================================
bool Telegram::deleteMessage(qint64 message_id)
{
    return deleteMessage(m_chat.id, message_id);
}
//==============================================================================
bool Telegram::deleteMessage(qint64 chat_id, qint64 message_id)
{
    if(chat_id == 0) {

        emit toLog(tr("Не определен chat_id"), Log::LogError);
        return false;
    }

    QString url = telegram_api_url + "bot" + m_token + "/deleteMessage";
    QJsonObject obj;
    obj["chat_id"] = chat_id;
    obj["message_id"] = message_id;

    return sendToTelegram(url, obj);
}
//==============================================================================
bool Telegram::deleteMessage(const QString &user_name, qint64 message_id)
{
    if(user_name.isEmpty()) {

        emit toLog(tr("Не определен user_name"), Log::LogError);
        return false;
    }

    QString url = telegram_api_url + "bot" + m_token + "/deleteMessage";
    QJsonObject obj;
    obj["chat_id"] = "@" + user_name;
    obj["message_id"] = message_id;

    return sendToTelegram(url, obj);
}
//==============================================================================
QString Telegram::getChatTypeText(ChatType chatType)
{
    switch (chatType) {
    case Chat_Private: return "Private";
    case Chat_Group: return "Group";
    case Chat_Supergroup: return "Supergroup";
    case Chat_Channel: return "Channel";
    default:
        break;
    }
    return "Unknown";
}
//==============================================================================
QString Telegram::lastError() const
{
    return m_lastError;
}
//==============================================================================
QString Telegram::token() const
{
    return m_token;
}
//==============================================================================
QJsonObject Telegram::requestObject() const
{
    return m_requestObj;
}
//==============================================================================
Telegram::ChatStruct Telegram::chat() const
{
    return m_chat;
}
//==============================================================================
Telegram::UserStruct Telegram::user() const
{
    return m_user;
}
//==============================================================================
Telegram::MsgStruct Telegram::message() const
{
    return m_msg;
}
//==============================================================================
Telegram::PhotoStruct Telegram::photo() const
{
    return m_photo;
}
//==============================================================================
Telegram::DocumentStruct Telegram::document() const
{
    return m_document;
}
//==============================================================================
Telegram::CallbackStruct Telegram::callback() const
{
    return m_callback;
}
//==============================================================================
qint64 Telegram::update_id() const
{
    return m_update_id;
}
//==============================================================================
bool Telegram::is_callback() const
{
    return !m_callback.id.isEmpty();
}
//==============================================================================
bool Telegram::is_reply() const
{
    return m_is_reply;
}
//==============================================================================
bool Telegram::is_reply_to_me() const
{
    return m_is_reply && (m_reply_user.type == User_Bot) && (m_reply_user.name == m_name);
}
//==============================================================================
bool Telegram::is_chat_title_change() const
{
    return !m_new_chat_title.isNull();
}
//==============================================================================
bool Telegram::is_chat_migrate() const
{
    return (m_migrate_to_chat_id != 0) && (m_migrate_from_chat_id != 0);
}
//==============================================================================
Telegram::UserStruct Telegram::reply_user() const
{
    return m_reply_user;
}
//==============================================================================
Telegram::MsgStruct Telegram::reply_message() const
{
    return m_reply_msg;
}
//==============================================================================
qint64 Telegram::new_chat_id() const
{
    return m_migrate_to_chat_id;
}
//==============================================================================
qint64 Telegram::old_chat_id() const
{
    return m_migrate_from_chat_id;
}
//==============================================================================
QString Telegram::new_chat_title() const
{
    return m_new_chat_title;
}
//==============================================================================

} // namespace
