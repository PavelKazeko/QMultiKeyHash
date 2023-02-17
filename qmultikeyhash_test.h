#ifndef QMULTIKEYHASH_TEST_H
#define QMULTIKEYHASH_TEST_H

#include <QObject>
#include <QTest>

struct TestData {
    TestData()
    {
    }
    TestData(const QString &str, const QByteArray &arr = QByteArray())
        : str(str), arr(arr)
    {
    }

    friend QDebug operator<<(QDebug stream, const TestData &data) {
        stream << qPrintable(QStringLiteral("Str: %1").arg(data.str.isEmpty() ? "-" : data.str).leftJustified(14));
        stream << qPrintable(QStringLiteral("|"));
        stream << qPrintable(QStringLiteral("Arr: %2").arg(data.arr.isEmpty() ? "-" : QString::fromUtf8(data.arr.toHex()).leftJustified(14)));
        return stream;
    }

    QString str;
    QByteArray arr;
};

class QMultiKeyHashTest : public QObject
{
    Q_OBJECT
public:
    QMultiKeyHashTest(qsizetype iterationCount, QObject *parent = nullptr);

private slots:
    void benchStandardHash();
    void benchMultikeyHash();

private:
    const qsizetype m_iterationCount;
};

#endif // QMULTIKEYHASH_TEST_H
