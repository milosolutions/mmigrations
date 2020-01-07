#include "dbmigrationsdata.h"

#include <QString>
#include <QList>

#include "dbhelpers.h"
#include "dbmigration.h"

//!!! Always remember to update current version for any db changes!
const QVersionNumber db::LATEST_DB_VERSION = { 0, 0, 2 };

//!!! Add migrations here
const QVector<db::Migration> db::DB_MIGRATIONS = {
    {
        { 0, 0, 1 },
        std::bind(&Helpers::runQueries, std::placeholders::_1, QList<QLatin1String>{
            QLatin1String("CREATE TABLE `Migrations` (`id` INTEGER primary key AUTOINCREMENT, `timestamp` INTEGER NOT NULL, `version` TEXT NOT NULL)"),
        }),
        std::bind(&Helpers::runQueries, std::placeholders::_1, QList<QLatin1String>{
            QLatin1String("DROP TABLE `Migrations`"),
        })
    },
    {
        { 0, 0, 2 },    // EXAMPLE DATA
        std::bind(&Helpers::runQueries, std::placeholders::_1, QList<QLatin1String>{
            QLatin1String("CREATE TABLE `User`  (`id`   INTEGER primary key UNIQUE,"
                                                "`name`		TEXT NOT NULL,"
                                                "`type`	INTEGER NOT NULL"
                                                ")"),
            QLatin1String("INSERT INTO `User`   (`name`, `type`)"
                                                "VALUES ('Super User', 0)"),
        }),
        std::bind(&Helpers::runQueries, std::placeholders::_1, QList<QLatin1String>{
            QLatin1String("DROP TABLE `User`")
        })
    },
};
