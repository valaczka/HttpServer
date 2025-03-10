#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "httpCookie.h"
#include "httpServerConfig.h"
#include "util.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QMimeDatabase>
#include <QString>
#include <QTcpSocket>
#include <functional>
#include <unordered_map>


// Forward declaration
class HttpRequest;

class HTTPSERVER_EXPORT HttpResponse : public QObject
{
    Q_OBJECT

private:
    // Used for determining MIME type when sending files
    static QMimeDatabase mimeDatabase;

    HttpServerConfig *config;

    // The HTTP version is currently fixed at 1.1 since HTTP/2 is not supported
    QString version_ = "HTTP/1.1";
    HttpStatus status_;

    std::unordered_map<QString, QString, QStringCaseInsensitiveHash, QStringCaseInSensitiveEqual> headers;
    // Note: Cookies ARE case sensitive, headers are not
    std::unordered_map<QString, HttpCookie> cookies;

    QByteArray body_;

    int writeIndex;
    QByteArray buffer;

public:
    HttpResponse(HttpServerConfig *config, QObject *parent = nullptr);

    bool isValid() const;
    bool isSending() const;

    QString version() const;
    HttpStatus status() const;
    QByteArray body() const;

    bool header(QString key, QString *value) const;
    bool cookie(QString name, HttpCookie *cookie) const;

    // Make sure you only call this once per repsonse!
    // Or, better, just rely on setStatus to do it for you
    void setFinished();

    void setStatus(HttpStatus status, bool finished_ = true);
    void setStatus(HttpStatus status, QByteArray body, QString contentType = "", bool finished_ = true);
    void setStatus(HttpStatus status, QJsonDocument body, bool finished_ = true);
    void setStatus(HttpStatus status, QString body, QString mimeType, bool finished_ = true);
    void setBody(QByteArray body);

    void setError(HttpStatus status, QString errorMessage = "", bool closeConnection = false, bool finished_ = true);

    void redirect(QUrl url, bool permanent = false);
    void redirect(QString url, bool permanent = false);
    void compressBody(int compressionLevel = Z_DEFAULT_COMPRESSION);

    void sendFile(QString filename, QString mimeType = "", QString charset = "", int len = -1, int compressionLevel = -2, QString attachmentFilename = "", int cacheTime = 0);
    void sendFile(QIODevice *device, QString mimeType = "", QString charset = "", int len = -1, int compressionLevel = -2, QString attachmentFilename = "", int cacheTime = 0);

    void setCookie(HttpCookie &cookie);

    void setHeader(QString name, QString value, bool encode = false);
    void setHeader(QString name, QDateTime value);
    void setHeader(QString name, int value);

    void setupFromRequest(HttpRequest *request);
    void prepareToSend();
    bool writeChunk(QTcpSocket *socket);

signals:
    // When this signal is called, if asynchronous work is being done, the user MUST not use the response pointer again because it will be deleted
    void cancelled();
    void finished();
};

#endif // HTTP_RESPONSE_H
