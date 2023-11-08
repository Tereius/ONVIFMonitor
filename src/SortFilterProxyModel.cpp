#include "SortFilterProxyModel.h"
#include <QDebug>
#include <QDateTime>
#include <QJSEngine>
#include <QCoreApplication>
#include <QRegularExpression>
#include <cstdint>

bool aboutToQuit = false;

SortFilterProxyModel::SortFilterProxyModel(QObject *parent /*= nullptr*/) : QSortFilterProxyModel(parent), mHasFunctor(false) {

	connect(this, &SortFilterProxyModel::rowsInserted, this, &SortFilterProxyModel::countChanged);
	connect(this, &SortFilterProxyModel::rowsRemoved, this, &SortFilterProxyModel::countChanged);
	connect(this, &SortFilterProxyModel::modelReset, this, &SortFilterProxyModel::countChanged);
	connect(qApp, &QCoreApplication::aboutToQuit, this, []() { aboutToQuit = true; });
}

SortFilterProxyModel::~SortFilterProxyModel() {

	if(!aboutToQuit)
		deleteJsFunctor(); // TODO: Workaround - Leads to segfault if the dtor is called in the process of QQmlEngine getting destroyed
}

void SortFilterProxyModel::setSortOrder(SortOrder sortOrder) {

	m_sortOrder = sortOrder;
	switch(m_sortOrder) {
		case SORT_ascending:
			sort(0, Qt::AscendingOrder);
			break;
		case SORT_descending:
			sort(0, Qt::DescendingOrder);
			break;
		default:
			break;
	}
	emit sortOrderChanged();
}

void SortFilterProxyModel::setFilterFunctor(QJSValue functor) {

	mHasFunctor = setJsFunctor(functor);
	invalidateFilter();
}

void SortFilterProxyModel::unsetFilterFunctor() {

	deleteJsFunctor();
}

QJSValue SortFilterProxyModel::getFilterFunctor() const {

	return getJsFunctor();
}

SortFilterProxyModel::SortOrder SortFilterProxyModel::getSortOrder() {

	return m_sortOrder;
}

void SortFilterProxyModel::setChildModel(QAbstractItemModel *child) {

	setSourceModel(child);
	setSortCaseSensitivity(Qt::CaseInsensitive);
	setSortOrder(SORT_ascending);
	emit childModelChanged();
}

QAbstractItemModel *SortFilterProxyModel::getChildModel() const {

	return sourceModel();
}

void SortFilterProxyModel::setFilterExpr(const QString &expr) {

	setFilterRegularExpression(QRegularExpression(expr));
}

QString SortFilterProxyModel::getFilterExpr() const {

	return filterRegularExpression().pattern();
}

void SortFilterProxyModel::unsetFilterExpr() {

	setFilterRegularExpression(QRegularExpression(""));
}

bool SortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {

	QVariant leftData = sourceModel()->data(left, m_sortRole);
	QVariant rightData = sourceModel()->data(right, m_sortRole);

	// sort by other types and just fallback to qt implementation if they are equal
	switch(leftData.type()) {
		case QVariant::Type::Date:
			if(leftData.toDate() != rightData.toDate()) {
				return leftData.toDate() < rightData.toDate();
			}
		case QVariant::Type::DateTime:
			if(leftData.toDateTime() != rightData.toDateTime()) {
				return leftData.toDateTime() < rightData.toDateTime();
			}
		default:
			break;
	}

	// fallback to qt default implementation
	return QSortFilterProxyModel::lessThan(left, right);
}

QVariantMap SortFilterProxyModel::get(int row) {

	QHash<int, QByteArray> names = roleNames();
	QHashIterator<int, QByteArray> i(names);
	QVariantMap res;
	while(i.hasNext()) {
		i.next();
		QModelIndex idx = index(row, 0);
		QVariant data = idx.data(i.key());
		res[i.value()] = data;
	}
	return res;
}

QModelIndex SortFilterProxyModel::invokableMapToSource(const QModelIndex &proxyIndex) {

	return mapToSource(proxyIndex);
}

void SortFilterProxyModel::reload() {

    beginResetModel();
    endResetModel();
}

bool SortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {

	bool acceptsRow = true;
	auto engine = qjsEngine(this);
	if(mHasFunctor) {
		if(engine) {
			auto functor = getJsFunctor();
			if(functor.isCallable()) {
				if(auto source = sourceModel()) {
					QModelIndex index = source->index(source_row, 0, source_parent);
					acceptsRow = functor.call({engine->toScriptValue<QVariant>(index.data(filterRole()))}).toBool();
				}
			} else {
				qWarning() << "JS functor not callable";
			}
		} else {
			qWarning() << "Can't call JS functor. Missing JS engine";
		}
	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent) && acceptsRow;
}

QJSValue SortFilterProxyModel::getJsFunctor() const {

	auto engine = qjsEngine(this);
	if(engine) {
		auto globalObject = engine->globalObject();
		if(globalObject.hasProperty(getUniqueIdentifier())) {
			return globalObject.property(getUniqueIdentifier());
		}
	} else {
		qWarning() << "Can't get JS functor. Missing JS engine";
	}
	return QJSValue(QJSValue::UndefinedValue);
}

bool SortFilterProxyModel::setJsFunctor(const QJSValue &rFunctor) const {

	auto engine = qjsEngine(this);
	if(engine) {
		auto globalObject = engine->globalObject();
		if(!rFunctor.isNull() && !rFunctor.isUndefined()) {
			globalObject.setProperty(getUniqueIdentifier(), rFunctor);
			return true;
		} else {
			globalObject.deleteProperty(getUniqueIdentifier());
		}
	} else {
		qWarning() << "Can't set JS functor. Missing JS engine";
	}
	return false;
}

void SortFilterProxyModel::deleteJsFunctor() const {

	auto engine = qjsEngine(this);
	if(engine) {
		auto globalObject = engine->globalObject();
		globalObject.deleteProperty(getUniqueIdentifier());
	}
}

QString SortFilterProxyModel::getUniqueIdentifier() const {

	auto id = QString::fromLocal8Bit(staticMetaObject.className()) + QString::number(reinterpret_cast<std::uintptr_t>(this));
	return id;
}
