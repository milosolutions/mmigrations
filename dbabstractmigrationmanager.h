#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QFutureWatcher>

#include "connectionproviders/dbconnectionprovidersqlite.h"
//#include "dbmigrationmanager.h"


class MMigrationsBase : public QObject
{
    Q_OBJECT
public:
    explicit MMigrationsBase(QObject* parent = nullptr);

    virtual void setupDatabase() = 0;

signals:
    void databaseReady();

    void databaseUpdateStarted() const;
    void databaseUpdateError() const;

protected:
    static const QLatin1String scDbName;
    const QString cDbPath;

    bool mSetupDone = false;

    QFuture<bool> mMigrationRunner;
    QFutureWatcher<bool> mMigrationProgress;

    void onMigrationFinished(bool result);
};

#endif // DATABASEMANAGER_H
