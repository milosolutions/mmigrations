#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QFutureWatcher>

#include "connectionproviders/dbconnectionprovidersqlite.h"
#include "dbmigrationmanager.h"

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    using ConnectionProvider = db::ConnectionProviderSQLite;
    explicit DatabaseManager(QObject *parent = nullptr);

    void setupDatabase();

signals:
    void databaseReady();

    void databaseUpdateStarted() const;
    void databaseUpdateError() const;

private:
    using MigrationManager = db::MigrationManager<ConnectionProvider>;

    static const QLatin1String scDbName;
    const QString cDbPath;

    bool mSetupDone = false;

    MigrationManager mMigrationManager;
    QFuture<bool> mMigrationRunner;
    QFutureWatcher<bool> mMigrationProgress;

    void onMigrationFinished(bool result);
};

#endif // DATABASEMANAGER_H
