#ifndef DBMIGRATIONSDATA_H
#define DBMIGRATIONSDATA_H

#include <QVersionNumber>
#include <QVector>
#include <array>
namespace mdatabase {
    class Migration;


    class MIGRATIONS {
    public:
        static const QVersionNumber& latestDbVersion()
        {
            return sVersion;
        }

        static const QVector<Migration>& migrations()
        {
            return sMigrations;
        }

        static void init(const QVersionNumber& version, const QVector<Migration>& migrations)
        {
            sVersion = version;
            sMigrations = migrations;
        }

    private:
        static QVersionNumber sVersion;
        static QVector<Migration> sMigrations;
    };



}
#endif // DBMIGRATIONSDATA_H
