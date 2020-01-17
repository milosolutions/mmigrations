#ifndef DBMIGRATIONSDATA_H
#define DBMIGRATIONSDATA_H

#include <QVersionNumber>
#include <QVector>

namespace mdatabase {

class Migration;

extern const QVersionNumber LATEST_DB_VERSION;
extern const QVector<Migration> DB_MIGRATIONS;

}
#endif // DBMIGRATIONSDATA_H
