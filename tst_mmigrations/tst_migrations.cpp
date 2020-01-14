/*******************************************************************************
Copyright (C) 2017 Milo Solutions
Contact: https://www.milosolutions.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#include <QtTest>
#include <QCoreApplication>

#include "dbmigration.h"
#include "dbmigrationsdata.h"

namespace db {
    const QVersionNumber LATEST_DB_VERSION = { 0, 0, 3 };
    const QVector<db::Migration> DB_MIGRATIONS;
}


class TestMMigrations : public QObject
{
   Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testMigrationBuilder();
};

void TestMMigrations::initTestCase()
{
    QCoreApplication::setApplicationName("MMigrations Test");
    QCoreApplication::setOrganizationName("Milo");
}

void TestMMigrations::cleanupTestCase()
{
}

void TestMMigrations::testMigrationBuilder()
{
    db::Migration m = db::MigrationBuilder::builder()
        .setVersion("0.0.3")
        .build();

    
    QCOMPARE(m.number(), QVersionNumber(0, 0, 3));
}


QTEST_MAIN(TestMMigrations)

#include "tst_migrations.moc"