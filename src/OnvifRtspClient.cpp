#include "OnvifRtspClient.h"
#include "App.h"
#include "RtspMessage.h"
#include "SessionDescription.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QScopeGuard>
#include <QTcpSocket>
#include <QTimer>
#include <algorithm>


#define ONVIF_AUDIO_BACKCHANNEL_TAG "www.onvif.org/ver20/backchannel"
#define DEFAULT_RTSP_PORT 554
#define MIN_DYNAMIC_PORT 49152
#define MAX_DYNAMIC_PORT 60999


OnvifRtspClient::OnvifRtspClient(const QUrl &rtspUrl, QObject *pParent /*= nullptr*/) :
 QObject(pParent),
 mCseq(1),
 mRtspUrl(rtspUrl),
 mAuth(),
 mUser(rtspUrl.userName()),
 mPassword(rtspUrl.password()),
 mpLoop(new QEventLoop(this)),
 mpSock(new QTcpSocket(this)),
 mSession(),
 mConnectTimeout(10000),
 mSendTimeout(10000),
 mReceiveTimeout(10000) {
	mRtspUrl.setUserName({});
	mRtspUrl.setPassword({});
}

OnvifRtspClient::~OnvifRtspClient() {

	stop();
}

void OnvifRtspClient::setConnectTimeout(int timeoutMs) {}
void OnvifRtspClient::setSendTimeout(int timeoutMs) {}
void OnvifRtspClient::setReceiveTimeout(int timeoutMs) {}

DetailedResult<MediaDescription> OnvifRtspClient::hasAudioBackchannel() {

	mCseq = 1;
	if(auto connectResult = connectHost(mRtspUrl.host(), mRtspUrl.port(DEFAULT_RTSP_PORT))) {
		auto deferDisconnect = qScopeGuard([this] { disconnectHost(); });
		auto describeRequest = RtspMessageRequest("DESCRIBE", mRtspUrl.toString());
		describeRequest.addField("Require", ONVIF_AUDIO_BACKCHANNEL_TAG);
		if(auto describeResult = sendRtspMsg(describeRequest)) {
			auto response = describeResult.GetResultObject();
			const auto sdp = SessionDescription(response.getPayload());
			const auto sendonlyMediaDescription = sdp.getMediaDescriptionWithAttribute("sendonly");
			if(!sendonlyMediaDescription.isNull()) {
				auto control = sendonlyMediaDescription.getAttributeValueFirst("control");
				if(!control.isNull()) {
					auto stream = RtspStream(sendonlyMediaDescription);
					if(!stream.getMediaDescription().getAttributeValues("rtpmap").isEmpty()) {
						return DetailedResult<MediaDescription>(sendonlyMediaDescription);
					}
				}
			}
		}
	}
	return DetailedResult<MediaDescription>(Result::FAULT, "");
}

// ffmpeg -re -i '/home/bjoern/Music/Palladium ft. Bianca Varela - Adrift (Radio Edit).m4a' -filter:a "volume=0.03" -ac
// 1 -ar 8000 -ab 64k -acodec pcm_mulaw -f rtp "rtp://192.168.0.38:8336?localrtpport=55000"

// ffmpeg -re -i '/home/bjoern/Music/Palladium ft. Bianca Varela - Adrift (Radio Edit).m4a' -filter:a
// "aresample=8000,asetnsamples=n=160:p=1,volume=0.03" -ac 1 -ar 8000 -ab 64k -acodec pcm_mulaw -f rtp
// "rtp://192.168.0.38:8336?localrtpport=55000"

// PATH/TO/FFMPEG -re -i PATH/TO/MP3FILE -filter_complex 'aresample=8000,asetnsamples=n=160' -acodec pcm_mulaw -ac 1 -f
// rtp udp://239.255.255.244:5555

// PATH/TO/FFMPEG -re -i PATH/TO/MP3FILE -filter_complex 'aresample=16000,asetnsamples=n=160' -acodec g722 -ac 1 -f rtp
// udp://239.255.255.244:5555

DetailedResult<RtspStream> OnvifRtspClient::startAudioBackchannelStream() {

	mCseq = 1;
	stop();
	qDebug() << "About to start RTSP stream";
	if(auto connectResult = connectHost(mRtspUrl.host(), mRtspUrl.port(DEFAULT_RTSP_PORT))) {
		auto deferDisconnect = qScopeGuard([this] { disconnectHost(); });
		auto describeRequest = RtspMessageRequest("DESCRIBE", mRtspUrl.toString());
		describeRequest.addField("Require", ONVIF_AUDIO_BACKCHANNEL_TAG);
		if(auto describeResult = sendRtspMsg(describeRequest)) {
			auto response = describeResult.GetResultObject();
			const auto sdp = SessionDescription(response.getPayload());
			const auto sendonlyMediaDescription = sdp.getMediaDescriptionWithAttribute("sendonly");
			if(!sendonlyMediaDescription.isNull()) {
				auto control = sendonlyMediaDescription.getAttributeValueFirst("control");
				if(!control.isNull()) {
					auto stream = RtspStream(sendonlyMediaDescription);
					auto rtpPort = OnvifRtspClient::randomRtpPort();
					auto rtcpPort = rtpPort + 1;
					auto setupRequest = RtspMessageRequest("SETUP", control);
					setupRequest.addField("Transport", QString("RTP/AVP;unicast;client_port=%1-%2").arg(rtpPort).arg(rtcpPort));
					setupRequest.addField("Require", ONVIF_AUDIO_BACKCHANNEL_TAG);
					if(auto setupResult = sendRtspMsg(setupRequest)) {
						auto playRequest = RtspMessageRequest("PLAY", mRtspUrl.toString());
						auto session = setupResult.GetResultObject().getFirstField("Session");
						const auto sessionSplit = session.split(";");
						if(!sessionSplit.isEmpty()) {
							session = sessionSplit.first();
							if(!session.isEmpty()) {
								int serverRtpPort = 0;
								int serverRtcpPort = 0;
								auto serverPortString = setupResult.GetResultObject().getFirstFieldValue("Transport", "server_port").split("-");
								if(serverPortString.count() == 1) {
									serverRtpPort = serverPortString.at(0).toInt();
								} else if(serverPortString.count() > 1) {
									serverRtpPort = serverPortString.at(0).toInt();
									serverRtcpPort = serverPortString.at(1).toInt();
								}
								stream.setSsrc(setupResult.GetResultObject().getFirstFieldValue("Transport", "ssrc").toInt(nullptr, 16));
								stream.setServerRtpPort(serverRtpPort);
								stream.setServerRtcpPort(serverRtcpPort);
								stream.setClientRtpPort(rtpPort);
								stream.setClientRtcpPort(rtcpPort);
								playRequest.addField("Range", "npt=now-");
								playRequest.addField("Session", session);
								playRequest.addField("Require", ONVIF_AUDIO_BACKCHANNEL_TAG);
								if(auto playResult = sendRtspMsg(playRequest)) {
									qInfo() << "Successfully started RTSP stream with session" << session;
									deferDisconnect.dismiss(); // somehow we have to keep the tcp connection open
									mSession = session;
									return DetailedResult<RtspStream>(stream);
								} else {
									qWarning() << "RTSP PLAY failed";
									return {playResult, {}};
								}
							} else {
								qWarning() << "Missing RTSP session value";
								return {Result::FAULT, tr("The device does not provide a audio backchannel session")};
							}
						} else {
							qWarning() << "Missing RTSP session key";
							return {Result::FAULT, tr("The device does not provide a audio backchannel session")};
						}
					} else {
						qWarning() << "RTSP SETUP failed";
						return {setupResult, {}};
					}
				} else {
					qWarning() << "Missing control entry in sdp - no backchannel support";
					return {Result::FAULT, tr("The device does not provide audio backchannel support")};
				}
			} else {
				qWarning() << "Missing sendonly entry in sdp - no backchannel support";
				return {Result::FAULT, tr("The device does not provide audio backchannel support")};
			}
		} else {
			qWarning() << "RTSP DESCRIBE failed";
			return {describeResult, {}};
		}
	} else {
		return {connectResult, {}};
	}
}

void OnvifRtspClient::stop() {

	if(!mSession.isEmpty()) {
		qDebug() << "About to stop RTSP stream with session" << mSession;
		if(auto connectResult = connectHost(mRtspUrl.host(), mRtspUrl.port(DEFAULT_RTSP_PORT))) {
			auto deferDisconnect = qScopeGuard([this] { disconnectHost(); });
			auto teardownRequest = RtspMessageRequest("TEARDOWN", mRtspUrl.toString());
			teardownRequest.addField("Session", mSession);
			teardownRequest.addField("Require", ONVIF_AUDIO_BACKCHANNEL_TAG);
			if(auto teardownResult = sendRtspMsg(teardownRequest)) {
				qDebug() << "Stopped RTSP stream with session" << mSession;
			} else {
				qWarning() << teardownResult;
			}
			mSession = {};
		} else {
			qWarning() << connectResult;
		}
	}
}

Result OnvifRtspClient::connectHost(const QString &host, int port) {

	if(mpSock->state() != QAbstractSocket::ConnectedState) {
		qDebug() << "About to connect host" << host + ":" + QString::number(port);
		Result result;
		auto connected = false;
		connect(
		 mpSock, &QTcpSocket::connected, mpLoop,
		 [&]() {
			 qDebug() << "Successfully connected host" << host + ":" + QString::number(port);
			 connected = true;
			 mpLoop->quit();
		 },
		 Qt::QueuedConnection);
		connect(
		 mpSock, &QTcpSocket::errorOccurred, mpLoop,
		 [&](QAbstractSocket::SocketError err) {
			 Q_UNUSED(err)
			 qWarning() << "Connecting host failed" << host + ":" + QString::number(port) << mpSock->errorString();
			 result = {Result::FAULT, tr("Connecting host failed: %1").arg(mpSock->errorString())};
			 mpLoop->quit();
		 },
		 Qt::QueuedConnection);
		QTimer timer;
		timer.setSingleShot(true);
		connect(
		 &timer, &QTimer::timeout, mpLoop,
		 [&]() {
			 qWarning() << "Connect timeout" << host + ":" + QString::number(port);
			 result = {Result::FAULT, tr("Connect timeout occurred")};
			 mpLoop->quit();
		 },
		 Qt::QueuedConnection);
		timer.start(mConnectTimeout);
		mpSock->connectToHost(host, port);
		mpLoop->exec(); // quits as soon as rtsp message was complete read, error or timeout occurred
		mpSock->disconnect(mpLoop);
		timer.disconnect(mpLoop);
		timer.stop();
		if(connected && result) {
			return Result::OK;
		}
		return result;
	}
	return Result::OK;
}

Result OnvifRtspClient::disconnectHost() {

	if(mpSock->state() == QAbstractSocket::ConnectedState) {
		const auto tmpHost = mpSock->peerAddress();
		qDebug() << "About to disconnect host" << tmpHost.toString();
		Result result;
		auto disconnected = false;
		connect(
		 mpSock, &QTcpSocket::disconnected, mpLoop,
		 [&]() {
			 qDebug() << "Successfully disconnected host" << tmpHost.toString();
			 disconnected = true;
			 mpLoop->quit();
		 },
		 Qt::QueuedConnection);
		QTimer timer;
		timer.setSingleShot(true);
		connect(
		 &timer, &QTimer::timeout, mpLoop,
		 [&]() {
			 qWarning() << "Disconnect timeout" << tmpHost.toString();
			 result = {Result::FAULT, tr("Disconnect timeout occurred")};
			 mpLoop->quit();
		 },
		 Qt::QueuedConnection);
		timer.start(1000);
		mpSock->disconnectFromHost();
		mpLoop->exec(); // quits as soon as rtsp message was complete read, error or timeout occurred
		mpSock->disconnect(mpLoop);
		timer.disconnect(mpLoop);
		timer.stop();
		if(disconnected && result) {
			return Result::OK;
		}
		return result;
	}
	return Result::OK;
}

DetailedResult<RtspMessageResponse> OnvifRtspClient::sendRtspMsg(const RtspMessageRequest &msg, bool retry /*= false*/) {

	qDebug() << "About to send RTSP message" << msg.getMethod() + " " + msg.getUrl() << mpSock->peerAddress().toString()
	         << (retry ? "again" : "");

	auto result = DetailedResult<RtspMessageResponse>();
	auto messageFinished = false;
	auto message = QByteArray();
	auto payload = QByteArray();
	auto payloadSize = 0;
	connect(
	 mpSock, &QTcpSocket::errorOccurred, mpLoop,
	 [&](QAbstractSocket::SocketError err) {
		 Q_UNUSED(err)
		 qWarning() << "Sending RTSP message failed" << mpSock->peerAddress().toString() << mpSock->errorString();
		 result = DetailedResult<RtspMessageResponse>(Result::FAULT, tr("Sending RTSP message failed: %1").arg(mpSock->errorString()));
		 mpLoop->quit();
	 },
	 Qt::QueuedConnection);
	connect(
	 mpSock, &QTcpSocket::readyRead, mpLoop,
	 [&]() {
		 while(mpSock->canReadLine()) {
			 auto line = mpSock->readLine();
			 if(line.startsWith("Content-Length:")) {
				 const auto contenLengthStr = line.last(line.size() - 15).trimmed();
				 payloadSize = QString::fromUtf8(contenLengthStr).toInt();
			 }
			 if(!messageFinished) {
				 message.swap(message.append(line));
			 } else {
				 payload.swap(payload.append(line));
			 }
			 if(line.startsWith("\r\n") || line.startsWith("\n") || line.startsWith("\r")) {
				 messageFinished = true;
			 }
		 }
		 if(messageFinished && payloadSize == payload.size()) {
			 mpLoop->quit();
		 }
	 },
	 Qt::QueuedConnection);
	QTimer receiveTimer;
	receiveTimer.setSingleShot(true);
	connect(
	 &receiveTimer, &QTimer::timeout, mpLoop,
	 [&]() {
		 qWarning() << "Read timeout occurred" << mpSock->peerAddress().toString();
		 result = DetailedResult<RtspMessageResponse>(Result::FAULT, tr("Read timeout occurred"));
		 mpLoop->quit();
	 },
	 Qt::QueuedConnection);
	QTimer sendTimer;
	sendTimer.setSingleShot(true);
	connect(
	 &sendTimer, &QTimer::timeout, mpLoop,
	 [&]() {
		 qWarning() << "Send timeout occurred" << mpSock->peerAddress().toString();
		 result = DetailedResult<RtspMessageResponse>(Result::FAULT, tr("Send timeout occurred"));
		 mpLoop->quit();
	 },
	 Qt::QueuedConnection);
	auto data = msg.serialize(mCseq++, mAuth.getAuthHeader(mUser, mPassword, msg.getMethod(), msg.getUrl()));
	qDebug() << "└"
	         << QString::fromUtf8(msg.serialize(mCseq, mAuth.getAuthHeader("***********", "***********", msg.getMethod(), msg.getUrl())));
	auto writtenByte = 0LL;
	sendTimer.start(mSendTimeout);
	do {
		auto written = mpSock->write(data.last(data.size() - writtenByte));
		if(written >= 0) {
			writtenByte += written;
		} else {
			qWarning() << "Writing RTSP message failed" << mpSock->peerAddress().toString();
			result = DetailedResult<RtspMessageResponse>(Result::FAULT, tr("Writing RTSP message failed"));
			break;
		}
	} while(writtenByte < data.size());
	sendTimer.disconnect(mpLoop);
	sendTimer.stop();
	receiveTimer.start(mReceiveTimeout);
	mpLoop->exec(); // quits as soon as rtsp message (+ payload) was complete read, error or timeout occurred
	mpSock->disconnect(mpLoop);
	receiveTimer.disconnect(mpLoop);
	receiveTimer.stop();
	if(result) {
		const RtspMessageResponse response(message, payload);
		if(response.getStatus() == 200) {
			qDebug() << "Successfully sent message" << mpSock->peerAddress().toString();
			if(!payload.isEmpty()) {
				qDebug() << "├" << QString::fromUtf8(message);
				qDebug() << "└" << QString::fromUtf8(payload);
			} else {
				qDebug() << "└" << QString::fromUtf8(message);
			}
			result = DetailedResult<RtspMessageResponse>(response);
		} else if(response.getStatus() == 401 && !retry) {
			// retry
			qDebug() << "Authentication is required" << mpSock->peerAddress().toString();
			auto auths = OnvifRtspClient::parseAuthFields(response.getFields("WWW-Authenticate"));
			if(!auths.empty()) {
				mAuth = auths.first();
			} else {
				qWarning() << "Could not parse authenticate header" << mpSock->peerAddress().toString();
			}
			return sendRtspMsg(msg, true);
		} else {
			qWarning() << "RTSP request failed" << response.getStatusMessage();
			result = DetailedResult<RtspMessageResponse>(Result::FAULT, tr("RTSP request failed: %1").arg(response.getStatusMessage()));
		}
	}
	return result;
}

QList<OnvifRtspClient::Auth> OnvifRtspClient::parseAuthFields(const QList<QString> &msg) {

	QList<OnvifRtspClient::Auth> ret;
	for(const auto &line : msg) {
		auto auth = OnvifRtspClient::Auth::fromResponse(line);
		if(!auth.isNull()) {
			ret.append(auth);
		} else {
			qWarning() << "Got invalid WWW-Authenticate header from server";
		}
	}
	std::sort(ret.begin(), ret.end(), [](const auto &one, const auto &two) { return one.prefer() > two.prefer(); });
	return ret;
}

quint16 OnvifRtspClient::randomRtpPort() {

	quint16 port = QRandomGenerator::global()->bounded(MIN_DYNAMIC_PORT, MAX_DYNAMIC_PORT);
	if(port % 2 == 1) {
		port++;
	}
	return port;
}

OnvifRtspClient::Auth::Auth() : mType(""), mRealm(""), mNonce(""), mAlgorithmStr("MD5"), mAlgorithm(QCryptographicHash::Md5) {}

QString OnvifRtspClient::Auth::getAuthHeader(const QString &user, const QString &password, const QString &method,
                                             const QString &uri) const {

	QString ret;
	if(mType == "Basic") {
		ret = QString("Basic %1").arg(QString("%1:%2").arg(user, password).toUtf8().toBase64());
	} else if(mType == "Digest") {
		// TODO: more options may exist that are not implemented
		const auto HA1 = QCryptographicHash::hash(QString("%1:%2:%3").arg(user, mRealm, password).toUtf8(), mAlgorithm);
		const auto HA2 = QCryptographicHash::hash(QString("%1:%2").arg(method, uri).toUtf8(), mAlgorithm);
		auto response = QCryptographicHash::hash(QString("%1:%2:%3").arg(HA1.toHex(), mNonce, HA2.toHex()).toUtf8(), mAlgorithm);
		ret = QString(R"(Digest username="%1", realm="%2", nonce="%3", uri="%4", response="%5", algorithm="%6")")
		       .arg(user, mRealm, mNonce, uri, response.toHex(), mAlgorithmStr);
	}
	return ret;
}

OnvifRtspClient::Auth OnvifRtspClient::Auth::fromResponse(const QString &WWWAuthenticate) {

	auto auth = Auth();
	static auto realmRegex = QRegularExpression("realm=\"([^\"]+)");
	static auto nonceRegex = QRegularExpression("nonce=\"([^\"]+)");
	static auto algorithmRegex = QRegularExpression("algorithm=\"([^\"]+)");
	if(WWWAuthenticate.startsWith("Digest")) {
		auth.mNonce = nonceRegex.match(WWWAuthenticate).captured(1);
		auth.mRealm = realmRegex.match(WWWAuthenticate).captured(1);
		if(!auth.mNonce.isEmpty() && !auth.mRealm.isEmpty()) {
			auth.mType = "Digest";
			auto algorithm = algorithmRegex.match(WWWAuthenticate).captured(1);
			if(algorithm == "SHA-256") {
				auth.mAlgorithmStr = algorithm;
				auth.mAlgorithm = QCryptographicHash::Sha256;
			} else {
				auth.mAlgorithmStr = "MD5";
				auth.mAlgorithm = QCryptographicHash::Md5;
			}
		}
	} else if(WWWAuthenticate.startsWith("Basic")) {
		auth.mType = "Basic";
		auth.mRealm = realmRegex.match(WWWAuthenticate).captured(1);
	}
	return auth;
}

bool OnvifRtspClient::Auth::isNull() const {

	return mType.isEmpty();
}

int OnvifRtspClient::Auth::prefer() const {

	auto ret = 0;
	if(!isNull()) {
		if(mType == "Basic") {
			ret = 1;
		} else if(mType == "Digest") {
			ret = 2;
			if(mAlgorithm == QCryptographicHash::Sha256) {
				ret++;
			}
		}
	}
	return ret;
}
