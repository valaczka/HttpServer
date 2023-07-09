/*
 * ---- Call of Suli ----
 *
 * httpEventStream.cpp
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

#include "httpEventStream.h"
#include "httpConnection.h"


HttpEventStream::HttpEventStream(HttpConnection *httpConnection)
	: QObject{httpConnection}
	, m_httpConnection(httpConnection)
	, m_socket(nullptr)
{
	Q_ASSERT(m_httpConnection);

	m_socket = m_httpConnection->socket;

	Q_ASSERT(m_socket);

	connect(&m_pingTimer, &QTimer::timeout, this, &HttpEventStream::ping);
	m_pingTimer.start(30*1000);
}


/**
 * @brief HttpEventStream::~HttpEventStream
 */

HttpEventStream::~HttpEventStream()
{
	m_pingTimer.stop();
}


HttpConnection *HttpEventStream::httpConnection() const
{
	return m_httpConnection;
}


/**
 * @brief HttpEventStream::write
 * @param event
 * @param data
 * @return
 */

bool HttpEventStream::write(const QByteArray &event, const QByteArray &data)
{
	if (!m_headerSent)
		sendHeader();

	m_buffer += QByteArrayLiteral("event: ")+event+QByteArrayLiteral("\r\n");
	m_buffer += QByteArrayLiteral("data: ")+data+QByteArrayLiteral("\r\n\r\n");

	return writeChunk();
}


/**
 * @brief HttpEventStream::ping
 * @return
 */

bool HttpEventStream::ping()
{
	m_buffer += QByteArrayLiteral(": ping\r\n\r\n");

	return writeChunk();
}


/**
 * @brief HttpEventStream::writeChunk
 * @return
 */

bool HttpEventStream::writeChunk()
{
	if (m_buffer.isEmpty() || m_writeIndex >= m_buffer.size() - 1) {
		m_buffer.clear();
		m_writeIndex = 0;
		return true;
	}

	if (!m_socket)
		return true;

	int bytesWritten = m_socket->write(m_buffer.mid(m_writeIndex));
	if (bytesWritten == -1)
	{
		// Force close the socket and say we're done
		m_socket->close();
		return true;
	}

	// Increment the bytes written, if we wrote the entire buffer, return true, otherwise return false
	m_writeIndex += bytesWritten;
	if (m_writeIndex >= m_buffer.size() - 1) {
		m_buffer.clear();
		m_writeIndex = 0;
		return true;
	}

	return false;
}



/**
 * @brief HttpEventStream::headerSent
 * @return
 */

bool HttpEventStream::headerSent() const
{
	return m_headerSent;
}


/**
 * @brief HttpEventStream::sendHeader
 */

void HttpEventStream::sendHeader()
{
	if (m_headerSent)
		return;

	m_writeIndex = 0;
	m_buffer.clear();

	m_buffer += QByteArrayLiteral("HTTP/1.1 200 OK\r\n");

	struct Header {
		const char *key;
		const char *value;

		Header(const char *k, const char *v) : key(k), value(v) {}
	};

	QVector<Header> headers;
	headers.append(Header("Content-Type", "text/event-stream"));
	headers.append(Header("Connection", "keep-alive"));
	headers.append(Header("Cache-Control", "no-cache"));

	foreach (const Header &h, headers) {
		m_buffer += h.key;
		m_buffer += ": ";
		m_buffer += h.value;
		m_buffer += "\r\n";
	}

	m_buffer += "\r\n";

	m_headerSent = true;

	writeChunk();

}
