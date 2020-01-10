#include "dbmigrationmanager.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

#include "dbhelpers.h"
#include "dbmigration.h"
#include "dbmigrationsdata.h"

template<class ConnectionProvider, typename Valid>
db::MigrationManager<ConnectionProvider, Valid>::MigrationManager(const QString &connectionName)
    : c_dbConnectionName(connectionName)
{}

template<class ConnectionProvider, typename Valid>
void db::MigrationManager<ConnectionProvider, Valid>::loadVersion()
{
    m_dbVersion = getVersionNumber();
}

template<class ConnectionProvider, typename Valid>
bool db::MigrationManager<ConnectionProvider, Valid>::needsUpdate()
{
    return (m_dbVersion != LATEST_DB_VERSION);
}

template<class ConnectionProvider, typename Valid>
bool db::MigrationManager<ConnectionProvider, Valid>::update()
{
    if (!needsUpdate()) {
        qInfo() << "Database up to date.";
        return true;
    }

    return updateDb();
}

template<class ConnectionProvider, typename Valid>
QVersionNumber db::MigrationManager<ConnectionProvider, Valid>::getVersionNumber() const
{
    static const QLatin1String VersionQuery = QLatin1String("SELECT `version` from `Migrations` ORDER BY `id` DESC LIMIT 1");

    auto query = QSqlQuery(ConnectionProvider::instance().databaseConnection(c_dbConnectionName));
    query.prepare(VersionQuery);
    db::Helpers::execQuery(query);
    if (!query.first()) {
        qCritical() << "No version in migration table.";
        return {};
    }

    return QVersionNumber::fromString(query.value(0).toString());
}

template<class ConnectionProvider, typename Valid>
bool db::MigrationManager<ConnectionProvider, Valid>::updateDb()
{
    auto db = ConnectionProvider::instance().databaseConnection(c_dbConnectionName);
    auto dbName = db.connectionName();

    if (m_dbVersion > LATEST_DB_VERSION) {
        // backward
        return applyMigrations(DB_MIGRATIONS.rbegin(), DB_MIGRATIONS.rend(), std::bind(Migration::RunBackward, std::placeholders::_1, db), false);
    }

    // forward
    return applyMigrations(DB_MIGRATIONS.begin(), DB_MIGRATIONS.end(), std::bind(Migration::RunForward, std::placeholders::_1, db), true);
}

template<class ConnectionProvider, typename Valid>
template<typename It>
bool db::MigrationManager<ConnectionProvider, Valid>::applyMigrations(It begin, It end, std::function<bool(const Migration &)> const &handler, bool forward)
{
    auto start = begin;
    if (!m_dbVersion.isNull()) {
        start = findMigrationNumber(begin, end, m_dbVersion);
        if (start == end) {
            qCritical() << "Cannot update database - version missing.";
            return false;
        }
        start += (forward ? 1 : 0);
    }

    auto finish = findMigrationNumber(begin, end, LATEST_DB_VERSION);
    if (finish == end) {
        qCritical() << "Cannot update database - latest version missing.";
        return false;
    }
    finish += (forward ? 1 : 0);

    return std::all_of(start, finish, handler);
}

template<class ConnectionProvider, typename Valid>
template<typename It>
It db::MigrationManager<ConnectionProvider, Valid>::findMigrationNumber(It begin, It end, const QVersionNumber &number)
{
    auto item = std::find_if(begin, end, [&number](const Migration &migration) { return (migration.number() == number); });
    if (item == end) {
        qCritical() << "Version not found in migrations! Version:" << number;
    }

    return item;
}

// teplate declarations
#include "connectionproviders/dbconnectionprovidersqlite.h"
template class db::MigrationManager<db::ConnectionProviderSQLite>;
