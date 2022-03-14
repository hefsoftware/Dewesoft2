#ifndef FILE_H
#define FILE_H
#include "entry.h"
namespace DataFile
{
  enum class Error;
  class File: public Entry
  {
    Q_OBJECT
  public:
    File();
    QString filename() const;
  protected:
    QString m_filename;
    static void addFileLoader();
  };
  typedef QSharedPointer<File> FilePtr;
  FilePtr openFile(const QString &filename, Error *error=nullptr);
  EntryPtr open(const QString &filename, Error *error=nullptr);
}
#endif // FILE_H
