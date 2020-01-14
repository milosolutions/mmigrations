MMigrations
===

[Online documentation](https://docs.milosolutions.com/milo-code-db/mmigrations)

[Source code](https://github.com/milosolutions/mmigrations)

# Getting started

MMigraions is a library that helps to manage database migrations.

For example usage, see example project. You can also check doxygen docs.

### Setting up

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

Migration is a simple class which contains number and 2 functions performing 
forward and backward migration. You can initialize it this way if you need full
control:
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

Or you can use this fluent builder if all you need is to set queries:
```c++
MigrationBuilder::builder()
    .setVersion("0.0.3")
    .addForwardQuery("CREATE TABLE `Fun`  ("
                        "`id` INTEGER primary key UNIQUE,"
                        "`name` TEXT NOT NULL,"
                        "`type` INTEGER NOT NULL"
                        ")")
    .addBackwardQuery("DROP TABLE `Fun`")
    .build()
```

# License

This project is licensed under the MIT License - see the LICENSE-MiloCodeDB.txt file for details