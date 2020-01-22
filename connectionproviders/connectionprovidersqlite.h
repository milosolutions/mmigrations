#ifndef DBCONNECTIONPROVIDERSQLITE_H
#define DBCONNECTIONPROVIDERSQLITE_H

#include "connectionproviderbase.h"

namespace mdatabase {
class ConnectionProviderSQLite : public ConnectionProviderBase
{
public:
    static mdatabase::ConnectionProviderSQLite &instance();

    virtual void setupConnectionData(const QString &databasePath,
                const QString &connectionName = QLatin1String(QSqlDatabase::defaultConnection));

    static bool databaseExist(const QString &databasePath);
    static bool createDatabase(const QString &databasePath);

protected:
    ConnectionProviderSQLite();
    ConnectionProviderSQLite(const QString& type);
};
}

#endif // DBCONNECTIONPROVIDERBASE_H
