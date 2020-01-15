#include "dbmigrationsdata.h"

#include <QString>
#include <QList>

#include "dbhelpers.h"
#include "dbmigration.h"

//!!! Always remember to update current version for any db changes!
const QVersionNumber db::LATEST_DB_VERSION = { 0, 0, 4 };

//!!! Add migrations here
const QVector<db::Migration> db::DB_MIGRATIONS = {
    // this static method returns very first migration
    // which creates Migrations table with version and a timestamp
    MigrationBuilder::migration001()
    ,

    // Here is an example of 'raw' migration. It is simply a version number
    // and a two functions (one for forward, second for backward migrations)
    // functions takes db as a argument
    {
        { 0, 0, 2 },
        std::bind(&Helpers::runQueries, std::placeholders::_1, db::Helpers::Queries {
            QLatin1String("CREATE TABLE `User`  (`id`   INTEGER primary key UNIQUE,"
                                                "`name`		TEXT NOT NULL,"
                                                "`type`	INTEGER NOT NULL"
                                                ")"),
            QLatin1String("INSERT INTO `User`   (`name`, `type`)"
                                                "VALUES ('Super User', 0)"),
            QLatin1String("INSERT INTO `User`   (`name`, `type`)"
                                                "VALUES ('Login', 0)"),
        }),
        std::bind(&Helpers::runQueries, std::placeholders::_1, db::Helpers::Queries {
            QLatin1String("DROP TABLE `User`")
        })
    },

    // Next migration example (with builder). It has simpler syntax
    // and some basic error checs.
    MigrationBuilder::builder()
        // sets version (could be also QVersionNumber)
        .setVersion("0.0.3")
        // set of forward queries
        .addForwardQuery(" CREATE TABLE `Fun`  (                 "
                         "     `id` INTEGER primary key UNIQUE,  "
                         "     `name` TEXT NOT NULL,             "
                         "     `type` INTEGER NOT NULL           "
                         " )                                     ")
        .addForwardQuery(" INSERT INTO `Fun`   (`name`, `type`)  "
                         "        VALUES ('Super Fun', 0)        ")
        // one backward query
        .addBackwardQuery("DROP TABLE `Fun`")
        // creates Migration object
        .build()
    ,
    // Another example (with setForward/BackwardQueries) ---
    MigrationBuilder::builder()
        .setVersion(QVersionNumber(0, 0, 4))
        .setForwardQueries( {
           QLatin1String(" CREATE TABLE `Fun`  (                "
                         "     `id` INTEGER primary key UNIQUE, "
                         "     `name` TEXT NOT NULL,            "
                         "     `type` INTEGER NOT NULL          "
                         " )"),
            QLatin1String("INSERT INTO `Fun`   (`name`, `type`)"
                         "VALUES ('Super Fun', 0)") } )
        .setBackwardQueries( {
            QLatin1String("DROP TABLE `Fun`") })
        .build()
};
