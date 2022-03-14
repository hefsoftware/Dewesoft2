#ifndef ENTRY_H
#define ENTRY_H
#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QDebug>
namespace DataFile
{
  class Entry;
  enum class Error;
  typedef QSharedPointer<Entry> EntryPtr;
  typedef QSharedPointer<QObject> DataPtr;
  class Entry: public QObject, public QEnableSharedFromThis<Entry>
  {
    Q_OBJECT
  public:
    typedef QString Key;
    typedef QString INode;
    typedef QMap<Key, INode> Children;
    Entry(EntryPtr parent);
    inline EntryPtr sharedEntry() { EntryPtr ret; ret=sharedFromThis().staticCast<Entry>(); if(!ret)ret=EntryPtr(this); return ret; }
    virtual Children children()=0;
    EntryPtr open(const QString &path, Error *error=nullptr);
    virtual EntryPtr openKey(const Key &key);
    virtual EntryPtr openINode(const INode &index)=0;
    EntryPtr openINode(const QList<INode> &list);
    virtual DataPtr data();
    virtual QMap<QString, QVariant> metadata();
    static QString iNodeJoin(const QList<INode> &inodes);
    static QString iNodeJoin(const QString &path, INode inode);
    static QList<INode> iNodeSplit(const QString &path);
  // Path string utilities

  protected:
    QSharedPointer<Entry> m_parent;
  };
}

#endif // ENTRY_H
