#pragma once
#include <QUrl>
#include <QMetaType>


class EventService {

	Q_GADGET
		Q_PROPERTY(bool initialized READ isInitialized CONSTANT)
		Q_PROPERTY(bool wSSubscriptionPolicySupport READ getWSSubscriptionPolicySupport CONSTANT)
		Q_PROPERTY(bool wSPullPointSupport READ getWSPullPointSupport CONSTANT)
		Q_PROPERTY(bool wSPausableSubscriptionManagerInterfaceSupport READ getWSPausableSubscriptionManagerInterfaceSupport CONSTANT)
		Q_PROPERTY(int maxNotificationProducers READ getMaxNotificationProducers CONSTANT)
		Q_PROPERTY(int maxPullPoints READ getMaxPullPoints CONSTANT)
		Q_PROPERTY(bool persistentNotificationStorage READ getPersistentNotificationStorage CONSTANT)
		Q_PROPERTY(QUrl serviceEndpoint READ getServiceEndpoint CONSTANT)

public:
	EventService();

	QUrl getServiceEndpoint() const;
	void setServiceEndpoint(const QUrl &val);
	bool isInitialized() const;
	void setInitialized(bool initialized);
	bool getWSSubscriptionPolicySupport() const;
	void setWSSubscriptionPolicySupport(bool val);
	bool getWSPullPointSupport() const;
	void setWSPullPointSupport(bool val);
	bool getWSPausableSubscriptionManagerInterfaceSupport() const;
	void setWSPausableSubscriptionManagerInterfaceSupport(bool val);
	int getMaxNotificationProducers() const;
	void setMaxNotificationProducers(int val);
	int getMaxPullPoints() const;
	void setMaxPullPoints(int val);
	bool getPersistentNotificationStorage() const;
	void setPersistentNotificationStorage(bool val);

private:

	QUrl serviceEndpoint;
	bool mInitialized;
	bool mWSSubscriptionPolicySupport;
	bool mWSPullPointSupport;
	bool mWSPausableSubscriptionManagerInterfaceSupport;
	int mMaxNotificationProducers;
	int mMaxPullPoints;
	bool mPersistentNotificationStorage;
};
