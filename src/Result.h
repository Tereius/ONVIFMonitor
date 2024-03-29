#pragma once
#include "SafeBool.h"
#include "Response.h"
#include <QString>
#include <QDebug>
#include <QMetaObject>


#define RESULT_OK 0
#define RESULT_UNKNOWN 1
#define RESULT_FAULT 2
#define RESULT_RPC 3
#define RESULT_UNAUTHORIZED 4

class Result : public SafeBool<Result> {

	Q_GADGET

public:
	Result();
	Result(const Result &rType, const QString &rDetails);
	~Result() {}
	inline bool BooleanTest() const { return isSuccess(); }
	Q_INVOKABLE inline bool isSuccess() const { return !isFault(); }
	Q_INVOKABLE inline bool isFault() const { return mValue > 0; }
	inline bool operator==(const Result &rOther) const { return mValue == rOther.mValue; }
	inline bool operator!=(const Result &rOther) const { return mValue != rOther.mValue; }
	inline int getErrorCode() const { return mValue; }
	inline const QString& getLabel() const { return mLabel; }
	inline const QString& getDetails() const { return mDetails; }
	QString toString() const;
	void setLabel(const QString &rLabel);
	void setDetails(const QString &rDetails);

	const static Result OK;
	const static Result UNKNOWN;
	const static Result FAULT;
	const static Result RPC;
	const static Result UNAUTHORIZED;
	static Result fromResponse(const SimpleResponse &rResponse, const QString &rDescription = QString());

private:
	Result(int value, const QString &rLabel);

	int mValue;
	QString mLabel;
	QString mDetails;
};

QDebug operator<<(QDebug debug, const Result &rRestult);


template <typename T>
class DetailedResult : public Result {

public:
	DetailedResult() :
		Result(){}
	DetailedResult(const Result &rType, const QString &rDetails) :
		Result(rType, rDetails){}
	void setResultObject(T resultObj) { mResultObject = resultObj; }
	T GetResultObject() const { return mResultObject; }
	static DetailedResult fromResponse(const SimpleResponse &rResponse, const QString &rDescription = QString()) {
		DetailedResult res;
		if(rResponse.IsFault()) {
			if(rResponse.IsAuthFault()) {
				res = DetailedResult(Result::UNAUTHORIZED, rResponse.GetSoapFault());
			}
			else {
				res = DetailedResult(Result::RPC, rResponse.GetSoapFault());
			}
			res.setDetails(rResponse.GetCompleteFault());
			if(!rDescription.isEmpty()) {
				res.setLabel(rDescription);
			}
		}
		return res;
	}

private:
		T mResultObject;
};
