#ifndef OPENER_H
#define OPENER_H
#include <QString>
#include "file.h"
namespace DataFile
{
  enum class Error;
  bool addOpener(const QString &key, const QString &description, const QStringList &extensions, const std::function<FilePtr(const QString &, Error *error)> &opener);
  class RegisterOpener
  {
  public:
    RegisterOpener(const QString &key, const QString &description, const QStringList &extensions, const std::function<FilePtr(const QString &, Error *error)> &opener)
    {
      m_success=addOpener(key, description, extensions, opener);
    }
  protected:
    bool m_success;
  };
}
#endif // OPENER_H
