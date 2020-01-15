#include <QCoreApplication>

#define AppVersion "0.0.1"
#define CompanyName "Milo Solutions"
#define AppName "MigrationsMCDB"

#include <QSqlQuery>

#include "dbmigrationmanager.h"
#include "dbhelpers.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(migrations)

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion(AppVersion);
    app.setOrganizationName(CompanyName);
    app.setApplicationName(AppName);

    using SqliteMigrations = db::MigrationManager<db::ConnectionProviderSQLite>;
    SqliteMigrations dbManager;
    QObject::connect(&dbManager, &SqliteMigrations ::databaseUpdateStarted,
                     &app, []{ qInfo() << "Database update started!"; });
    QObject::connect(&dbManager, &SqliteMigrations ::databaseUpdateError,
                     &app, []{ qInfo() << "Database update error!"; });
    QObject::connect(&dbManager, &SqliteMigrations ::databaseReady,
                     &app, [&]{
        qCInfo(migrations) << "Database ready!";

        // example usage
        auto dbConnection = dbManager.databaseConnection();
        auto query = QSqlQuery(dbConnection);
        query.prepare("SELECT * FROM `User`;");
        db::Helpers::execQuery(query);

        qCDebug(migrations) << "USERS:";
        while (query.next()) {
            auto userName = query.value(1).toString();
            qCDebug(migrations) << userName;
        }
    });

    dbManager.setupDatabase();

    return app.exec();
}
