#include "databasemanager.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

extern template class db::MigrationManager<db::ConnectionProviderSQLite>;

const QLatin1String DatabaseManager::scDbName = QLatin1String("local.db");

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), cDbPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + scDbName)
{
    connect(&mMigrationProgress, &QFutureWatcher<bool>::finished,
            this, [this]() {
        if (!mMigrationProgress.result()) {
            emit databaseUpdateError();
        }

        emit databaseReady();
    });
}

void DatabaseManager::setupDatabase()
{
    Q_ASSERT_X(!mSetupDone, __PRETTY_FUNCTION__, "Trying to setup database twice");
    if (!mSetupDone) {
        db::ConnectionProviderSQLite::instance().setupConnectionData(cDbPath);
        qDebug() << "Database path:" << cDbPath;

        mMigrationManager.loadVersion();
        if (mMigrationManager.needsUpdate()) {
            emit databaseUpdateStarted();

            mMigrationRunner = QtConcurrent::run(std::bind(&MigrationManager::update, &mMigrationManager));
            mMigrationProgress.setFuture(mMigrationRunner);
        } else {
            emit databaseReady();
        }

        mSetupDone = true;
    }
}
