#include "sortfilterproxymodel.h"

#include <QCoreApplication>

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

void SortFilterProxyModel::setFilterStartsWith(const QString &str)
{
    start = str;
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    return sourceModel()->data(index).toString().startsWith(start);
}
