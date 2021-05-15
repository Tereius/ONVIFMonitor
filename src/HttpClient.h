#pragma once
#include "Client.h"
#include "Response.h"
#include <QByteArray>


class HttpClient : public Client {

	Q_OBJECT

 public:
	explicit HttpClient(QSharedPointer<SoapCtx> sharedCtx = QSharedPointer<SoapCtx>::create(), QObject *pParent = nullptr);

	ArbitraryResponse<QByteArray> get(const QUrl &url);

 private:
	Q_DISABLE_COPY(HttpClient);
};
