#ifndef QMULTIKEYHASH_H
#define QMULTIKEYHASH_H

#include <QSharedPointer>
#include <QHash>

//**************************************************************************************************
//  Helpers
//**************************************************************************************************
template <qsizetype Index, typename AddKey, typename... AddKeys>
struct ExtractTypeAt
{
    using type = typename ExtractTypeAt<Index - 1, AddKeys...>::type;
};

template <typename AddKey, typename... AddKeys>
struct ExtractTypeAt<0, AddKey, AddKeys...>
{
    using type = AddKey;
};


//**************************************************************************************************
//  QMultiKeyData
//**************************************************************************************************
template <typename Key, typename Value>
class QMultiKeyData
{
public:
    explicit QMultiKeyData()
    {
    }

    void addCleaner(std::function<void(Key)> cleaner)
    {
        m_cleaners.append(cleaner);
    }

    qsizetype size() const
    {
        return m_hash.size();
    }

    QList<Key> keys() const
    {
        return m_hash.keys();
    }

    Value value(const Key &key) const
    {
        return m_hash.value(key);
    }

    bool insert(const Key &key, const Value &value)
    {
        m_hash.insert(key, value);
        return true;
    }

    bool remove(const Key &key)
    {
        for (auto &&cleaner : m_cleaners) {
            cleaner(key);
        }

        return m_hash.remove(key);
    }

    bool contains(const Key &key)
    {
        return m_hash.contains(key);
    }

private:
    QHash<Key, Value> m_hash;
    QList<std::function<void(Key)>> m_cleaners;
};


//**************************************************************************************************
//  QMultiKeyImpl
//**************************************************************************************************
template <qsizetype Index, typename Value, typename PrimKey, typename AddKey>
class QMultiKeyImpl
{
public:
    QMultiKeyImpl(const QSharedPointer<QMultiKeyData<PrimKey, Value>> &hash)
        : m_hash(hash)
    {
        m_hash->addCleaner([this](PrimKey key) {
            remove(m_keys.key(key));
        });
    }

    qsizetype size() const
    {
        return m_keys.size();
    }

    QList<AddKey> keys() const
    {
        return m_keys.keys();
    }

    Value value(const AddKey &addKey) const
    {
        const auto &primKey { m_keys.value(addKey) };
        return m_hash->value(primKey);
    }

    bool insert(const AddKey &addKey, const PrimKey &primKey)
    {
        if (!m_hash->contains(primKey)) {
            return false;
        }

        m_keys.insert(addKey, primKey);
        return true;
    }

    bool remove(const AddKey &addKey)
    {
        return m_keys.remove(addKey);
    }

private:
    QSharedPointer<QMultiKeyData<PrimKey, Value>> m_hash;
    QHash<AddKey, PrimKey> m_keys;
};

//**************************************************************************************************

template <typename Value, typename PrimKey, typename AddKey>
class QMultiKeyImpl<0, Value, PrimKey, AddKey>
{
public:
    QMultiKeyImpl(const QSharedPointer<QMultiKeyData<PrimKey, Value>> &hash)
        : m_hash(hash)
    {
    }

    qsizetype size() const
    {
        return m_hash->size();
    }

    QList<PrimKey> keys() const
    {
        return m_hash->keys();
    }

    Value value(const PrimKey &primKey) const
    {
        return m_hash->value(primKey);
    }

    bool insert(const PrimKey &primKey, const Value &value)
    {
        m_hash->insert(primKey, value);
        return true;
    }

    bool remove(const PrimKey &primKey)
    {
        return m_hash->remove(primKey);
    }

private:
    QSharedPointer<QMultiKeyData<PrimKey, Value>> m_hash;
};


//**************************************************************************************************
//  QMultiKeyRecurr
//**************************************************************************************************
template <qsizetype Index, typename Value, typename PrimKey, typename... AddKeys>
class QMultiKeyRecurr
{
public:
    QMultiKeyRecurr(const QSharedPointer<QMultiKeyData<PrimKey, Value>> &hash)
    {
    }
};

//**************************************************************************************************

template <qsizetype Index, typename Value, typename PrimKey, typename AddKey, typename... AddKeys>
class QMultiKeyRecurr<Index, Value, PrimKey, AddKey, AddKeys...>
    : public QMultiKeyImpl<Index, Value, PrimKey, AddKey>,
      public QMultiKeyRecurr<Index + 1, Value, PrimKey, AddKeys...>
{
public:
    QMultiKeyRecurr(const QSharedPointer<QMultiKeyData<PrimKey, Value>> &hash)
        : QMultiKeyImpl<Index, Value, PrimKey, AddKey>(hash),
          QMultiKeyRecurr<Index + 1, Value, PrimKey, AddKeys...>(hash)
    {
    }
};


//**************************************************************************************************
//  QMultiKeyHash (after C++23 will be posible to use non-terminate variadic template)
//**************************************************************************************************
template <typename Value, typename... Keys>
class QMultiKeyHash
    : public QSharedPointer<QMultiKeyData<typename ExtractTypeAt<0, Keys...>::type, Value>>,
      public QMultiKeyRecurr<0, Value, typename ExtractTypeAt<0, Keys...>::type, Keys...>
{
public:
    using PrimKey = typename ExtractTypeAt<0, Keys...>::type;

public:
    explicit QMultiKeyHash()
        : QSharedPointer<QMultiKeyData<PrimKey, Value>>(new QMultiKeyData<PrimKey, Value>()),
          QMultiKeyRecurr<0, Value, PrimKey, Keys...>(static_cast<QSharedPointer<QMultiKeyData<PrimKey, Value>>>(*this))
    {
        static_assert(sizeof...(Keys) > 0u, "too less argument");
        static_assert(std::is_default_constructible_v<Value>, "value must have default constructor");
    }

    template <qsizetype Index>
    auto &column()
    {
        return (static_cast<QMultiKeyImpl<Index, Value, PrimKey, typename ExtractTypeAt<Index, Keys...>::type> &>(*this));
    }

    template <qsizetype Index>
    const auto &column() const
    {
        return (static_cast<const QMultiKeyImpl<Index, Value, PrimKey, typename ExtractTypeAt<Index, Keys...>::type> &>(*this));
    }
};

#endif // QMULTIKEYHASH_H
