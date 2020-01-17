#ifndef DBCONNECTIONPROVIDERSQLITE_H
#define DBCONNECTIONPROVIDERSQLITE_H

#include "dbconnectionproviderbase.h"

namespace db {
class ConnectionProviderSQLite : public ConnectionProviderBase
{
public:
    static db::ConnectionProviderSQLite &instance();

    void setupConnectionData(const QString &databasePath, 
                const QString &connectionName = QLatin1String(QSqlDatabase::defaultConnection));

    static bool databaseExist(const QString &databasePath);
    static bool createDatabase(const QString &databasePath);

protected:
    ConnectionProviderSQLite();
};
}

#endif // DBCONNECTIONPROVIDERBASE_H
