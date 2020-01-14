#ifndef DBCONNECTIONPROVIDERBASE_H
#define DBCONNECTIONPROVIDERBASE_H

#include <QSqlDatabase>
#include <QString>
#include <QHash>

namespace db {
class ConnectionProviderBase
{
public:
    ConnectionProviderBase(const QString &databaseType);
    virtual ~ConnectionProviderBase();

    bool hasDatabaseConnection(const QString &connectionName = QLatin1String(QSqlDatabase::defaultConnection)) const;
    QSqlDatabase databaseConnection(const QString &connectionName = QLatin1String(QSqlDatabase::defaultConnection)) const;

protected:
    QString mDatabaseType;

    QSqlDatabase createDatabaseConnection(const QString &connectionName) const;

    QString baseConnectionName(const QString &connectionName) const;
    QString extendedConnectionName(const QString &connectionName) const;
};
}

#endif // DBCONNECTIONPROVIDERBASE_H
