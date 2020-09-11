#include <QStringList>
#include <QSqlDriverPlugin>
#include <QDebug>
#include "qsql_sqlite_p.h"

class QSQLiteSeeDriverPlugin : public QSqlDriverPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QSqlDriverFactoryInterface" FILE "sqlitesee.json")

public:
    QSQLiteSeeDriverPlugin();

    QSqlDriver* create(const QString &) Q_DECL_OVERRIDE;
};

QSQLiteSeeDriverPlugin::QSQLiteSeeDriverPlugin()
    : QSqlDriverPlugin()
{
}

QSqlDriver* QSQLiteSeeDriverPlugin::create(const QString &name)
{
    if (name == QLatin1String("QSQLITESEE")) {
        QSQLiteDriver* driver = new QSQLiteDriver();
        qDebug() << "Activating QSQLITESEE plugin" << driver;
        return driver;
    }
    return 0;
}

#include "smain.moc"
