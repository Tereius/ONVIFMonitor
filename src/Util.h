#pragma once
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
};
