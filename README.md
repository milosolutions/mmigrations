MDatabase
===

[Online documentation](https://docs.milosolutions.com/milo-code-db/mmigrations)

[Source code](https://github.com/milosolutions/mmigrations)

# Getting started

MDatabase is a library that simplifies and improves database handling in Qt.
MDatabase contains extra SQL plugins (SQLiteCipher and SQLite SEE) and 
adds a module for automated handling of database migrations.

For example usage, see example project. You can also check doxygen docs.

# Setting up migrations

To set up migrations you have to create two  variables containing:
* version
* migrations list
There are helpers macros which makes whole process very simple. Here is an example
of basic setup:
```c++
#include "migrationsdata.h"

// ...

using namespace mdatabase;

CURRENT_MIGRATION_VERSION(0, 0, 4);
START_MIGRATIONS
    MigrationBuilder::migration001(),
        /* Migration2, Migration3 ... */ 
END_MIGRATIONS
```

First migration should contain MIGRATIONS table with timestamp and a migration 
number. Luckly there is a static method to create it:
```c++
    // very first migration can be created this way:
    MigrationBuilder::migration001()
    // next migrations goes here 
     , Migration2, Migration3.... , MigrationN
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


One step left to initialize migrations - setting up MIGRATIONS class. 
There is also helper macro for it. Just run it before any other migration operations, like this:

```c++
int main(int argc, char *argv[])
{
    // ...
    INIT_MIGRATIONS
    mdb::ConnectionProviderSQLite::instance().setupConnectionData(
                QStandardPaths::writableLocation(
                    QStandardPaths::AppDataLocation) + "/local.db");

    using SqliteMigrations = mdb::MigrationManager<mdb::ConnectionProviderSQLite>;
    SqliteMigrations dbManager;
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

# Database drivers

## SQLite

Default SQLite driver, coming from Qt. Use it for local cache and any data which
does not contain sensitive information.

If you need to store any privacy or security critical information, use either
QtSQLiteCipher or (better) SQLiteSee.

## QtSQLiteCipher

This is an external (third party) plugin for Qt, which encrypts database data.
It leaves DB scheme intact, so encrypted file can still be read by standard
SQLite reader GUIs.

For deployment instructions, see the README inside `sqlitecipher` directory.

## SQLiteSee (SQLite Encryption Extension)

Milo owns a license for [SQLite SEE](https://www.hwaci.com/sw/sqlite/see.html).
SEE is fully compatible with SQLite, it's developed by the same team, and it
encrypts all SQLite data. From privacy and security standpoint, this is the best
option you can take for your project.

MCDB provides a handy installer script for Qt plugin, and a wrapper which makes 
it very easy to enable SQLiteSee in any project.

**Due to the binary-incompatible nature of Qt's plugins, the plugin** 
**installation procedure has to be repeated for every Qt version you use.**

### Prerequsites

* amalgamated source code of SQLiteSee has to be put into `sqlitesee/sqlitesee-sources`. It should contain `sqlite3.h` and `sqlite3-see-aes256-ofb.c` files (other will be ignored)
* either Qt source code or working Internet connection
* `qmake` in `$PATH`. QSQLiteSee plugin will be installed for this Qt version

### Plugin installation

* `cd` into `sqlitesee`
* make sure you have right Qt version in `$PATH` by running `qmake -version`
* for Android, also make sure that `ANDROID_NDK_ROOT` is set, for example by using `env | grep ANDROID_NDK_ROOT`
* run the plugin installer: `./sqlitesee_install.sh`. If you have Qt source code 
somewhere, you can provide it as first argument. If not, the installer will 
automatically download it for you
* inspect the output to make sure all went well

Inside your Qt installation, you should now have `libqsqlitesee.so` plugin.

This process has to be repeated for each Qt installation you are using. Because 
Qt plugins are not binary compatible, you cannot manually copy the `.so` file 
to another Qt version. 

### Use SQLiteSee in app

Once the plugin is installed, all that is necessary to use it, is:
* use "QSQLITESEE" when setting up the database connection: `auto db = QSqlDatabase::addDatabase(                "QSQLITESEE", "someConnectionName");`
* provide encryption key (password): `QSqlQuery key(QStringLiteral("PRAGMA key='abcdefghjk1234567890qwertyuiopas'"), db);`
* the password should be 32 bytes long
* for maximum security, do not provide the password in the source code in clear 
text. Insttead, use a define and provide it from command line (or environment 
variable) only during compilation

# License

This project is licensed under the MIT License - see the LICENSE-MiloCodeDB.txt file for details.
