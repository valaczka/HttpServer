#include "util.h"


/* Status Codes */

static const QHash<int, QString> httpStatusStrs {
	{100, "Continue"},
	{101, "Switching Protocols"},
	{102, "Processing"},

	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{207, "Multi-Status"},
	{208, "Already Reported"},
	{226, "IM Used"},

	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},

	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{421, "Misdirected Request"},
	{422, "Unprocessable Entity"},
	{423, "Locked"},
	{424, "Failed Dependency"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},

	{500, "Internal Server Error"},
	{501, "Not implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"}
};


QString getHttpStatusStr(HttpStatus status)
{
	auto it = httpStatusStrs.find(static_cast<int>(status));
	if (it == httpStatusStrs.end())
		return "";

	return it.value();
}

QByteArray gzipCompress(QByteArray &data, int compressionLevel)
{
	QByteArray ret;

	if (data.size() == 0)
		return QByteArray();

	if (compressionLevel < -1 || compressionLevel > 9)
		compressionLevel = -1;

	// Set chunk size based on size of data, but clamp between 1024 bytes to 128kB
	// Since we're compressing, output should be smaller than the input
	const int chunkSize = std::min(std::max((int)qNextPowerOfTwo(data.size()), 1024), 128 * 1024);

	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;

	// Initialize deflate operation
	// Use default memory level (8)
	// Use default window bits (15) but add 16 to make output gzip instead of zlib format
	int err = deflateInit2(&stream, compressionLevel, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
	if (err != Z_OK)
		return QByteArray();

	// Point stream to input data
	stream.avail_in = (unsigned int)data.size();
	stream.next_in = (unsigned char *)data.data();

	// In 99% of cases, this loop should only run once since the output data will be less than the input data
	do
	{
		// Allocate one more chunk size to the byte array
		ret.resize(ret.size() + chunkSize);

		// Point to output data
		stream.avail_out = (unsigned long)ret.size() - stream.total_out;
		stream.next_out = (unsigned char *)&ret.data()[stream.total_out];

		err = deflate(&stream, Z_FINISH);
	} while (err == Z_OK);

	if (err != Z_STREAM_END)
		return QByteArray();

	ret.resize((int)stream.total_out);
	deflateEnd(&stream);
	return ret;
}

QByteArray gzipUncompress(QByteArray &data)
{
	QByteArray ret;

	if (data.size() == 0)
		return QByteArray();

	// Set chunk size to be twice the size of the data to the nearest power of two
	// Clamped between 1024 bytes and 128kB to prevent consuming too little or too much memory
	// We use twice the size of the data because the average file will have a compression ratio of around 2:1
	// This gives a good starting point for decompressing the file in as few passes as possible
	const int chunkSize = std::min(std::max((int)qNextPowerOfTwo(data.size() * 2), 1024), 128 * 1024);

	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;

	// Initialize inflate operation
	// Use default window bits (15) but add 16 to make output gzip instead of zlib format
	int err = inflateInit2(&stream, 15 + 16);
	if (err != Z_OK)
		return QByteArray();

	// Point stream to input data
	stream.avail_in = (unsigned int)data.size();
	stream.next_in = (unsigned char *)data.data();

	// If the compression ratio is less than 2:1, then this loop will only run once
	do
	{
		// Allocate one more chunk size to the byte array
		ret.resize(ret.size() + chunkSize);

		// Point to output data
		stream.avail_out = (unsigned long)ret.size() - stream.total_out;
		stream.next_out = (unsigned char *)&ret.data()[stream.total_out];

		err = inflate(&stream, Z_NO_FLUSH);
	} while (err == Z_OK || err == Z_BUF_ERROR); // Check for Z_BUF_ERROR too because that indicates buffer not big enough

	if (err != Z_STREAM_END)
		return QByteArray();

	ret.resize((int)stream.total_out);
	inflateEnd(&stream);
	return ret;
}
