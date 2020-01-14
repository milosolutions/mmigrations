#ifndef DBMIGRATIONMANAGER_H
#define DBMIGRATIONMANAGER_H

#include <QSqlDatabase>
#include <QVersionNumber>
#include <functional>
#include <type_traits>

#include "connectionproviders/dbconnectionproviderbase.h"

namespace db {
class Migration;

template<class ConnectionProvider, typename Valid = std::enable_if_t<std::is_base_of<ConnectionProviderBase, ConnectionProvider>::value>>
class MigrationManager final
{
public:
    MigrationManager(const QString &connectionName = QLatin1String(QSqlDatabase::defaultConnection));

    void loadVersion();

    bool needsUpdate();
    bool update();

private:
    const QString c_dbConnectionName;

    QVersionNumber m_dbVersion;

    QVersionNumber getVersionNumber() const;
    bool updateDb();

    template<typename It>
    It findMigrationNumber(It begin, It end, const QVersionNumber &number);

    template<typename It>
    bool applyMigrations(It begin, It end, std::function<bool(const Migration &)> const &handler, bool forward);
};
}
#endif // DBMIGRATIONMANAGER_H
