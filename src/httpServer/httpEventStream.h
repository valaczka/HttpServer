/*
 * ---- Call of Suli ----
 *
 * httpEventStream.h
 *
 * Created on: 2023. 03. 25.
 *     Author: Valaczka János Pál <valaczka.janos@piarista.hu>
 *
 * HttpEventStream
 *
 *  This file is part of Call of Suli.
 *
 *  Call of Suli is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HTTPEVENTSTREAM_H
#define HTTPEVENTSTREAM_H

#include <QObject>
#include "qtcpsocket.h"
#include "qtimer.h"
#include "util.h"

class HttpConnection;

/**
 * @brief The HttpEventStream class
 */

class HTTPSERVER_EXPORT HttpEventStream : public QObject
{
	Q_OBJECT

public:
	explicit HttpEventStream(HttpConnection *httpConnection);
	virtual ~HttpEventStream();

	HttpConnection *httpConnection() const;

	bool headerSent() const;
	void sendHeader();

	bool write(const QByteArray &event, const QByteArray &data);
	bool writeChunk();

public slots:
	bool ping();

signals:

protected:
	HttpConnection *const m_httpConnection;
	QTcpSocket *m_socket;
	int m_writeIndex = -1;
	QByteArray m_buffer;
	bool m_headerSent = false;
	QTimer m_pingTimer;

};

#endif // HTTPEVENTSTREAM_H
