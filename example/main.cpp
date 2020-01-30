#include <QCoreApplication>

#define AppVersion "0.0.1"
#define CompanyName "Milo Solutions"
#define AppName "MigrationsMCDB"

#include <QSqlQuery>
#include <QStandardPaths>

#include "migrationmanager.h"
#include "dbhelpers.h"
#include <QLoggingCategory>

#include <connectionproviders/connectionprovidersqlite.h>
#include <connectionproviders/connectionprovidersqlitecipher.h>

Q_DECLARE_LOGGING_CATEGORY(migrations)

namespace mdb = mdatabase;

void printUsers(const QSqlDatabase& connection)
{
    auto query = QSqlQuery(connection);
    query.prepare("SELECT * FROM `User`;");
    mdb::Helpers::execQuery(query);

    qCDebug(migrations) << "USERS:";
    while (query.next()) {
        auto userName = query.value(1).toString();
        qCDebug(migrations) << userName;
    }
}



int main(int argc, char *argv[])
{

    extern void dbmigrationsinit();
    dbmigrationsinit();
    QCoreApplication app(argc, argv);
    app.setApplicationVersion(AppVersion);
    app.setOrganizationName(CompanyName);
    app.setApplicationName(AppName);


    // **** SQLITE 3 ****
    qCDebug(migrations) << "==================================";
    qCDebug(migrations) << "SQLite3 example:";
    // setting up SQLite db path
    mdb::ConnectionProviderSQLite::instance().setupConnectionData(
                QStandardPaths::writableLocation(
                    QStandardPaths::AppDataLocation) + "/local.db");


    // SQlite migrations manager 
    using SqliteMigrations = mdb::MigrationManager<mdb::ConnectionProviderSQLite>;
    SqliteMigrations dbManager;
    QObject::connect(&dbManager, &SqliteMigrations::databaseUpdateStarted,
                     &app, []{ qInfo() << "Database update started!"; });
    QObject::connect(&dbManager, &SqliteMigrations::databaseUpdateError,
                     &app, []{ qInfo() << "Database update error!"; });
    QObject::connect(&dbManager, &SqliteMigrations::databaseReady,
                     &app, [&]{
                        qCInfo(migrations) << "Database ready!";
                        printUsers(dbManager.provider().databaseConnection());
                     });
    dbManager.setupDatabase();



    // **** SQLITE 3 Cipher ****

    qCDebug(migrations) << "==================================";
    qCDebug(migrations) << "SQlite3Cipher example:";

    auto& cipher= mdb::ConnectionProviderSQLiteCipher::instance();
    if (cipher.checkPluginAvailable()) {
        cipher.setPassword("secret");
        cipher.setupConnectionData(QStandardPaths::writableLocation
                               (QStandardPaths::AppDataLocation)+ "/localCipher.db");
        using SqliteCipherMigrations = mdb::MigrationManager<mdb::ConnectionProviderSQLiteCipher>;
        SqliteCipherMigrations cipherManager;
        QObject::connect(&cipherManager, &SqliteCipherMigrations::databaseUpdateStarted,
                         &app, []{ qInfo() << "Ciphered database update started!"; });
        QObject::connect(&cipherManager, &SqliteCipherMigrations::databaseUpdateError,
                         &app, []{ qInfo() << "Ciphered database update error!"; });
        QObject::connect(&cipherManager, &SqliteCipherMigrations::databaseReady,
                         &app, [&]{
                            qCInfo(migrations) << "Ciphered database ready!";
                            printUsers(cipherManager.provider().databaseConnection());
                         });
        cipherManager.setupDatabase();
    } else {
        qCCritical(migrations) << "SQLite3 Cipher Plugin was not found!!!"
                               << "Check mmigraions/scripts/build_sqlitecipherplugin.sh "
                                  "script for details";

    }

    return app.exec();
}
