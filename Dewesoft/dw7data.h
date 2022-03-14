#ifndef DEWETRONDATA_H
#define DEWETRONDATA_H
#include <Qt>
#include <QString>
#include <QMap>
#include <QVariant>

struct DataEvent
{
  double time;
  QString reason;
  int type;
};


class DataChannel
{
public:
  DataChannel(){arraySize=1;}
  QString id;
  QString name;
  int arraySize;
};

//class DewetronData
//{

//  DewetronData(const QString &filename);
//public:
//  static DewetronData *open(const QString &filename);
//private:
//  bool m_ok;
//  bool m_opened;
//static bool m_initDeweLib;
//};

#endif // DEWETRONDATA_H
