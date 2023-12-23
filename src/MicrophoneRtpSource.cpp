#include "MicrophoneRtpSource.h"
#include "OnvifRtspClient.h"
#include <QAudioInput>
#include <QAudioSource>
#include <QBuffer>
#include <QScopedPointer>
#include <QWaitCondition>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/timestamp.h>
}


#define AVOK 0

struct AVDictionaryDeleter {
	static inline void cleanup(AVDictionary **pointer) {
		if(*pointer) {
			av_dict_free(pointer);
		}
	}
};

struct AVFormatContextDeleter {
	static inline void cleanup(AVFormatContext *pointer) {
		if(pointer) {
			avformat_free_context(pointer);
		}
	}
};

struct AVCodecContextDeleter {
	static inline void cleanup(AVCodecContext *pointer) {
		if(pointer) {
			avcodec_free_context(&pointer);
		}
	}
};

struct AVFilterContextDeleter {
	static inline void cleanup(AVFilterContext *pointer) {
		if(pointer) {
			avfilter_free(pointer);
		}
	}
};

struct AVFilterGraphDeleter {
	static inline void cleanup(AVFilterGraph *pointer) {
		if(pointer) {
			avfilter_graph_free(&pointer);
		}
	}
};

struct AVIOContextDeleter {
	static inline void cleanup(AVIOContext *pointer) {
		if(pointer) {
			avio_close(pointer);
			avio_context_free(&pointer);
		}
	}
};

struct AVFrametDeleter {
	static inline void cleanup(AVFrame *pointer) {
		if(pointer) {
			av_frame_unref(pointer);
			av_frame_free(&pointer);
		}
	}
};

struct AVPacketDeleter {
	static inline void cleanup(AVPacket *pointer) {
		if(pointer) {
			av_packet_unref(pointer);
			av_packet_free(&pointer);
		}
	}
};

struct QAudioSourceCloser {
	static inline void cleanup(QAudioSource *pointer) {
		if(pointer) {
			pointer->stop();
		}
	}
};

struct QIODeviceCloser {
	static inline void cleanup(QIODevice *pointer) {
		if(pointer) {
			pointer->close();
		}
	}
};

MicrophoneRtpSource::MicrophoneRtpSource(QObject *parent) :
 QThread(parent),
 mRtpSink(),
 mpAudioSource(nullptr),
 mPayloadFormat(Auto),
 mVolume(std::numeric_limits<int>::max()),
 mMute(1),
 mResult(),
 mpBuffer(nullptr) {

	setObjectName("MicrophoneRtpSource");

	connect(this, &QThread::finished, this, [this]() {
		if(mpAudioSource) {
			mpAudioSource->stop();
		}
	});
}

MicrophoneRtpSource::~MicrophoneRtpSource() {

	disconnect(this, nullptr, this, nullptr);
	stop();
}

void MicrophoneRtpSource::start(const QUrl &rtpSink) {

	stop();
	mRtpSink = rtpSink;
	if(!mRtpSink.host().isEmpty()) {
		if(mpAudioSource) {
			if(auto *buffer = mpAudioSource->start()) {
				mpBuffer = buffer;
				if(mpAudioSource->error() == QAudio::NoError) {
					QThread::start(QThread::HighPriority);
				} else {
					qWarning() << "QAudioSource signaled an error" << mpAudioSource->error();
					mResult = Result(Result::FAULT, tr("Microphone audio source did not start"));
				}
			} else {
				qWarning() << "QBuffer could not be opened";
				mResult = Result(Result(Result::FAULT, tr("Could not open audio buffer")));
			}
		} else {
			qWarning() << "QAudioSource is null";
			mResult = Result(Result(Result::FAULT, tr("Missing a valid microphone audio source")));
		}

	} else {
		qWarning() << "Rtp url is unsupported" << mRtpSink;
		mResult = Result(Result(Result::FAULT, tr("Missing a valid rtp host")));
	}
}

void MicrophoneRtpSource::stop() {

	requestInterruption();
	wait();
	if(mpAudioSource) {
		mpAudioSource->stop();
	}
}

void MicrophoneRtpSource::mute(bool mute) {

	if(mute) {
		mMute = 0;
	} else {
		mMute = 1;
	}
}

void MicrophoneRtpSource::setVolume(float volume) {

	if(volume >= 1.0) {
		mVolume = std::numeric_limits<int>::max();
	} else if(volume <= 0) {
		mVolume = 0;
	} else {
		mVolume = volume * std::numeric_limits<int>::max();
		qInfo() << mVolume << volume;
	}
}

QAudioInput *MicrophoneRtpSource::getAudioInput() const {

	return nullptr;
}

void MicrophoneRtpSource::setAudioInput(QAudioInput *input) {

	stop();
	if(input) {
		connect(input, &QAudioInput::volumeChanged, this, &MicrophoneRtpSource::setVolume);
		connect(input, &QAudioInput::mutedChanged, this, &MicrophoneRtpSource::mute);
		connect(input, &QAudioInput::deviceChanged, this, [this, input]() {
			input->disconnect(this);
			setAudioInput(input);
		});
		if(mpAudioSource) {
			mpAudioSource->deleteLater();
			mpAudioSource = nullptr;
		}
		auto device = input->device();
		qInfo() << device.description();
		mpAudioSource = new QAudioSource(device, device.preferredFormat(), this);
	}
}

MicrophoneRtpSource::RtpPayload MicrophoneRtpSource::getPayloadFormat() const {

	return mPayloadFormat;
}

void MicrophoneRtpSource::setPayloadFormat(MicrophoneRtpSource::RtpPayload fmt) {

	mPayloadFormat = fmt;
	emit payloadFormatChanged();
}

/*
rtp AVOptions:
-ttl               <int>        ED......... Time to live (multicast only) (from -1 to 255) (default -1)
-buffer_size       <int>        ED......... Send/Receive buffer size (in bytes) (from -1 to INT_MAX) (default -1)
-rtcp_port         <int>        ED......... Custom rtcp port (from -1 to INT_MAX) (default -1)
-local_rtpport     <int>        ED......... Local rtp port (from -1 to INT_MAX) (default -1)
-local_rtcpport    <int>        ED......... Local rtcp port (from -1 to INT_MAX) (default -1)
-connect           <boolean>    ED......... Connect socket (default false)
-write_to_source   <boolean>    ED......... Send packets to the source address of the latest received packet (default false)
-pkt_size          <int>        ED......... Maximum packet size (from -1 to INT_MAX) (default -1)
-dscp              <int>        ED......... DSCP class (from -1 to INT_MAX) (default -1)
-timeout           <int64>      ED......... set timeout (in microseconds) of socket I/O operations (from -1 to I64_MAX) (default -1)
-sources           <string>     ED......... Source list
-block             <string>     ED......... Block list
-fec               <string>     E.......... FEC
-localaddr         <string>     ED......... Local address
*/

/*
 RTP muxer AVOptions:
 -rtpflags          <flags>      E.......... RTP muxer flags (default 0)
    latm                         E.......... Use MP4A-LATM packetization instead of MPEG4-GENERIC for AAC
    rfc2190                      E.......... Use RFC 2190 packetization instead of RFC 4629 for H.263
    skip_rtcp                    E.......... Don't send RTCP sender reports
    h264_mode0                   E.......... Use mode 0 for H.264 in RTP
    send_bye                     E.......... Send RTCP BYE packets when finishing
 -payload_type      <int>        E.......... Specify RTP payload type (from -1 to 127) (default -1)
 -ssrc              <int>        E.......... Stream identifier (from INT_MIN to INT_MAX) (default 0)
 -cname             <string>     E.......... CNAME to include in RTCP SR packets
 -seq               <int>        E.......... Starting sequence number (from -1 to 65535) (default -1)
*/

void MicrophoneRtpSource::run() {

	const auto rtspClient = QScopedPointer<OnvifRtspClient>(new OnvifRtspClient(mRtpSink));
	if(auto rtspResult = rtspClient->startAudioBackchannelStream()) {

		const auto rtspStream = rtspResult.GetResultObject();
		const auto mediaDescription = rtspStream.getMediaDescription();

		const auto rtpUrl =
		 QString("rtp://%1:%2?localrtpport=%3").arg(mRtpSink.host()).arg(rtspStream.getServerRtpPort()).arg(rtspStream.getClientRtpPort());
		qInfo() << "Starting microphone rtp loop with url" << rtpUrl;

		auto bufferSize = mpAudioSource->bufferSize();
		auto srcFormat = mpAudioSource->format();

		qInfo() << "Started QAudioSource with buffer size" << bufferSize << "and format" << srcFormat;

		if(const auto *format = av_guess_format("rtp", nullptr, nullptr)) {

			AVFormatContext *acfcptr = nullptr;
			avformat_alloc_output_context2(&acfcptr, format, nullptr, nullptr);
			const QScopedPointer<AVFormatContext, AVFormatContextDeleter> avfc(acfcptr);

			if(avfc) {

				if(auto *avs = avformat_new_stream(avfc.get(), nullptr)) {

					const auto encoderSettings = MicrophoneRtpSource::selectEncoder(mPayloadFormat, mediaDescription);

					if(!encoderSettings.codecName.isEmpty()) {

						if(const auto *avc = avcodec_find_encoder_by_name(qPrintable(encoderSettings.codecName))) {

							qInfo() << "Selected encoder" << avc->name << "bitrate" << encoderSettings.bitrate << "sample rate"
							        << encoderSettings.samplerate << "channels" << encoderSettings.numChannels;

							if(auto avcc = QScopedPointer<AVCodecContext, AVCodecContextDeleter>(avcodec_alloc_context3(avc))) {

								av_channel_layout_default(&avcc->ch_layout, encoderSettings.numChannels);
								avcc->sample_rate = encoderSettings.samplerate;
								avcc->sample_fmt = avc->sample_fmts[0];
								if(encoderSettings.bitrate > 0) {
									avcc->bit_rate = encoderSettings.bitrate;
								}
								avcc->time_base = av_make_q(1, encoderSettings.samplerate);
								avcc->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

								if(auto codecResult = MicrophoneRtpSource::toResult(avcodec_open2(avcc.get(), avc, nullptr))) {

									avcodec_parameters_from_context(avs->codecpar, avcc.get());

									if(avfc->oformat->flags & AVFMT_GLOBALHEADER) {
										avfc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
									}

									auto volume = calcVolume();

									auto filterStr = QString("abuffer=time_base=%1:sample_fmt=%2:channel_layout=%3:sample_rate=%4,"
									                         "volume=volume=%5:precision=%6,"
									                         "aformat=sample_fmts=%7:channel_layouts=%8:sample_rates=%9,"
									                         "asetnsamples=n=%10:p=1,"
									                         "abuffersink")
									                  .arg(QString("1/%2").arg(srcFormat.sampleRate()))
									                  .arg(av_get_sample_fmt_name(MicrophoneRtpSource::convertSampleFormat(srcFormat.sampleFormat())))
									                  .arg(MicrophoneRtpSource::chLayoutName(MicrophoneRtpSource::convertChLayout(srcFormat.channelConfig())))
									                  .arg(srcFormat.sampleRate())
									                  .arg(volume)
									                  .arg(MicrophoneRtpSource::convertSampleFormatPrecision(srcFormat.sampleFormat()))
									                  .arg(av_get_sample_fmt_name(avcc->sample_fmt))
									                  .arg(MicrophoneRtpSource::chLayoutName(avcc->ch_layout))
									                  .arg(avcc->sample_rate)
									                  .arg(encoderSettings.frameSize > 0 ? encoderSettings.frameSize
									                       : avcc->frame_size > 0        ? avcc->frame_size
									                                                     : 1024);

									qInfo() << "Using filter" << filterStr;

									if(auto graph = QScopedPointer<AVFilterGraph, AVFilterGraphDeleter>(avfilter_graph_alloc())) {
										AVFilterInOut *filterIn = nullptr;
										AVFilterInOut *filterOut = nullptr;

										if(auto filterResult =
										    MicrophoneRtpSource::toResult(avfilter_graph_parse2(graph.get(), qPrintable(filterStr), &filterIn, &filterOut))) {

											if(auto filterConfigResult = MicrophoneRtpSource::toResult(avfilter_graph_config(graph.get(), nullptr))) {

												AVFilterContext *buffersrc = nullptr;
												AVFilterContext *buffersink = nullptr;

												for(int i = 0; i < graph->nb_filters; ++i) {
													if(QString::fromLocal8Bit(graph->filters[i]->name).contains("abuffersink")) {
														buffersink = graph->filters[i];
													} else if(QString::fromLocal8Bit(graph->filters[i]->name).contains("abuffer")) {
														buffersrc = graph->filters[i];
													}
												}

												if(buffersrc != nullptr && buffersink != nullptr) {

													if(auto openResult = MicrophoneRtpSource::toResult(avio_open(&avfc->pb, qPrintable(rtpUrl), AVIO_FLAG_WRITE))) {

														AVDictionary *muxer_opts = nullptr;
														av_dict_set_int(&muxer_opts, "payload_type", encoderSettings.payloadType, 0);
														av_dict_set_int(&muxer_opts, "ssrc", rtspStream.getSsrc(), 0);
														const QScopedPointer<AVDictionary *, AVDictionaryDeleter> dictScope(&muxer_opts);

														if(auto writeHeaderResult = MicrophoneRtpSource::toResult(avformat_write_header(avfc.get(), &muxer_opts))) {

															const QScopedPointer<AVFrame, AVFrametDeleter> frame(av_frame_alloc());
															const QScopedPointer<AVPacket, AVPacketDeleter> packet(av_packet_alloc());

															qint64 pts = 0;
															bool reported = false;

															// ---- begin filter write loop ----
															do {

																if(isInterruptionRequested()) {
																	qInfo() << "Interruption of microphone rtp loop was requested";
																	break;
																}

																if(mpAudioSource->bytesAvailable() <= 0) {

																	QThread::usleep(qMax(1000, srcFormat.durationForBytes(bufferSize) / 2));
																	continue;
																}

																const auto byteRead = MicrophoneRtpSource::readToFrame(mpBuffer, frame.get());
																frame->ch_layout = MicrophoneRtpSource::convertChLayout(srcFormat.channelConfig());
																frame->format = MicrophoneRtpSource::convertSampleFormat(srcFormat.sampleFormat());
																frame->nb_samples = byteRead / srcFormat.bytesPerFrame();
																frame->sample_rate = srcFormat.sampleRate();
																frame->pts = pts;
																pts += frame->nb_samples;

																auto newVolume = calcVolume();
																if(newVolume != volume) {
																	avfilter_graph_send_command(graph.get(), "volume", "volume", qPrintable(QString::number(newVolume)),
																	                            nullptr, 0, 0);
																	volume = newVolume;
																}

																// write to filter -------->
																auto writeFilterFrameResult =
																 MicrophoneRtpSource::toResult(av_buffersrc_write_frame(buffersrc, frame.get()));

																if(!writeFilterFrameResult) {
																	qWarning() << "av_buffersrc_write_frame returned an error" << writeFilterFrameResult;
																	mResult = Result(Result(Result::FAULT, tr("Could not write samples to filter")));
																	QThread::requestInterruption();
																	break;
																}

																av_frame_unref(frame.get());

																auto skipFilter = false;

																// ---- begin filter read loop ----
																do {

																	if(isInterruptionRequested()) {
																		break;
																	}

																	if(!skipFilter) {

																		// read from filter <--------
																		const auto readFilterFrameResult =
																		 MicrophoneRtpSource::toResultExpect(av_buffersink_get_frame(buffersink, frame.get()),
																		                                     AVERROR(EAGAIN)); // EAGAIN (more input required)

																		if(readFilterFrameResult) {
																			if(readFilterFrameResult.GetResultObject() == AVERROR(EAGAIN)) {
																				// early finished filter loop - filter needs more samples
																				break;
																			}
																		} else {
																			qWarning() << "av_buffersink_get_frame returned an unexpected error" << readFilterFrameResult;
																			mResult = Result(Result(Result::FAULT, tr("Could not read samples from filter")));
																			QThread::requestInterruption();
																			break;
																		}
																	}

																	// write to codec -------->
																	auto writeCodecFrameResult =
																	 MicrophoneRtpSource::toResultExpect(avcodec_send_frame(avcc.get(), frame.get()), AVERROR(EAGAIN));

																	if(writeCodecFrameResult) {
																		if(writeCodecFrameResult.GetResultObject() == AVERROR(EAGAIN)) {
																			skipFilter =
																			 true; // keep the reference to the frame - we have to send it again after we received a package
																		} else {
																			av_frame_unref(frame.get());
																			skipFilter = false;
																		}
																	} else {
																		qWarning() << "avcodec_send_frame returned an unexpected error" << writeCodecFrameResult;
																		mResult = Result(Result(Result::FAULT, tr("Could not write samples to encoder")));
																		QThread::requestInterruption();
																		break;
																	}

																	// ---- begin codec loop ----
																	do {

																		if(isInterruptionRequested()) {
																			break;
																		}

																		// read from codec <--------
																		auto receiveCodecPackageResult = MicrophoneRtpSource::toResultExpect(
																		 avcodec_receive_packet(avcc.get(), packet.get()), AVERROR(EAGAIN)); // EAGAIN (send input again)

																		if(receiveCodecPackageResult) {
																			if(receiveCodecPackageResult.GetResultObject() == AVERROR(EAGAIN)) {
																				// early finished codec loop - encoder needs more samples
																				break;
																			}
																		} else {
																			qWarning() << "avcodec_receive_packet returned an unexpected error" << receiveCodecPackageResult;
																			mResult = Result(Result(Result::FAULT, tr("Could not read samples from encoder")));
																			QThread::requestInterruption();
																			break;
																		}

																		packet->stream_index = 0;

																		// av_packet_rescale_ts(output_packet, decoder->audio_avs->time_base,
																		// encoder->audio_avs->time_base);

																		auto sendResult = MicrophoneRtpSource::toResult(av_interleaved_write_frame(avfc.get(), packet.get()));

																		av_packet_unref(packet.get());

																		if(sendResult) {
																			if(!reported) {
																				reported = true;
																				mResult = Result(Result::OK);
																			}
																		} else {
																			qWarning() << "av_interleaved_write_frame returned an unexpected error" << sendResult;
																			mResult = Result(Result(Result::FAULT, tr("Could not send rtp package")));
																			QThread::requestInterruption();
																			break;
																		}

																		// break;

																	} while(true);
																	// ---- end codec loop ----
																} while(true);
																// ---- end filter read loop ----
															} while(true);
															// ---- end filter write loop ----

															mResult = Result(Result(Result::OK));

														} else {
															qWarning() << "avformat_write_header returned an error" << writeHeaderResult;
															mResult = Result(Result(Result::FAULT, tr("Could not send to url %s").arg(rtpUrl)));
														}
													} else {
														qWarning() << "avio_open returned an error" << openResult;
														mResult = Result(Result(Result::FAULT, tr("Could not connect to url %s").arg(rtpUrl)));
													}
												} else {
													qWarning() << "could not find filter abuffersink or abuffer";
													mResult = Result(Result(Result::FAULT, tr("Could not validate audio filter")));
												}
											} else {
												qWarning() << "avfilter_graph_config returned an error" << filterConfigResult;
												mResult = Result(Result(Result::FAULT, tr("Could not configure audio filter")));
											}
										} else {
											qWarning() << "avfilter_graph_parse2 returned an error" << filterResult;
											mResult = Result(Result(Result::FAULT, tr("Could not parse audio filter")));
										}
									} else {
										qWarning() << "avfilter_graph_alloc returned null";
										mResult = Result(Result(Result::FAULT, tr("Could not create audio filter")));
									}
								} else {
									qWarning() << "avcodec_open2 returned an error" << codecResult;
									mResult = Result(Result(Result::FAULT, tr("Could not start %1 encoder").arg(avc->long_name)));
								}
							} else {
								qWarning() << "avcodec_alloc_context3 returned null for codec" << avc->name << "bitrate" << encoderSettings.bitrate
								           << "sample rate" << encoderSettings.samplerate;
								mResult = Result(Result(Result::FAULT, tr("Could not provide %1 encoder").arg(avc->long_name)));
							}
						} else {
							qWarning() << "could not find codec" << encoderSettings.codecName << "Was it deactivated in this ffmpeg build?";
							mResult = Result(Result(Result::FAULT, tr("Could not find %1 encoder").arg(encoderSettings.codecName)));
						}
					} else {
						qWarning() << "the sdp does not contain the requested payload format or the codec was deactivated in this ffmpeg build"
						           << mPayloadFormat;
						mResult = Result(Result(Result::FAULT, tr("The device does not support the requested payload format")));
					}
				} else {
					qWarning() << "avformat_new_stream returned null";
					mResult = Result(Result(Result::FAULT, tr("Could not create rtp audio stream")));
				}
			} else {
				qWarning() << "avformat_alloc_output_context2 did not allocate memory for output format";
				mResult = Result(Result(Result::FAULT, tr("Could not provide rtp output format")));
			}
		} else {
			qWarning() << "Rtp output format not found. Was it deactivated in this ffmpeg build?";
			mResult = Result(Result(Result::FAULT, tr("Could not find rtp output format")));
		}
	} else {
		qWarning() << rtspResult;
		mResult = Result(Result(Result::FAULT, tr("Could not start RTSP session")));
	}

	qInfo() << "Finished running microphone rtp loop";
}

double MicrophoneRtpSource::calcVolume() {

	return (double)mVolume / std::numeric_limits<int>::max() * mMute;
}

int MicrophoneRtpSource::readToFrame(QIODevice *ioDev, AVFrame *frame) {

	if(ioDev && frame) {
		av_frame_unref(frame);
		auto *data = new QByteArray(ioDev->readAll());

		frame->buf[0] = av_buffer_create(
		 reinterpret_cast<uint8_t *>(data->data()), data->size(),
		 [](void *opaque, uint8_t *data) {
			 Q_UNUSED(data);
			 delete static_cast<QByteArray *>(opaque);
		 },
		 static_cast<void *>(data), 0);

		frame->data[0] = frame->buf[0]->data;
		frame->linesize[0] = data->size();
		return data->size();
	}
	return 0;
}

AVChannelLayout MicrophoneRtpSource::convertChLayout(QAudioFormat::ChannelConfig cfg) {

	switch(cfg) {
		case QAudioFormat::ChannelConfigMono:
			return AV_CHANNEL_LAYOUT_MONO;
		case QAudioFormat::ChannelConfigStereo:
			return AV_CHANNEL_LAYOUT_STEREO;
		case QAudioFormat::ChannelConfig2Dot1:
			return AV_CHANNEL_LAYOUT_2POINT1;
		case QAudioFormat::ChannelConfig3Dot0:
			return AV_CHANNEL_LAYOUT_SURROUND;
		case QAudioFormat::ChannelConfig3Dot1:
			return AV_CHANNEL_LAYOUT_3POINT1;
		case QAudioFormat::ChannelConfigSurround5Dot0:
			return AV_CHANNEL_LAYOUT_5POINT0;
		case QAudioFormat::ChannelConfigSurround5Dot1:
			return AV_CHANNEL_LAYOUT_5POINT1;
		case QAudioFormat::ChannelConfigSurround7Dot0:
			return AV_CHANNEL_LAYOUT_7POINT0;
		case QAudioFormat::ChannelConfigSurround7Dot1:
			return AV_CHANNEL_LAYOUT_7POINT1;
		default:
			return AV_CHANNEL_LAYOUT_MONO;
	}
}

AVSampleFormat MicrophoneRtpSource::convertSampleFormat(QAudioFormat::SampleFormat fmt) {

	switch(fmt) {
		case QAudioFormat::UInt8:
			return AV_SAMPLE_FMT_U8;
		case QAudioFormat::Int16:
			return AV_SAMPLE_FMT_S16;
		case QAudioFormat::Int32:
			return AV_SAMPLE_FMT_S32;
		case QAudioFormat::Float:
			return AV_SAMPLE_FMT_FLT;
		default:
			return AV_SAMPLE_FMT_NONE;
	}
}

QString MicrophoneRtpSource::convertSampleFormatPrecision(QAudioFormat::SampleFormat fmt) {

	switch(fmt) {
		case QAudioFormat::UInt8:
		case QAudioFormat::Int16:
		case QAudioFormat::Int32:
			return {"fixed"};
		case QAudioFormat::Float:
			return {"float"};
		default:
			return {""};
	}
}

QString MicrophoneRtpSource::chLayoutName(AVChannelLayout layout) {

	auto inChLayoutName = QByteArray(255, 0);
	if(const auto read = av_channel_layout_describe(&layout, inChLayoutName.data(), 255)) {
		if(read > 0) {
			return QString::fromLocal8Bit(inChLayoutName.first(read - 1));
		}
	}
	return {};
}

MicrophoneRtpSource::EncoderSettings MicrophoneRtpSource::selectEncoder(MicrophoneRtpSource::RtpPayload payloadType, MediaDescription md) {

	// map rtp payload format name to ffmpeg codec name and include a priority number
	static const auto codecMap = QHash<QString, MicrophoneRtpSource::EncoderSettings>({
	 {"mpeg4-generic",
	  {"aac", RTP_mpeg4_generic, -1 /*bitrate*/, -1 /*sample rate*/, -1 /*channels*/, -1 /*type*/, -1 /*frame size*/, 7 /*priority*/}},
	 {"MP4A-LATM",
	  {"aac", RTP_MP4A_LATM, -1 /*bitrate*/, -1 /*sample rate*/, -1 /*channels*/, -1 /*type*/, -1 /*frame size*/, 6 /*priority*/}},
	 {"PCMU",
	  {"pcm_mulaw", RTP_PCMU_8000_1, 64000 /*bitrate*/, 8000 /*sample rate*/, 1 /*channels*/, 0 /*type*/, 160 /*frame size*/,
	   5 /*priority*/}},
	 {"G726-40",
	  {"adpcm_g726", RTP_G726_40_8000_1, 40000 /*bitrate*/, 8000 /*sample rate*/, 1 /*channels*/, -1 /*type*/, -1 /*frame size*/,
	   4 /*priority*/}},
	 {"G726-32",
	  {"adpcm_g726", RTP_G726_32_8000_1, 32000 /*bitrate*/, 8000 /*sample rate*/, 1 /*channels*/, -1 /*type*/, -1 /*frame size*/,
	   3 /*priority*/}},
	 {"G726-24",
	  {"adpcm_g726", RTP_G726_24_8000_1, 24000 /*bitrate*/, 8000 /*sample rate*/, 1 /*channels*/, -1 /*type*/, -1 /*frame size*/,
	   2 /*priority*/}},
	 {"G726-16",
	  {"adpcm_g726", RTP_G726_16_8000_1, 16000 /*bitrate*/, 8000 /*sample rate*/, 1 /*channels*/, -1 /*type*/, -1 /*frame size*/,
	   1 /*priority*/}},
	 {"G726",
	  {"g726", RTP_G726_16_8000_1, 16000 /*bitrate*/, 8000 /*sample rate*/, 1 /*channels*/, -1 /*type*/, -1 /*frame size*/, 1 /*priority*/}},
	});

	auto formats = md.getPayloadFormats();
	// sort by preferred codec
	std::sort(formats.begin(), formats.end(), [](const RtpPayloadFormat &left, const RtpPayloadFormat &right) {
		auto leftNm = 0;
		auto rightNm = 0;
		if(codecMap.contains(left.name)) leftNm = codecMap[left.name].priority;
		if(codecMap.contains(right.name)) rightNm = codecMap[right.name].priority;
		return leftNm > rightNm;
	});
	// remove unsupported codecs
	formats.removeIf([](const RtpPayloadFormat &desc) {
		if(codecMap.contains(desc.name)) {
			auto codec = avcodec_find_encoder_by_name(qPrintable(codecMap[desc.name].codecName));
			return codec == nullptr;
		}
		return true;
	});

	MicrophoneRtpSource::EncoderSettings encoderSettings = {};

	if(formats.empty()) {
		qWarning() << "Missing a payload format in sdp - default to PCMU";
	}

	for(const auto &format : formats) {
		encoderSettings = codecMap[format.name];
		if(format.payloadType > 0) {
			encoderSettings.payloadType = format.payloadType;
		}
		if(format.numChannels > 0) {
			encoderSettings.numChannels = format.numChannels;
		}
		if(format.sampleRate > 0) {
			encoderSettings.samplerate = format.sampleRate;
		}
		if(encoderSettings.numChannels <= 0) {
			qWarning() << format.name << "missing a valid channel number - default to mono";
			encoderSettings.numChannels = 1;
		}
		if(encoderSettings.payloadType < 0) {
			qWarning() << format.name << "missing a valid payload type";
		}
		if(payloadType == Auto || payloadType == encoderSettings.payload) {
			break;
		}
	}

	return encoderSettings;
}

Result MicrophoneRtpSource::toResult(int ffmpegError) {

	auto result = Result::OK;
	if(ffmpegError == AVOK) {
		result = Result::OK;
	} else {
		auto msg = QByteArray(AV_ERROR_MAX_STRING_SIZE, 0);
		av_make_error_string(msg.data(), AV_ERROR_MAX_STRING_SIZE, ffmpegError);
		result = Result(Result::FAULT, QString::fromLocal8Bit(msg, -1));
	}
	return result;
}

DetailedResult<int> MicrophoneRtpSource::toResultExpect(int ffmpegError, int expectedError) {

	auto res = DetailedResult<int>(Result::OK, "");
	if(ffmpegError == AVOK || ffmpegError == expectedError) {
		res.setResultObject(ffmpegError);
	} else {
		res = DetailedResult<int>(MicrophoneRtpSource::toResult(ffmpegError), "");
	}
	return res;
}
