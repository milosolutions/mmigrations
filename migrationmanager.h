#ifndef DBMIGRATIONMANAGER_H
#define DBMIGRATIONMANAGER_H

#include <QSqlDatabase>
#include <QVersionNumber>
#include <functional>
#include <type_traits>

#include "connectionproviders/dbconnectionproviderbase.h"

#include "dbabstractmigrationmanager.h"

template <typename T, typename = void>
struct has_instance_method : std::false_type {};

template <typename T>
struct has_instance_method<T, 
           std::void_t<decltype(T::instance()) >> : std::true_type {};

namespace mdatabase {
class Migration;

template<class ConnectionProvider, typename Valid = std::enable_if_t<
            std::is_base_of<ConnectionProviderBase, ConnectionProvider>::value
            && has_instance_method<ConnectionProvider>::value>>
class MigrationManager : public AbstractMigrationManager
{
public:
    MigrationManager(const QString &connectionName = 
                              QLatin1String(QSqlDatabase::defaultConnection));

    void loadVersion();

    bool needsUpdate();
    bool update();

    void setupDatabase() override;

    const ConnectionProvider& provider() const {
        return ConnectionProvider::instance();
    }

protected:
    const QString cDbConnectionName;

    QVersionNumber mDbVersion;

    QVersionNumber getVersionNumber() const;
    bool updateDb();

    template<typename It>
    It findMigrationNumber(It begin, It end, const QVersionNumber &number);

    template<typename It>
    bool applyMigrations(It begin, It end, 
            std::function<bool(const Migration &)> const &handler, bool forward);
};
}



// Migrations Manager implementation
#include <QSqlQuery>
#include <QSqlError>

#include "dbhelpers.h"
#include "dbmigration.h"
#include "dbmigrationsdata.h"
#include <QLoggingCategory>
#include <QtConcurrent/QtConcurrent>

Q_DECLARE_LOGGING_CATEGORY(migrations)
namespace mdatabase {
// Migration manager implementation
template<class ConnectionProvider, typename Valid>
MigrationManager<ConnectionProvider, Valid>::MigrationManager(const QString &connectionName)
    : cDbConnectionName(connectionName)
{}

template<class ConnectionProvider, typename Valid>
void MigrationManager<ConnectionProvider, Valid>::loadVersion()
{
    mDbVersion = getVersionNumber();
}

template<class ConnectionProvider, typename Valid>
bool MigrationManager<ConnectionProvider, Valid>::needsUpdate()
{
    return (mDbVersion != LATEST_DB_VERSION);
}

template<class ConnectionProvider, typename Valid>
bool MigrationManager<ConnectionProvider, Valid>::update()
{
    if (!needsUpdate()) {
        qCInfo(migrations) << "Database up to date.";
        return true;
    }

    return updateDb();
}

template<class ConnectionProvider, typename Valid>
void MigrationManager<ConnectionProvider, Valid>::setupDatabase()
{
    Q_ASSERT_X(!mSetupDone, Q_FUNC_INFO, "Trying to setup database twice");
    if (!mSetupDone) {
        loadVersion();
        if (needsUpdate()) {
            emit databaseUpdateStarted();

            mMigrationRunner = QtConcurrent::run(
                std::bind(&MigrationManager::update, this));
            mMigrationProgress.setFuture(mMigrationRunner);
        } else {
            emit databaseReady();
        }

        mSetupDone = true;
    }
}

template<class ConnectionProvider, typename Valid>
QVersionNumber MigrationManager<ConnectionProvider, Valid>::getVersionNumber() const
{
    static const QLatin1String VersionQuery = 
            QLatin1String("SELECT `version` from `Migrations` ORDER BY `id` DESC LIMIT 1");

    auto query = QSqlQuery(provider().databaseConnection(cDbConnectionName));
    query.prepare(VersionQuery);
    mdatabase::Helpers::execQuery(query);
    if (!query.first()) {
        qCCritical(migrations) << "No version in migration table.";
        return {};
    }

    return QVersionNumber::fromString(query.value(0).toString());
}

template<class ConnectionProvider, typename Valid>
bool MigrationManager<ConnectionProvider, Valid>::updateDb()
{
    auto db = provider().databaseConnection(cDbConnectionName);
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
bool MigrationManager<ConnectionProvider, Valid>::applyMigrations(
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
It MigrationManager<ConnectionProvider, Valid>::findMigrationNumber(
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
}
#endif // DBMIGRATIONMANAGER_H
