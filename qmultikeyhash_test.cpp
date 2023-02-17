#include "qmultikeyhash.h"
#include "qmultikeyhash_test.h"

QMultiKeyHashTest::QMultiKeyHashTest(qsizetype iterationCount, QObject *parent)
    : QObject(parent), m_iterationCount(iterationCount)
{
}

void QMultiKeyHashTest::benchStandardHash()
{
    QBENCHMARK {
        QString primKey {  };
        QByteArray addKey0 {  };
        qsizetype addKey1 { 0ll };
        TestData value {  };

        QHash<decltype(primKey), decltype(value)> hashPrimKey_Value;
        QHash<decltype(addKey0), decltype(primKey)> hashAddKey0_PrimKey;
        QHash<decltype(addKey1), decltype(primKey)> hashAddKey1_PrimKey;

        for (addKey1 = 0ll; addKey1 < m_iterationCount; ++addKey1) {
            primKey = { QString::number(addKey1) };
            addKey0 = { primKey.toUtf8() };
            value = { TestData(primKey, addKey0) };

            hashPrimKey_Value.insert(primKey, value);
            hashAddKey0_PrimKey.insert(addKey0, primKey);
            hashAddKey1_PrimKey.insert(addKey1, primKey);
        }

        for (addKey1 = 0ll; addKey1 < m_iterationCount; ++addKey1) {
            primKey = { QString::number(addKey1) };
            addKey0 = { primKey.toUtf8() };

            value = { hashPrimKey_Value.value(primKey) };
            value = { hashPrimKey_Value.value(hashAddKey0_PrimKey.value(addKey0)) };
            value = { hashPrimKey_Value.value(hashAddKey1_PrimKey.value(addKey1)) };
        }
    }
}

void QMultiKeyHashTest::benchMultikeyHash()
{
    QBENCHMARK {
        QString primKey {  };
        QByteArray addKey0 {  };
        qsizetype addKey1 { 0ll };
        TestData value {  };

        QMultiKeyHash<decltype(value), decltype(primKey), decltype(addKey0), decltype(addKey1)> hash;

        for (addKey1 = 0ll; addKey1 < m_iterationCount; ++addKey1) {
            primKey = { QString::number(addKey1) };
            addKey0 = { primKey.toUtf8() };
            value = { TestData(primKey, addKey0) };

            hash.column<0>().insert(primKey, value);
            hash.column<1>().insert(addKey0, primKey);
            hash.column<2>().insert(addKey1, primKey);
        }

        for (addKey1 = 0ll; addKey1 < m_iterationCount; ++addKey1) {
            primKey = { QString::number(addKey1) };
            addKey0 = { primKey.toUtf8() };

            value = { hash.column<0>().value(primKey) };
            value = { hash.column<1>().value(addKey0) };
            value = { hash.column<2>().value(addKey1) };
        }
    }
}
