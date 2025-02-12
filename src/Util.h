#pragma once
#include "SecretsManager.h"
#include <QUrl>


class Util {

public:
	static int getDefaultPort(const QUrl &url) {

		if(url.scheme().compare("http") == 0)
			return url.port(80);
		else if(url.scheme().compare("https") == 0)
			return url.port(443);
		return url.port();
	}

	static QString mergeUsernamePassword(const QString &user, const QString &password) {

		auto combined = QByteArray(QString("%1%2").arg(user, password).toUtf8());
		combined.prepend(1, '\0');
		quint8 len = user.toUtf8().length();
		memcpy(combined.data(), &len, sizeof len);
		return QString::fromUtf8(combined);
	}

	static QPair<QString, QString> unmergeUsernamePassword(const QString &combined) {

		auto data = combined.toUtf8();
		if(data.length() > 0) {
			quint8 len = 0;
			memcpy(&len, data.data(), sizeof len);
			return qMakePair(QString::fromUtf8(data.mid(1, len)), QString::fromUtf8(data.mid(len + 1)));
		}
		return qMakePair(QString(), QString());
	}
};
