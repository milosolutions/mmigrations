#ifndef DBMIGRATION_H
#define DBMIGRATION_H

#include <QVersionNumber>
#include <functional>
#include <QVector>

#include "dbhelpers.h"

class QSqlDatabase;

namespace mdatabase {
class Migration final {
public:
    Migration(const QVersionNumber &mNumber,
              const std::function<bool(QSqlDatabase &)> &mForward,
              const std::function<bool(QSqlDatabase &)> &mBackward);

    static bool RunForward(const Migration &m, QSqlDatabase &mdatabase);
    static bool RunBackward(const Migration &m, QSqlDatabase &mdatabase);

    QVersionNumber number() const;

private:
    QVersionNumber mNumber;
    std::function<bool(QSqlDatabase &)> mForward;
    std::function<bool(QSqlDatabase &)> mBackward;

    void runCommon(QSqlDatabase &mdatabase) const;
};


class MigrationBuilder
{
public:
    static Migration migration001();
    static MigrationBuilder builder();
    Migration build();
    MigrationBuilder&& setVersion(const QVersionNumber& version);
    MigrationBuilder&& setVersion(const QString& version);
    MigrationBuilder&& addForwardQuery(const QLatin1String& query);
    MigrationBuilder&& addForwardQuery(const char* query);
    MigrationBuilder&& addBackwardQuery(const QLatin1String& query);
    MigrationBuilder&& addBackwardQuery(const char* query);
    MigrationBuilder&& setForwardQueries(const Helpers::Queries& queries);
    MigrationBuilder&& setBackwardQueries(const Helpers::Queries& queries);

private:
    MigrationBuilder() = default;
    Helpers::Queries mForward;
    Helpers::Queries mBackward;
    QVersionNumber mVersion;
};

}
#endif // DBMIGRATION_H
