#include "dbmigrationmanager.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

#include "dbhelpers.h"
#include "dbmigration.h"
#include "dbmigrationsdata.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(migrations)

template<class ConnectionProvider, typename Valid>
db::MigrationManager<ConnectionProvider, Valid>::MigrationManager(const QString &connectionName)
    : cDbConnectionName(connectionName)
{}

template<class ConnectionProvider, typename Valid>
void db::MigrationManager<ConnectionProvider, Valid>::loadVersion()
{
    mDbVersion = getVersionNumber();
}

template<class ConnectionProvider, typename Valid>
bool db::MigrationManager<ConnectionProvider, Valid>::needsUpdate()
{
    return (mDbVersion != LATEST_DB_VERSION);
}

template<class ConnectionProvider, typename Valid>
bool db::MigrationManager<ConnectionProvider, Valid>::update()
{
    if (!needsUpdate()) {
        qCInfo(migrations) << "Database up to date.";
        return true;
    }

    return updateDb();
}

template<class ConnectionProvider, typename Valid>
QVersionNumber db::MigrationManager<ConnectionProvider, Valid>::getVersionNumber() const
{
    static const QLatin1String VersionQuery = 
            QLatin1String("SELECT `version` from `Migrations` ORDER BY `id` DESC LIMIT 1");

    auto query = QSqlQuery(ConnectionProvider::instance().databaseConnection(cDbConnectionName));
    query.prepare(VersionQuery);
    db::Helpers::execQuery(query);
    if (!query.first()) {
        qCCritical(migrations) << "No version in migration table.";
        return {};
    }

    return QVersionNumber::fromString(query.value(0).toString());
}

template<class ConnectionProvider, typename Valid>
bool db::MigrationManager<ConnectionProvider, Valid>::updateDb()
{
    auto db = ConnectionProvider::instance().databaseConnection(cDbConnectionName);
    auto dbName = db.connectionName();

    if (mDbVersion > LATEST_DB_VERSION) {
        // backward
        return applyMigrations(DB_MIGRATIONS.rbegin(), DB_MIGRATIONS.rend(), 
            std::bind(Migration::RunBackward, std::placeholders::_1, db), false);
    }

    // forward
    return applyMigrations(DB_MIGRATIONS.begin(), DB_MIGRATIONS.end(), 
            std::bind(Migration::RunForward, std::placeholders::_1, db), true);
}

template<class ConnectionProvider, typename Valid>
template<typename It>
bool db::MigrationManager<ConnectionProvider, Valid>::applyMigrations(
                It begin, It end, 
                std::function<bool(const Migration &)> const &handler, bool forward)
{
    auto start = begin;
    if (!mDbVersion.isNull()) {
        start = findMigrationNumber(begin, end, mDbVersion);
        if (start == end) {
            qCCritical(migrations) << "Cannot update database - version missing.";
            return false;
        }
        start += (forward ? 1 : 0);
    }

    auto finish = findMigrationNumber(begin, end, LATEST_DB_VERSION);
    if (finish == end) {
        qCCritical(migrations) << "Cannot update database - latest version missing.";
        return false;
    }
    finish += (forward ? 1 : 0);

    return std::all_of(start, finish, handler);
}

template<class ConnectionProvider, typename Valid>
template<typename It>
It db::MigrationManager<ConnectionProvider, Valid>::findMigrationNumber(
                                It begin, It end, const QVersionNumber &number)
{
    auto item = std::find_if(begin, end, 
            [&number](const Migration &migration) {
                return (migration.number() == number); 
            });
    if (item == end) {
        qCCritical(migrations) << "Version not found in migrations! Version:" << number;
    }

    return item;
}

// teplate declarations
#include "connectionproviders/dbconnectionprovidersqlite.h"
template class db::MigrationManager<db::ConnectionProviderSQLite>;
