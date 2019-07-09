#pragma once
#include <QByteArray>
#include <QCryptographicHash>


class Cryptography {

public:
	static QByteArray pbkdf2(QCryptographicHash::Algorithm method, const QByteArray &rPassword, const QByteArray &rSalt, int rounds, int keyLength);
};
