#pragma once
#include "AsyncFuture/asyncfuture.h"
#include "Result.h"
#include "SessionDescription.h"
#include <QAtomicInt>
#include <QAudioInput>
#include <QAudioSource>
#include <QByteArray>
#include <QFuture>
#include <QThread>
#include <QUrl>
#include <QtQmlIntegration>
extern "C" {
#include <libavutil/samplefmt.h>
}


class QAudioSource;
class QIODevice;
struct AVFrame;
struct AVChannelLayout;

class EncoderSettings {

	Q_GADGET
	Q_PROPERTY(QString codec MEMBER codecName)

 public:
	// https://en.wikipedia.org/wiki/RTP_payload_formats
	enum RtpPayload {
		Auto,
		RTP_G726_16_8000_1, // RFC 3551
		RTP_G726_24_8000_1, // RFC 3551
		RTP_G726_32_8000_1, // RFC 3551
		RTP_G726_40_8000_1, // RFC 3551
		RTP_PCMU_8000_1, // RFC 3551
		RTP_mpeg4_generic, // RFC 3640
		RTP_MP4A_LATM // RFC 6416
	};
	Q_ENUM(RtpPayload)

	QString codecName;
	RtpPayload payload;
	int bitrate;
	int samplerate;
	int numChannels;
	int payloadType;
	int frameSize; // number of samples per av frame
	int priority;
};

class MicrophoneRtpSource : public QThread {

	Q_OBJECT
	Q_PROPERTY(QAudioInput *audioInput READ getAudioInput WRITE setAudioInput)
	Q_PROPERTY(EncoderSettings::RtpPayload payloadFormat READ getPayloadFormat WRITE setPayloadFormat NOTIFY payloadFormatChanged)
	QML_ELEMENT

 public:
	explicit MicrophoneRtpSource(QObject *parent = nullptr);
	~MicrophoneRtpSource() override;

	// Start sending microphone audio stream to the rtp sink. Call from main thread
	Q_INVOKABLE void start(const QUrl &rtpSink);
	// Stop sending microphone audio stream to the rtp sink
	Q_INVOKABLE void stop();

	Q_INVOKABLE static QList<EncoderSettings> supportedEncoder(MediaDescription sdp);

	QAudioInput *getAudioInput() const;
	void setAudioInput(QAudioInput *input);
	EncoderSettings::RtpPayload getPayloadFormat() const;
	void setPayloadFormat(EncoderSettings::RtpPayload fmt);

 public slots:
	void mute(bool mute);
	void setVolume(float volume);

 signals:
	void payloadFormatChanged();

 protected:
	void run() override;

 private:
	void prepareRun();
	double calcVolume();
	static qint64 readToFrame(QIODevice *ioDev, AVFrame *frame);
	static AVChannelLayout convertChLayout(QAudioFormat::ChannelConfig cfg, int channelCount);
	static AVSampleFormat convertSampleFormat(QAudioFormat::SampleFormat fmt);
	static QString convertSampleFormatPrecision(QAudioFormat::SampleFormat fmt);
	static QString chLayoutName(AVChannelLayout layout);
	static EncoderSettings selectEncoder(EncoderSettings::RtpPayload payloadType, MediaDescription sdp);
	// convert ffmpeg error to Result
	static Result toResult(int ffmpegError);
	static DetailedResult<int> toResultExpect(int ffmpegError, int expectedError);

	QUrl mRtpSink;
	QAudioSource *mpAudioSource;
	EncoderSettings::RtpPayload mPayloadFormat;
	QAtomicInt mVolume;
	QAtomicInt mMute;
	QIODevice *mpBuffer;
	Result mResult;
};
