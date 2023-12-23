#pragma once
#include "Result.h"
#include "RtspMessage.h"
#include "RtspStream.h"
#include <QCryptographicHash>
#include <QString>
#include <QThread>
#include <QUrl>

class QEventLoop;
class QTcpSocket;


class OnvifRtspClient : public QObject {

	Q_OBJECT

 public:
	explicit OnvifRtspClient(const QUrl &rtspUrl, QObject *pParent = nullptr);
	~OnvifRtspClient() override;
	void setConnectTimeout(int timeoutMs);
	void setSendTimeout(int timeoutMs);
	void setReceiveTimeout(int timeoutMs);
	Result hasAudioBackchannel();
	DetailedResult<RtspStream> startAudioBackchannelStream();
	void stop();

 private:
	Q_DISABLE_COPY(OnvifRtspClient);

	class Auth {
	 public:
		// Create Null object
		Auth();
		// From WWW-Authenticate
		static Auth fromResponse(const QString &WWWAuthenticate);
		QString getAuthHeader(const QString &user, const QString &password, const QString &method, const QString &uri) const;
		bool isNull() const;
		int prefer() const;

	 private:
		QString mType; // either "Digest" or "Basic"
		QString mRealm;
		QString mNonce;
		QString mAlgorithmStr;
		QCryptographicHash::Algorithm mAlgorithm;
	};

	Result connectHost(const QString &host, int port);
	Result disconnectHost();
	DetailedResult<RtspMessageResponse> sendRtspMsg(const RtspMessageRequest &msg, bool retry = false);
	// sorted by preferred auth method
	static QList<OnvifRtspClient::Auth> parseAuthFields(const QList<QString> &field);
	static quint16 randomRtpPort();

	int mCseq;
	QUrl mRtspUrl;
	Auth mAuth;
	QString mUser;
	QString mPassword;
	QEventLoop *mpLoop;
	QTcpSocket *mpSock;
	QString mSession;
	int mConnectTimeout;
	int mSendTimeout;
	int mReceiveTimeout;
};
