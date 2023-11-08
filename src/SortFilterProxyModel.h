#pragma once
#include <QJSValue>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QtQmlIntegration>

class SortFilterProxyModel : public QSortFilterProxyModel {

	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(QAbstractItemModel *model READ getChildModel WRITE setChildModel NOTIFY childModelChanged)
	Q_PROPERTY(SortOrder sortOrder READ getSortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
	Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
	Q_PROPERTY(QString filterExpr READ getFilterExpr WRITE setFilterExpr)
	Q_PROPERTY(QJSValue filterFunctor READ getFilterFunctor WRITE setFilterFunctor RESET unsetFilterFunctor)

 public:
	Q_ENUMS(SortOrder)
	enum SortOrder { SORT_ascending = 0, SORT_descending };

	~SortFilterProxyModel() override;
	explicit SortFilterProxyModel(QObject *parent = nullptr);
	void setFilterExpr(const QString &expr);
	QString getFilterExpr() const;
	void unsetFilterExpr();
	void setSortOrder(SortOrder sort);
	QJSValue getFilterFunctor() const;
	void setFilterFunctor(QJSValue functor);
	void unsetFilterFunctor();
	SortOrder getSortOrder();
	void setChildModel(QAbstractItemModel *child);
	QAbstractItemModel *getChildModel() const;
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
	Q_INVOKABLE QVariantMap get(int row);
	Q_INVOKABLE QModelIndex invokableMapToSource(const QModelIndex &proxyIndex);
	Q_INVOKABLE void reload();

 signals:
	void childModelChanged();
	void sortRoleChanged();
	void sortOrderChanged();
	void countChanged();

 private:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

	QJSValue getJsFunctor() const;
	bool setJsFunctor(const QJSValue &rFunctor) const;
	void deleteJsFunctor() const;
	QString getUniqueIdentifier() const;

	bool mHasFunctor;
	int m_sortRole = 0;
	SortOrder m_sortOrder = SortOrder::SORT_ascending;
};
