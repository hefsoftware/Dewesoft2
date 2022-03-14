#include "entry.h"
#include "escape.h"
#include <QRegularExpression>


using namespace DataFile;

Entry::Entry(QSharedPointer<DataFile::Entry> parent): m_parent(parent)
{

}

EntryPtr Entry::open(const QString &path, Error *error)
{
  EntryPtr ret=sharedFromThis();

  auto split=splitUnescapePath(path);
  for(int i=0;ret && i<split.size();i++)
  {
    ret=ret->openKey(split[i]);
  }
  return ret;
}

EntryPtr Entry::openKey(const Entry::Key &key)
{
  auto c=children();
  EntryPtr ret;
  if(c.contains(key))
    ret=openINode(c[key]);
//  for(auto i=c.begin();i!=c.end();i++)
//  {
//    if(i.value()==key)
//    {
//      ret=openINode(i.key());
//      break;
//    }
//  }
  return ret;
}

EntryPtr Entry::openINode(const QList<Entry::INode> &list)
{
  EntryPtr ret=sharedEntry();
  if(!list.isEmpty())
  {
    for(int i=0;i<list.size() && ret;i++)
      ret=ret->openINode(list[i]);
  }
  return ret;
}

//QMap<Entry::INode, Entry::Key> Entry::children()
//{
//  return QMap<INode, Key>();
//}

//EntryPtr Entry::open(const INode &index)
//{
//  Q_UNUSED(index);
//  return EntryPtr();
//}

DataPtr Entry::data()
{
  return DataPtr();
}

QMap<QString, QVariant> Entry::metadata()
{
  return QMap<QString, QVariant>();
}

QString Entry::iNodeJoin(const QList<Entry::INode> &inodes)
{
  return joinPath(inodes);
}

QString Entry::iNodeJoin(const QString &path, Entry::INode inode)
{
  return joinPath(path, inode);
}

QList<Entry::INode> Entry::iNodeSplit(const QString &path)
{
  return splitPath(path);
}

