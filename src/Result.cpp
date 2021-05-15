#include "Result.h"
#include <QTextDocumentFragment>
#include <QObject>


Result::Result() : mValue(Result::OK.mValue), mLabel(Result::OK.mLabel), mDetails() {}

Result::Result(const Result &rType, const QString &rDetails) : mValue(rType.mValue), mLabel(rType.mLabel), mDetails(rDetails) {}

Result::Result(int value, const QString &rLabel) : mValue(value), mLabel(rLabel), mDetails() {}

QString Result::toString() const {

	auto ret = QString(mLabel);
	if(!mDetails.isNull()) {
		ret.append(QString(" %1").arg(mDetails));
	}
	return ret;
}

void Result::setLabel(const QString &rLabel) {

	mLabel = rLabel;
}

void Result::setDetails(const QString &rDetails) {

	mDetails = rDetails;
}

Result Result::fromResponse(const SimpleResponse &rResponse, const QString &rDescription /*= QString()*/) {

	Result res;
	if(rResponse.IsFault()) {
		auto simpleMessage = QTextDocumentFragment::fromHtml(rResponse.GetSoapFault()).toPlainText().trimmed();
		if(simpleMessage.startsWith("null", Qt::CaseInsensitive)) simpleMessage.clear();
		auto detailedMessaage = QTextDocumentFragment::fromHtml(rResponse.GetSoapFaultDetail()).toPlainText().trimmed();
		if(detailedMessaage.startsWith("null", Qt::CaseInsensitive)) detailedMessaage.clear();
		if(rResponse.IsAuthFault()) {
			res = Result(Result::UNAUTHORIZED, detailedMessaage.isEmpty() ? simpleMessage : detailedMessaage);
		} else {
			res = Result(Result::RPC, detailedMessaage.isEmpty() ? simpleMessage : detailedMessaage);
		}
		if(!rDescription.isEmpty()) {
			res.setLabel(rDescription);
		}
	}
	return res;
}

QDebug operator<<(QDebug debug, const Result &rRestult) {
	QDebugStateSaver saver(debug);
	debug.nospace() << rRestult.getLabel() << ": " << rRestult.getDetails();
	return debug;
}

const Result Result::OK(RESULT_OK, "");
const Result Result::UNKNOWN(RESULT_UNKNOWN, QObject::tr("Unknown error"));
const Result Result::FAULT(RESULT_FAULT, QObject::tr("Error"));
const Result Result::RPC(RESULT_RPC, QObject::tr("Device error"));
const Result Result::UNAUTHORIZED(RESULT_UNAUTHORIZED, QObject::tr("Authorization error"));
