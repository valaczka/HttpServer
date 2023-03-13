#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <functional>
#include <map>
#include <QString>
#include <QHash>
#include <QtCore/qglobal.h>
#include <QtMath>
#include <zlib.h>

#if defined(HTTPSERVER_LIBRARY)
#  define HTTPSERVER_EXPORT Q_DECL_EXPORT
#else
#  define HTTPSERVER_EXPORT Q_DECL_IMPORT
#endif


enum class HttpStatus
{
	None = 0,

	// 1xx: Informational
	Continue = 100,
	SwitchingProtocols,
	Processing,

	// 2xx: Success
	Ok = 200,
	Created,
	Accepted,
	NonAuthoritativeInformation,
	NoContent,
	ResetContent,
	PartialContent,
	MultiStatus,
	AlreadyReported,
	IMUsed = 226,

	// 3xx: Redirection
	MultipleChoices = 300,
	MovedPermanently,
	Found,
	SeeOther,
	NotModified,
	UseProxy,
	// 306: not used, was proposed as "Switch Proxy" but never standardized
	TemporaryRedirect = 307,
	PermanentRedirect,

	// 4xx: Client Error
	BadRequest = 400,
	Unauthorized,
	PaymentRequired,
	Forbidden,
	NotFound,
	MethodNotAllowed,
	NotAcceptable,
	ProxyAuthenticationRequired,
	RequestTimeout,
	Conflict,
	Gone,
	LengthRequired,
	PreconditionFailed,
	PayloadTooLarge,
	UriTooLong,
	UnsupportedMediaType,
	RequestRangeNotSatisfiable,
	ExpectationFailed,
	ImATeapot,
	MisdirectedRequest = 421,
	UnprocessableEntity,
	Locked,
	FailedDependency,
	UpgradeRequired = 426,
	PreconditionRequired = 428,
	TooManyRequests,
	RequestHeaderFieldsTooLarge = 431,
	UnavailableForLegalReasons = 451,

	// 5xx: Server Error
	InternalServerError = 500,
	NotImplemented,
	BadGateway,
	ServiceUnavailable,
	GatewayTimeout,
	HttpVersionNotSupported,
	VariantAlsoNegotiates,
	InsufficientStorage,
	LoopDetected,
	NotExtended = 510,
	NetworkAuthenticationRequired,
	NetworkConnectTimeoutError = 599,
};

struct QStringCaseSensitiveHash
{
	size_t operator()(const QString &str) const
	{
		static const unsigned int seed = (unsigned int)qGlobalQHashSeed();

		// Case-sensitive QString hash
		return qHash(str, seed);
	}
};

struct QStringCaseSensitiveEqual
{
	bool operator()(const QString &str1, const QString &str2) const
	{
		return str1.compare(str2, Qt::CaseSensitive) == 0;
	}
};

struct QStringCaseInsensitiveHash
{
	size_t operator()(const QString &str) const
	{
		static const unsigned int seed = (unsigned int)qGlobalQHashSeed();

		// Case-insensitive QString hash
		return qHash(str.toLower(), seed);
	}
};

struct QStringCaseInSensitiveEqual
{
	bool operator()(const QString &str1, const QString &str2) const
	{
		return str1.compare(str2, Qt::CaseInsensitive) == 0;
	}
};

namespace std
{
	/*// Default hash and comparator for QString is case-sensitive
	template<> struct hash<QString>
	{
		size_t operator()(const QString &str) const
		{
			static const unsigned int seed = (unsigned int)qGlobalQHashSeed();

			return qHash(str, seed);
		}
	};*/

	template<> struct equal_to<QString>
	{
		bool operator()(const QString &str1, const QString &str2) const
		{
			return str1.compare(str2, Qt::CaseSensitive) == 0;
		}
	};
}

HTTPSERVER_EXPORT QString getHttpStatusStr(HttpStatus status);

QByteArray gzipCompress(QByteArray &data, int compressionLevel = Z_DEFAULT_COMPRESSION);
QByteArray gzipUncompress(QByteArray &data);

#endif // UTIL_H
