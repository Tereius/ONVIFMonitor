#include "Result.h"
#include <QScopedPointer>


Result::Result() :
mValue(Result::OK.mValue), mLabel(Result::OK.mLabel), mDetails() {

}

Result::Result(const Result &rType, const QString &rDetails) :
mValue(rType.mValue), mLabel(rType.mLabel), mDetails(rDetails) {

}

Result::Result(int value, const QString &rLabel) :
mValue(value), mLabel(rLabel), mDetails() {

}

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
		if(rResponse.IsAuthFault()) {
			res = Result(Result::UNAUTHORIZED, rResponse.GetSoapFault());
		}
		else {
			res = Result(Result::RPC, rResponse.GetSoapFault());
		}
		res.setDetails(rResponse.GetCompleteFault());
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

const Result Result::OK(RESULT_OK, "Success");
const Result Result::UNKNOWN(RESULT_UNKNOWN, "Unknown");
const Result Result::FAULT(RESULT_FAULT, "Generic Fault");
const Result Result::RPC(RESULT_RPC, "Rpc Fault");
const Result Result::UNAUTHORIZED(RESULT_UNAUTHORIZED, "Unauthorized");
