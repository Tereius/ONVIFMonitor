#include "HttpClient.h"
#include <utility>


HttpClient::HttpClient(QSharedPointer<SoapCtx> sharedCtx /*= QSharedPointer<SoapCtx>::create()*/, QObject *pParent /*= nullptr*/) :
 Client(QUrl(), std::move(sharedCtx), pParent) {}

ArbitraryResponse<QByteArray> HttpClient::get(const QUrl &url) {

	_tds__GetServicesResponse responseObject;
	char *data = nullptr;
	size_t dataSize = -1;
	auto ret = SOAP_OK;
	auto pSoap = AcquireCtx();
	do {
		if(soap_GET(pSoap, qPrintable(url.toString()), nullptr) || soap_begin_recv(pSoap) ||
		   (data = soap_http_get_body(pSoap, &dataSize)) != nullptr || soap_end_recv(pSoap)) {
			// error
		}
		soap_closesock(pSoap);
	} while(Retry(pSoap));
	auto response = ArbitraryResponse<QByteArray>::Builder();
	response.From(GetCtx(), QByteArray(data, dataSize));
	ReleaseCtx(pSoap);
	return response.Build();
}
