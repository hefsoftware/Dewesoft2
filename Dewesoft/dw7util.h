#ifndef DW7UTIL_H
#define DW7UTIL_H
#include <QObject>
#include "DWLoadLib.h"
#include "../Datafile/error.h"
namespace DW7Util
{
  QString typeName(int type);
  QByteArray stringToMultiByte(const QString &string);
  QString statusToString(DWStatus status);
  DataFile::Error statusToError(DWStatus status);
}

#endif // DW7UTIL_H
