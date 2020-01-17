MMigrations
===

[Online documentation](https://docs.milosolutions.com/milo-code-db/mmigrations)

[Source code](https://github.com/milosolutions/mmigrations)

# Getting started

MMigraions is a library that helps to manage database migrations.

For example usage, see example project. You can also check doxygen docs.

# Setting up migrations

To set up migrations you have to create two global variables containing:
* version
* migrations list
Here is an example definition for each one:
```c++
namespace db {
    const QVersionNumber LATEST_DB_VERSION = { 0, 0, 3 };
    const QVector<db::Migration> DB_MIGRATIONS { 
        /* Migration1, Migration2 ... */ 
    };
}
```

First migration should contain MIGRATIONS table with timestamp and a migration 
number. Luckly there is a static method to create it:
```c++
const QVector<db::Migration> db::DB_MIGRATIONS = {
    // very first migration can be created this way:
    MigrationBuilder::migration001()
    // next migrations goes here 
     , Migration2, Migration3.... , MigrationN
}
```


Migration is a simple class which contains number and 2 functions performing 
forward and backward migration. You can initialize it this way using fluent 
builder if all you need is to set queries:
```c++
MigrationBuilder::builder()
    .setVersion("0.0.3")
    .addForwardQuery(" CREATE TABLE `Fun`  (                "
                     "    `id` INTEGER primary key UNIQUE,  "
                     "    `name` TEXT NOT NULL,             "
                     "    `type` INTEGER NOT NULL           "
                     " )")
    .addBackwardQuery("DROP TABLE `Fun`")
    .build()
```

if you need full control you can initialize migration like that:
```c++
{
    { 0, 0, 2 },    // EXAMPLE DATA
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
```



# Migrations manager
To use migration manager you need to write your db connection provider or use 
available ones (currently only sqlite, it can be found in connectionproviders 
folder). Next step is to create Migration manager, connect to its signals
and invoke setupDatabase, like in this example:

```c++
// SQlite migrations manager 
using SqliteMigrations = db::MigrationManager<db::ConnectionProviderSQLite>;
SqliteMigrations dbManager;
// setting up SQLite db path (method comes from ConnectionProvederSQLite)
dbManager.setupConnectionData(QStandardPaths::writableLocation(
                            QStandardPaths::AppDataLocation) + "/local.db");

// connecting to migration manager signals
QObject::connect(&dbManager, &SqliteMigrations::databaseUpdateStarted,
                    &app, []{ qInfo() << "Database update started!"; });
QObject::connect(&dbManager, &SqliteMigrations ::databaseUpdateError,
                    &app, []{ qInfo() << "Database update error!"; });
QObject::connect(&dbManager, &SqliteMigrations::databaseReady,
                    &app, []{qCInfo(migrations) << "Database ready!"; });

// setting up database, performing migrations if needed
dbManager.setupDatabase();
```



# License

This project is licensed under the MIT License - see the LICENSE-MiloCodeDB.txt file for details.
