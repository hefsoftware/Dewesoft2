#include "dw7util.h"
#include <QDebug>
QString DW7Util::typeName(int type)
{
  switch(type)
  {
    case dtByte:
      return QObject::tr("Byte");
    case dtShortInt:
      return QObject::tr("Short int");
    case dtSmallInt:
      return QObject::tr("Small int");
    case dtWord:
      return QObject::tr("Word");
    case dtInteger:
      return QObject::tr("Integer");
    case dtSingle:
      return QObject::tr("Single");
    case dtInt64:
      return QObject::tr("Int64");
    case dtDouble:
      return QObject::tr("Double");
    case dtLongword:
      return QObject::tr("Longword");
    case dtComplexSingle:
      return QObject::tr("Complex single");
    case dtComplexDouble:
      return QObject::tr("Complex double");
    case dtText:
      return QObject::tr("Text");
    case dtBinary:
      return QObject::tr("Binary");
    case dtCANPortData:
      return QObject::tr("CAN port data");
    case dtCANFDPortData:
      return QObject::tr("CANFD port data");
    case dtBytes8:
      return QObject::tr("Bytes8");
    case dtBytes16:
      return QObject::tr("Bytes16");
    case dtBytes32:
      return QObject::tr("Bytes32");
    case dtBytes64:
      return QObject::tr("Bytes64");
    default:
      return QObject::tr("Type %1").arg(type);
  }
}

QByteArray DW7Util::stringToMultiByte(const QString &string)
{
  QByteArray ret;
  auto wString=string.toStdWString();
  int requiredSize=WideCharToMultiByte(CP_ACP,0,wString.c_str(),-1,0,0,0,0);
  ret.resize(requiredSize);
  BOOL defaultChars;
  requiredSize=WideCharToMultiByte(CP_ACP,0,wString.c_str(),-1,ret.data(),ret.size(),0,&defaultChars);
  if(defaultChars)
    qDebug()<<__FILE__<<__LINE__<<"Fault default Chars";
  return ret;
}

QString DW7Util::statusToString(DWStatus status)
{
  switch(status)
  {
  case DWSTAT_OK:									// status OK
    return QObject::tr("Dewesoft ok");
  case DWSTAT_ERROR: 								// error occurred in the dll
    return QObject::tr("Generic dewesoft error");
  case DWSTAT_ERROR_FILE_CANNOT_OPEN:				// unable to open file
    return QObject::tr("Dewesoft can't open file");
  case DWSTAT_ERROR_FILE_ALREADY_IN_USE:			// file already in use
    return QObject::tr("Dewesoft file already in use");
  case DWSTAT_ERROR_FILE_CORRUPT: 					// file corrupted
    return QObject::tr("Dewesoft file corrupted");
  case DWSTAT_ERROR_NO_MEMORY_ALLOC:				// memory not allocated
    return QObject::tr("Dewesoft can't allocate memory");
  case DWSTAT_ERROR_CREATE_DEST_FILE:				// error creating destination file (only for d7z files)
    return QObject::tr("Dewesoft create destination file");
  case DWSTAT_ERROR_EXTRACTING_FILE:				// error extracting data (only for d7z files)
    return QObject::tr("Dewesoft create extracting file");
  case DWSTAT_ERROR_CANNOT_OPEN_EXTRACTED_FILE:   		// error opening extracted file (only for d7z files)
    return QObject::tr("Dewesoft can't open extracted file");
  default:
    return QObject::tr("Dewesoft reported error: %1").arg((int)status);
  }
}

DataFile::Error DW7Util::statusToError(DWStatus status)
{
  switch(status)
  {
  case DWSTAT_OK:									// status OK
    return DataFile::Error::Success;
  case DWSTAT_ERROR: 								// error occurred in the dll
  default:
    return DataFile::Error::Generic;
  case DWSTAT_ERROR_FILE_CANNOT_OPEN:				// unable to open file
  case DWSTAT_ERROR_FILE_ALREADY_IN_USE:			// file already in use
  case DWSTAT_ERROR_FILE_CORRUPT: 					// file corrupted
  case DWSTAT_ERROR_CREATE_DEST_FILE:				// error creating destination file (only for d7z files)
  case DWSTAT_ERROR_EXTRACTING_FILE:				// error extracting data (only for d7z files)
  case DWSTAT_ERROR_CANNOT_OPEN_EXTRACTED_FILE:   		// error opening extracted file (only for d7z files)
    return DataFile::Error::Disk;
  case DWSTAT_ERROR_NO_MEMORY_ALLOC:				// memory not allocated
    return DataFile::Error::Memory;
  }
}
