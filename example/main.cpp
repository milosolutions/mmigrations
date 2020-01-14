#include <QCoreApplication>

#define AppVersion "0.0.1"
#define CompanyName "Milo Solutions"
#define AppName "MigrationsMCDB"

#include <QSqlQuery>

#include "databasemanager.h"
#include "dbhelpers.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(migrations)

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion(AppVersion);
    app.setOrganizationName(CompanyName);
    app.setApplicationName(AppName);

    DatabaseManager dbManager;
    QObject::connect(&dbManager, &DatabaseManager::databaseUpdateStarted,
                     &app, []{ qInfo() << "Database update started!"; });
    QObject::connect(&dbManager, &DatabaseManager::databaseUpdateError,
                     &app, []{ qInfo() << "Database update error!"; });
    QObject::connect(&dbManager, &DatabaseManager::databaseReady,
                     &app, []{
        qCInfo(migrations) << "Database ready!";

        // example usage
        auto dbConnection = DatabaseManager::ConnectionProvider::instance().databaseConnection();
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
