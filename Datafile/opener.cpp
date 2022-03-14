#include "opener.h"
#include <QFileInfo>
#include <QDir>
#include "error.h"
#include <QDebug>
namespace DataFile
{


  namespace Opener
  {
    struct OpenerData
    {
      QString description;
      QStringList extensions;
      std::function<FilePtr(const QString &, Error *error)> opener;
    };
    QMap<QString, OpenerData> *openers=nullptr;
    QMap<QString, QString> *regExtensions=nullptr;
    QMap<QString, QWeakPointer<File> > openedFiles;
  }
}
using namespace DataFile;
using namespace Opener;
EntryPtr DataFile::open(const QString &filename, Error *error)
{
  EntryPtr ret;
  QStringList entries;
  QString fn(filename);
//  QString validName;
  for(int j=0;j<150;j++)
  {
    QFileInfo fi(fn);
    entries.append(fn);
    auto newName=fi.path();
    if(newName==fn)
      break;
    else
      fn=newName;
  }
  QString validName;
  QString subPath;
  for(int j=entries.size()-1;j>=0;j--)
  {
    auto curF=entries[j];
    QFileInfo fi(curF);
    if(fi.isFile())
    {
      if(j>0)
        subPath=filename.mid(curF.size()+1);
      validName=curF;
      break;
    }
    else if(!fi.isDir())
      break;
  }
  if(!validName.isEmpty())
  {
    ret=openFile(validName);
    qDebug()<<"Open"<<validName<<ret;
    if(ret && !subPath.isEmpty())
      ret=ret->open(subPath);
  }
  return ret;
}

FilePtr DataFile::openFile(const QString &filename, Error *error)
{
  QFileInfo fi(filename);
  auto absPath=fi.absoluteFilePath();
  FilePtr ret;
  if(error)
    *error=Error::Generic;
  ret=openedFiles.value(absPath).toStrongRef();
  if(!ret && openers && regExtensions)
  {
    QString key=regExtensions->value(fi.suffix().toLower());
    if(openers->contains(key))
    {
      auto op=openers->value(key).opener;
      if(op)
      {
        ret=op(absPath, error);
        if(ret)
          openedFiles.insert(absPath, ret);
      }
    }
  }
  return ret;
}


bool DataFile::addOpener(const QString &key, const QString &description, const QStringList &extensions, const std::function<FilePtr (const QString &, Error *)> &opener)
{
  bool ret=false;
  if(!openers)
    openers=new QMap<QString, OpenerData>;
  if(!regExtensions)
    regExtensions=new QMap<QString, QString>;

  if(!openers->contains(key))
  {
    OpenerData data;
    data.description=description;
    data.opener=opener;
    openers->insert(key, data);
    QStringList validExt;
    for(int i=0;i<extensions.size();i++)
    {
      auto e=extensions[i].toLower();
      if(regExtensions->contains(e))
        qWarning("Duplicated plugin for extension %s (%s and %s)", e.toUtf8().data(), key.toUtf8().data(), (*regExtensions)[key].toUtf8().data());
      else
      {
        validExt.append(e);
        regExtensions->insert(e, key);
      }
    }
    std::sort(validExt.begin(), validExt.end());
    data.extensions=validExt;
  }
  return ret;
}
