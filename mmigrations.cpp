#include "mmigrations.h"

#include <QFuture>
#include <QLoggingCategory>
#include <QStandardPaths>

Q_DECLARE_LOGGING_CATEGORY(migrations)

//extern template class db::MigrationManager<db::ConnectionProviderSQLite>;

const QLatin1String MMigrationsBase::scDbName = QLatin1String("local.db");

MMigrationsBase::MMigrationsBase(QObject *parent)
    : QObject(parent), cDbPath(QStandardPaths::writableLocation(
                            QStandardPaths::AppDataLocation) + "/" + scDbName)
{
    connect(&mMigrationProgress, &QFutureWatcher<bool>::finished,
            this, [this]() {
        if (!mMigrationProgress.result()) {
            emit databaseUpdateError();
        }

        emit databaseReady();
    });
}

/*void MMigrationsBase::setupDatabase()
{
    Q_ASSERT_X(!mSetupDone, Q_FUNC_INFO, "Trying to setup database twice");
    if (!mSetupDone) {
        db::ConnectionProviderSQLite::instance().setupConnectionData(cDbPath);
        qCDebug(migrations) << "Database path:" << cDbPath;

        mMigrationManager.loadVersion();
        if (mMigrationManager.needsUpdate()) {
            emit databaseUpdateStarted();

            mMigrationRunner = QtConcurrent::run(
                std::bind(&MigrationManager::update, &mMigrationManager));
            mMigrationProgress.setFuture(mMigrationRunner);
        } else {
            emit databaseReady();
        }

        mSetupDone = true;
    }
}*/
