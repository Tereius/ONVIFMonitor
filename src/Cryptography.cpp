#include "Cryptography.h"
#include <QMessageAuthenticationCode>


QByteArray Cryptography::pbkdf2(QCryptographicHash::Algorithm method, const QByteArray &rPassword, const QByteArray &rSalt, int rounds, int keyLength) {

	if(rPassword.isEmpty() == true || rSalt.isEmpty() == true) return QByteArray();

	QByteArray key;
	if(rounds < 1 || keyLength < 1)
		return QByteArray();

	if(rSalt.size() == 0 || rSalt.size() > std::numeric_limits<int>::max() - 4)
		return QByteArray();

	QByteArray asalt = rSalt;
	asalt.resize(rSalt.size() + 4);

	for(int count = 1, remainingBytes = keyLength; remainingBytes > 0; ++count) {
		asalt[rSalt.size() + 0] = static_cast<char>((count >> 24) & 0xff);
		asalt[rSalt.size() + 1] = static_cast<char>((count >> 16) & 0xff);
		asalt[rSalt.size() + 2] = static_cast<char>((count >> 8) & 0xff);
		asalt[rSalt.size() + 3] = static_cast<char>(count & 0xff);
		QByteArray d1 = QMessageAuthenticationCode::hash(asalt, rPassword, method);
		QByteArray obuf = d1;

		for(int i = 1; i < rounds; ++i) {
			d1 = QMessageAuthenticationCode::hash(d1, rPassword, method);
			for(int j = 0; j < obuf.size(); ++j)
				obuf[j] = obuf[j] ^ d1[j];
		}
		key = key.append(obuf);
		remainingBytes -= obuf.size();
		d1.fill('\0');
		obuf.fill('\0');
	}
	asalt.fill('\0');
	return key.mid(0, keyLength);
}
