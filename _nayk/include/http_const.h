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
#ifndef HTTP_CONST_H
#define HTTP_CONST_H

#include <QString>

namespace nayk { //=============================================================

const QString contentTypeImageGIF             = "image/gif";
const QString contentTypeImageJPEG            = "image/jpeg";
const QString contentTypeImageJPG             = "image/jpg";
const QString contentTypeImagePNG             = "image/png";
const QString contentTypeImageSVG             = "image/svg+xml";
const QString contentTypeImageTIFF            = "image/tiff";
const QString contentTypeFont                 = "application/font";
const QString contentTypeFontEOT              = "application/vnd.ms-fontobject";
const QString contentTypeFontOTF              = "font/opentype";
const QString contentTypeFontTTF              = "font/ttf";
const QString contentTypeFontWOFF             = "font/woff";
const QString contentTypeScriptECMAS          = "text/ecmascript";
const QString contentTypeScriptVB             = "text/vbscript";
const QString contentTypeCSS                  = "text/css";
const QString contentTypeHTML                 = "text/html";
const QString contentTypeJS                   = "text/javascript";
const QString contentTypeJSON                 = "application/json";
const QString contentTypeText                 = "text/plain";
const QString contentTypeXML                  = "text/xml";
const QString contentTypeWWWForm              = "application/x-www-form-urlencoded";
const QString contentTypeMultipartForm        = "multipart/form-data";
const QString contentTypeBinary               = "application/octet-stream";
// Server headers:
const QString serverHeaderComSpec             = "COMSPEC";
const QString serverHeaderDocumentRoot        = "DOCUMENT_ROOT";
const QString serverHeaderGatewayInterface    = "GATEWAY_INTERFACE";
const QString serverHeaderHttpAccept          = "HTTP_ACCEPT";
const QString serverHeaderHttpAcceptEncoding  = "HTTP_ACCEPT_ENCODING";
const QString serverHeaderHttpAcceptLanguage  = "HTTP_ACCEPT_LANGUAGE";
const QString serverHeaderHttpConnection      = "HTTP_CONNECTION";
const QString serverHeaderHttpCookie          = "HTTP_COOKIE";
const QString serverHeaderHttpHost            = "HTTP_HOST";
const QString serverHeaderHttpUserAgent       = "HTTP_USER_AGENT";
const QString serverHeaderHttps               = "HTTPS";
const QString serverHeaderPath                = "PATH";
const QString serverHeaderQueryString         = "QUERY_STRING";
const QString serverHeaderRemoteAddress       = "REMOTE_ADDR";
const QString serverHeaderRemotePort          = "REMOTE_PORT";
const QString serverHeaderRequestMethod       = "REQUEST_METHOD";
const QString serverHeaderRequestScheme       = "REQUEST_SCHEME";
const QString serverHeaderRequestUri          = "REQUEST_URI";
const QString serverHeaderScriptFilename      = "SCRIPT_FILENAME";
const QString serverHeaderScriptName          = "SCRIPT_NAME";
const QString serverHeaderServerAddress       = "SERVER_ADDR";
const QString serverHeaderServerAdministrator = "SERVER_ADMIN";
const QString serverHeaderServerName          = "SERVER_NAME";
const QString serverHeaderServerPort          = "SERVER_PORT";
const QString serverHeaderServerProtocol      = "SERVER_PROTOCOL";
const QString serverHeaderServerSignature     = "SERVER_SIGNATURE";
const QString serverHeaderServerSoftware      = "SERVER_SOFTWARE";
const QString serverHeaderContentType         = "CONTENT_TYPE";
const QString serverHeaderContentLength       = "CONTENT_LENGTH";
const QString serverHeaderContextPrefix       = "CONTEXT_PREFIX";
const QString serverHeaderOrigin              = "ORIGIN";
//
const QString methodDelete                    = "DELETE";
const QString methodGet                       = "GET";
const QString methodHead                      = "HEAD";
const QString methodPost                      = "POST";
const QString methodPut                       = "PUT";
// Response headers:
const QString headerContentLength             = "Content-Length";
const QString headerContentType               = "Content-Type";
const QString headerCookie                    = "Set-Cookie";

} // namespace nayk //==========================================================
#endif // HTTP_CONST_H
