#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "httpConnection.h"
#include "httpServerConfig.h"
#include "httpRequestHandler.h"
#include "util.h"

#include <QBasicTimer>
#include <QSslKey>
#include <QTcpServer>


// HTTP server is HTTP/1.1 compliant and is based on RFC7230 series. This specification was created in June 2014 and obsoletes RFC2616, RFC2145
class HTTPSERVER_EXPORT HttpServer : public QTcpServer
{
	Q_OBJECT

private:
	HttpServerConfig config;
	HttpRequestHandler *requestHandler;

	QSslConfiguration *sslConfig;
	std::vector<HttpConnection *> connections;

	void loadSslConfig();

public:
	HttpServer(const HttpServerConfig &config, HttpRequestHandler *requestHandler, QObject *parent = nullptr);
	~HttpServer();

	bool listen();
	void close();

	QSslConfiguration *getSslConfig() const;

protected:
	void incomingConnection(qintptr socketDescriptor);

private slots:
	void connectionDisconnected();

signals:
	void handleConnection(int socketDescriptor);

};

#endif // HTTPSERVER_H
