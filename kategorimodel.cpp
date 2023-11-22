#include "kategorimodel.h"
#include "sqlhelper.h"

KategoriModel::KategoriModel(QObject *parent)
    : QSqlQueryModel{parent}
{
    refresh();
}

QHash<int, QByteArray> KategoriModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[KodeRole] = "kode";
    names[JenisRole] = "jenis";
    return names;
}

QVariant KategoriModel::data(const QModelIndex &item, int role) const
{
    QVariant value;

    if (item.isValid()) {
        if (role < Qt::UserRole) {
            value = QSqlQueryModel::data(item, role);
        } else {
            int columnIdx = role - Qt::UserRole;
            QModelIndex modelIndex = this->index(item.row(), columnIdx);
            value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
        }
    }
    return value;
}

void KategoriModel::refresh()
{
    QHash<QString, QVariant> bindMaps;
    QString queryString = "SELECT"
                          "   kd_kategori,"
                          "   nama_kategori "
                          "FROM Kategori";
    if (mTextQuery.length() > 0) {
        queryString.append(" WHERE nama_kategori LIKE :text_query");
        bindMaps[":text_query"] = "%" + mTextQuery + "%";
    }

    QSqlQuery query;
    query.prepare(queryString);

    SQLHelper::applyBindMaps(query, bindMaps);

    if (!query.exec())
        qFatal("Cannot query Kategori: %s", qPrintable(query.lastError().text()));

    setQuery(std::move(query));

    if (lastError().isValid()) {
        qFatal("Cannot move Kategori query: %s", qPrintable(lastError().text()));
    }
}

void KategoriModel::addNew(QString jenis)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Kategori("
               "   nama_kategori"
               ") VALUES ("
               "   :kategori"
               ")");

    query.bindValue(":kategori", jenis);

    if (!query.exec())
        qFatal() << "Cannot add Kategori " << query.lastError().text();

    refresh();
}

void KategoriModel::edit(int kode, QString jenis)
{
    QSqlQuery query;

    query.prepare("UPDATE Kategori SET "
                  " nama_kategori = :kategori "
                  "WHERE kd_kategori = :kode");

    query.bindValue(":kode", kode);
    query.bindValue(":kategori", jenis);

    if (!query.exec())
        qFatal() << "Cannot edit Kategori " << query.lastError().text();

    refresh();
}

void KategoriModel::remove(int kode)
{
    QSqlQuery query;

    query.prepare("DELETE FROM Kategori WHERE kd_kategori = :kode");

    query.bindValue(":kode", kode);

    if (!query.exec())
        qFatal() << "Cannot remove Kategori " << query.lastError().text();

    refresh();
}

int KategoriModel::getIndexByKode(int kode)
{
    int count = rowCount();
    for (int i = 0; i < count; i++) {
        if (data(index(i, 0), KodeRole).toInt() == kode) {
            return i;
        }
    }
    return -1;
}

QString KategoriModel::textQuery() const
{
    return mTextQuery;
}

void KategoriModel::setTextQuery(const QString &newTextQuery)
{
    if (mTextQuery == newTextQuery)
        return;
    mTextQuery = newTextQuery;
    refresh();
    emit textQueryChanged();
}
