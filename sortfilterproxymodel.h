#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProxyModel(QObject *parent = 0);
    void setFilterStartsWith(const QString& str);

protected:
    virtual bool filterAcceptsRow (int sourceRow, const QModelIndex & sourceParent ) const;
    
private:
    QString start;
};

#endif // SORTFILTERPROXYMODEL_H
