#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filewidget.h"
#include "Dewesoft/dw7datafile.h"
#include <QDebug>
#include <QFileInfo>
#include <QRegularExpression>
#include "Datafile/escape.h"
#include "Datafile/Test/source2dtestfile.h"
#include "testscalewidget.h"
#include <QMdiSubWindow>
#include <QDir>
//#include "Plotlib/plotwidgetbasic.h"
//#include "Plotlib/plotwidgetextestimplementation.h"
#include "Plotlib/Visualization/visualizationwidget.h"
#include "Plotlib/Manager/basic.h"
//QString escapeMinimal(const QString &name)
//{
//  QString ret;
//  for(int i=0;i<name.size();i++)
//  {
//    auto ch=name[i];
//    switch(ch.unicode())
//    {
//      case '\0':
//        ret+="\\0"; break;
//      case '\a':
//        ret+="\\a"; break;
//      case '\b':
//        ret+="\\b"; break;
//      case '\e':
//        ret+="\\e"; break;
//      case '\f':
//        ret+="\\f"; break;
//      case '\n':
//        ret+="\\n"; break;
//      case '\r':
//        ret+="\\r"; break;
//      case '\t':
//        ret+="\\t"; break;
//      case '\v':
//        ret+="\\v"; break;
//      case '\\':
//        ret+="\\\\"; break;
//      case '\"':
//        ret+="\\\""; break;
//      case '\'':
//        ret+="\\\'"; break;
//      default:
//        ret+=QChar(ch);
//    }
//  }
//  return ret;
//}

//QString escapeUnicode(const QString &name)
//{
//  auto nameChars=name.toUcs4();
//  QString ret;
//  for(int i=0;i<nameChars.size();i++)
//  {
//    auto ch=nameChars[i];
//    if(ch<0xA0)
//    {
//      // Character below A0 are encoded with a single character in UTF8
//      switch(ch)
//      {
//        case '\a':
//          ret+="\\a"; break;
//        case '\b':
//          ret+="\\b"; break;
//        case '\e':
//          ret+="\\e"; break;
//        case '\f':
//          ret+="\\f"; break;
//        case '\n':
//          ret+="\\n"; break;
//        case '\r':
//          ret+="\\r"; break;
//        case '\t':
//          ret+="\\t"; break;
//        case '\v':
//          ret+="\\v"; break;
//        case '\\':
//          ret+="\\\\"; break;
//        case '\"':
//          ret+="\\\""; break;
//        case '\'':
//          ret+="\\\'"; break;
//        case '\0':
//          ret+="\\0"; break;
//        default:
//          if(QChar(ch).isPrint())
//            ret+=QChar(ch);
//        else
//          ret+=QString("\\x")+QString::number(ch, 16).rightJustified(2,QChar('0'));
//      }
//    }
//    else if(QChar::isPrint(ch))
//      ret+=QString::fromUcs4(&ch, 1);
//    else if(ch<0x10000)
//      ret+=QString("\\u")+QString::number(ch, 16).rightJustified(4,QChar('0'));
//    else
//      ret+=QString("\\U")+QString::number(ch, 16).rightJustified(8,QChar('0'));
//  }
//  return ret;
//}

//QString escapeAscii(const QString &name)
//{
//  auto nameChars=name.toUcs4();
//  QString ret;
//  for(int i=0;i<nameChars.size();i++)
//  {
//    auto ch=nameChars[i];
//    if(ch<0x80)
//    {
//      // Character below A0 are encoded with a single character in UTF8
//      switch(ch)
//      {
//        case '\a':
//          ret+="\\a"; break;
//        case '\b':
//          ret+="\\b"; break;
//        case '\e':
//          ret+="\\e"; break;
//        case '\f':
//          ret+="\\f"; break;
//        case '\n':
//          ret+="\\n"; break;
//        case '\r':
//          ret+="\\r"; break;
//        case '\t':
//          ret+="\\t"; break;
//        case '\v':
//          ret+="\\v"; break;
//        case '\\':
//          ret+="\\\\"; break;
//        case '\"':
//          ret+="\\\""; break;
//        case '\'':
//          ret+="\\\'"; break;
//        case '\0':
//          ret+="\\0"; break;
//        default:
//          if(QChar(ch).isPrint())
//            ret+=QChar(ch);
//          else
//            ret+=QString("\\x")+QString::number(ch, 16).rightJustified(2,QChar('0'));
//      }
//    }
//    else if(ch<0xA0)
//      ret+=QString("\\x")+QString::number(ch, 16).rightJustified(2,QChar('0'));
//    else if(ch<0x10000)
//      ret+=QString("\\u")+QString::number(ch, 16).rightJustified(4,QChar('0'));
//    else
//      ret+=QString("\\U")+QString::number(ch, 16).rightJustified(8,QChar('0'));
//  }
//  return ret;
//}
//void writeChar(QFile &f, char32_t ch)
//{
//  QString base=QString::fromUcs4(&ch, 1);
//  QString encCOnly=CEscape::escape(base, [](const QChar &ch){return ch>=0x80 || !ch.isLetterOrNumber();}, true);
//  QString encGeneric=CEscape::escape(base, [](const QChar &ch){return ch>=0x80 || !ch.isLetterOrNumber();}, false);//escapeUnicode(base);
//  QString enc32=CEscape::escape32(base, [](const QChar &ch){return ch>=0x80 || !ch.isLetterOrNumber();}, true); //escapeMinimal(base);
//  QString encAscii=CEscape::escape(base, CEscape::escapeAscii); //escapeMinimal(base);
//  QString encUnicode=CEscape::escape(base, CEscape::escapePath, false); //escapeMinimal(base);
//  QString encUnicode4=CEscape::escape32(base, CEscape::escapePath, false); //escapeMinimal(base);
//  if(CEscape::unescape(encCOnly)!=base) qDebug()<<"Fail unescape COnly"<<ch<<base.toUtf8()<<CEscape::unescape(encCOnly).toUtf8();
////  if(CEscape::unescape(encGeneric)!=base) qDebug()<<"Fail unescape generic"<<ch;
////  if(CEscape::unescape(enc32)!=base) qDebug()<<"Fail unescape 32"<<ch;
////  if(CEscape::unescape(encAscii)!=base) qDebug()<<"Fail unescape Ascii"<<ch;
////  if(CEscape::unescape(encUnicode)!=base) qDebug()<<"Fail unescape Unicode"<<ch;
////  if(CEscape::unescape(encUnicode4)!=base) qDebug()<<"Fail unescape Unicode 4"<<ch;
//  f.write(QObject::tr("%8\t%1\t%2\t%3\t%4\t%5\t%6\t%7\n").arg(base, encGeneric,encCOnly, enc32, encAscii, encUnicode, encUnicode4).arg(ch,4,16).toUtf8());
//}
#include <Datafile/datasource2d.h>
void print(double *x, double *y, int n)
{
  for(int i=0;i<n;i++)
  {
    printf("%3d ",i);
    if(x)printf("%3.2f ", x[i]);
    else printf("------ ");
    if(y)printf("%3.2f ", y[i]);
    else printf("------ ");
    printf("\n");
  }
}
//void testIndexAfter(DataFile::DataSource2D *source, double x)
//{
//  qDebug()<<x<<source->indexLE(x);
//}
double test3(DataFile::DataSource2D *source, double xValue)
{
  Q_UNUSED(source);
  Q_UNUSED(xValue);
//  double c1_1,c1_2;
//  double x[4], y[4];
//  int j=source->indexLE(xValue);
//  if(j<0)
//    return qQNaN();
//  else if(j==source->size()-1)
//  {
//    return (source->x(source->size()-1)==xValue?source->y(source->size()-1):qQNaN());
//  }
//  source->data(j-1, 4, x, y);
//  double dx1=x[2]-x[1], dy1=y[2]-y[1], ds1=dy1/dx1;
//  if(j==0)
//    c1_1=ds1;
//  else
//  {
//    auto dx0=x[1]-x[0], dy0=y[1]-y[0], ds0=dy0/dx0;
//    if (ds0*ds1 <= 0)
//      c1_1=0.;
//    else {
//      auto common = dx0 + dx1;
//      c1_1=(3*common/((common + dx1)/ds0 + (common + dx0)/ds1));
//    }
//  }
//  if(j==source->size()-2)
//    c1_2=ds1;
//  else
//  {
//    auto dx2=x[3]-x[2], dy2=y[3]-y[2], ds2=dy2/dx2;
//    if (ds1*ds2 <= 0)
//      c1_2=0.;
//    else {
//      double common = dx1 + dx2;
//      c1_2=(3*common/((common + dx2)/ds1 + (common + dx1)/ds2));
//    }
//  }
//  auto invDx = 1/dx1, common = c1_1 + c1_2 - 2* ds1;
//  double c2=((ds1 - c1_1 - common)*invDx), c3=(common*invDx*invDx);

//  double diff = xValue - x[1], diffSq = diff*diff;
//  return y[1] + c1_1*diff + c2*diffSq + c3*diff*diffSq;
  return 0.;
}
double test2(DataFile::DataSource2D *source)
{
  Q_UNUSED(source);
//  QVector<double> xVal,yVal;
//  for(int i=0;i<source->size();i++)
//  {
//    xVal.append(source->x(i));
//    yVal.append(source->y(i));
//  }

//  QVector<double> dx,dy,ds;
//  for(int i=1;i<xVal.size();i++)
//  {
//    dx.append(xVal[i]-xVal[i-1]);
//    dy.append(yVal[i]-yVal[i-1]);
//    ds.append(dy.last()/dx.last());
//  }

//  QVector<double> c1s,c2s,c3s;
//  c1s.append(ds.first());
//  for(int i=0;i<ds.size()-1;i++)
//  {
//    auto m0=ds[i], m1=ds[i+1];
//    if (m0*m1 <= 0) {
//      c1s.append(0);
//    } else {
//       auto d0 = dx[i], d1 = dx[i + 1], common = d0 + d1;
//       c1s.append(3*common/((common + d1)/m0 + (common + d0)/m1));
//    }
//  }
//  c1s.append(ds.last());
//  for (int i = 0; i < c1s.size() - 1; i++) {
//     auto c1 = c1s[i], m_ = ds[i], invDx = 1/dx[i], common_ = c1 + c1s[i + 1] - m_ - m_;
//     c2s.append((m_ - c1 - common_)*invDx); c3s.append(common_*invDx*invDx);
//   }
//  for(int j=0;j<source->size()-1;j++)
//  {
//    double c1_1,c1_2;
//    double x[4], y[4];
//    source->data(j-1, 4, x, y);
//    double dx1=x[2]-x[1], dy1=y[2]-y[1], ds1=dy1/dx1;
//    if(j==0)
//      c1_1=ds1;
//    else
//    {
//      auto dx0=x[1]-x[0], dy0=y[1]-y[0], ds0=dy0/dx0;
//      if (ds0*ds1 <= 0)
//        c1_1=0.;
//      else {
//        auto common = dx0 + dx1;
//        c1_1=(3*common/((common + dx1)/ds0 + (common + dx0)/ds1));
//      }
//    }
//    if(j==source->size()-2)
//      c1_2=ds1;
//    else
//    {
//      auto dx2=x[3]-x[2], dy2=y[3]-y[2], ds2=dy2/dx2;
//      if (ds1*ds2 <= 0)
//        c1_2=0.;
//      else {
//        double common = dx1 + dx2;
//        c1_2=(3*common/((common + dx2)/ds1 + (common + dx1)/ds2));
//      }
//    }
//    auto invDx = 1/dx1, common = c1_1 + c1_2 - 2* ds1;
//    double c2=((ds1 - c1_1 - common)*invDx), c3=(common*invDx*invDx);

//    qDebug()<<j<<c1_1-c1s[j]<<c2-c2s[j]<<c3-c3s[j];//;c1s[j]-c1_i<<c1s[j+1]-c1_i1;//<<c2s[j]<<c3s[j];
////    qDebug()<<j<<c1_i<<c2<<c3;//;c1s[j]-c1_i<<c1s[j+1]-c1_i1;//<<c2s[j]<<c3s[j];
//  }
//  return 0.;
  return 0.;
}

double test(DataFile::DataSource2D *source, double xValue)
{
  Q_UNUSED(source);
  Q_UNUSED(xValue);
//  QVector<double> x,y,dx,dy,ds,c1s,c2s,c3s;
//  for(int i=0;i<source->size();i++)
//  {
//    x.append(source->x(i));
//    y.append(source->y(i));
//    if(i)
//    {
//      dx.append(x[i]-x[i-1]);
//      dy.append(y[i]-y[i-1]);
//      ds.append(dy.last()/dx.last());
//    }
//  }
//  c1s.append(ds.first());
//  for(int i=0;i<ds.size()-1;i++)
//  {
//    auto m0=ds[i], m1=ds[i+1];
//    if (m0*m1 <= 0) {
//      c1s.append(0);
//    } else {
//       auto d0 = dx[i], d1 = dx[i + 1], common = d0 + d1;
//       c1s.append(3*common/((common + d1)/m0 + (common + d0)/m1));
//    }
//  }
//  c1s.append(ds.last());
//  for (int i = 0; i < c1s.size() - 1; i++) {
//     auto c1 = c1s[i], m_ = ds[i], invDx = 1/dx[i], common_ = c1 + c1s[i + 1] - m_ - m_;
//     c2s.append((m_ - c1 - common_)*invDx); c3s.append(common_*invDx*invDx);
//   }

//  int j;
//  for(j=0;j<x.size();j++)
//  {
//    if(x[j]>xValue)
//      break;
//  }
//  j--;
//  if(j<0)
//    return qQNaN();
//  if(j>=x.size()-1)
//    return xValue==x.last()?y.last():qQNaN();
//  // Interpolate
//  double diff = xValue - x[j], diffSq = diff*diff;
//  return y[j] + c1s[j]*diff + c2s[j]*diffSq + c3s[j]*diff*diffSq;
  return 0.;
}
void testPlotStat();
void testPlotStat2();
QSharedPointer<DataFile::DataSource2D> testSource1;
QSharedPointer<DataFile::DataSource2D> testSource2;
#include "mathlib/sum.h"
#include "Plotlib/Area/Impl/plotarea2d.h"
#include <QElapsedTimer>
void testFloatingPoint();
namespace HEF
{
  quint64 encodeFloating64Portable(double value);
  quint64 encodeFloating64Fast(double value);
  quint64 encodeFloating64(double value);
  double decodeFloating64Portable(quint64 value);
  double decodeFloating64Fast(quint64 value);
  double decodeFloating64(quint64 value);
}
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow), m_testScaleSub(nullptr)
{
  QElapsedTimer t;
  t.start();
  for(qint64 i=0;i<100000000;i++)
    HEF::decodeFloating64Portable(HEF::encodeFloating64Portable(0.15));
  qDebug()<<"Encoded portable"<<t.restart();
  for(qint64 i=0;i<100000000;i++)
    HEF::decodeFloating64Fast(HEF::encodeFloating64Fast(0.15));
  qDebug()<<"Encoded fast"<<t.restart();
  for(qint64 i=0;i<100000000;i++)
    HEF::decodeFloating64(HEF::encodeFloating64(0.15));
  qDebug()<<"Encoded"<<t.restart();
  testFloatingPoint();
  exit(0);
//  QVector<double> x ({0, 0.993145, 1.98629, 2.97944, 3.97258, 4.96573, 5.95887, 6.95202, 7.94516, 8.93831, 9.93145, 10.9246, 11.9177, 12.9109, 13.904, 14.8972, 15.8903, 16.8835, 17.8766, 18.8698, 19.8629, 20.8561, 21.8492, 22.8423, 23.8355, 24.8286, 25.8218, 26.8149, 27.8081, 28.8012, 29.7944, 30.7875, 31.7807, 32.7738, 33.7669, 34.7601, 35.7532, 36.7464, 37.7395, 38.7327, 39.7258, 40.719, 41.7121, 42.7053, 43.6984, 44.6915, 45.6847, 46.6778, 47.671, 48.6641, 49.6573, 50.6504, 51.6436, 52.6367, 53.6299, 54.623, 55.6161, 56.6093, 57.6024, 58.5956, 59.5887, 60.5819, 61.575, 62.5682, 63.5613, 64.5545, 65.5476, 66.5407, 67.5339});
//  QVector<double> y ({207.509, 202.4, 206.942, 194.283, 177.702, 195.005, 185.042, 179.477, 162.863, 180.314, 185.794, 183.048, 182.328, 163.281, 172.884, 182.092, 165.862, 165.481, 142.822, 132.001, 143.85, 163.683, 162.735, 145.215, 125.066, 113.693, 99.7606, 84.2673, 48.9809, 87.7791, 81.2638, 90.4141, 80.987, 37.9909, 58.0087, 79.312, 92.04, 100.667, 103.035, 80.3793, 66.3808, 80.5896, 77.3884, 61.1469, 66.8095, 44.0433, 46.04, 5.52918, -39.6757, -36.1541, -51.2867, -47.5532, -41.3326, -55.157, -78.8052, -56.4657, -59.3636, -70.9313, -86.5469, -98.7616, -94.7697, -87.5395, -83.339, -111.133, -77.8034, -60.7085, -35.7969, -43.6868, -46.1658});
//  QVector<double> x ({19.8629, 20.8561, 21.8492, 22.8423, 23.8355, 24.8286, 25.8218, 26.8149});
//  QVector<double> y ({-143.85, -163.683, -162.735, -145.215, -125.066, -113.693, -99.7606, -84.2673});
////  const double maxAngle=85, maxSlope=tan(maxAngle*M_PI/180.);
//  double maxSlope=11.43;
////  for(int i=0;i<y.size();i++)
////    y[i]=-y[i];
////  x=x.mid(20,8); y=y.mid(20,8);
//  qDebug()<<x;
//  qDebug()<<y;
//  qDebug()<<maxSlope;
//  Plot::PlotArea2DSpanPainterV3::simplify(x,y, maxSlope, true);
//  qDebug()<<x;
//  qDebug()<<y;
//  exit(0);
//  double x[20], y[20];
//  auto data=new DataFile::DataSourceTest;
////  test2(data);
//  for(int i=0;i<33;i++)
//  {
//    double x=(i+14)/10.;
//    qDebug()<<x<<data->yLinear(x)<<data->yHermite(x)<<data->yStep(x)<<data->yStepMiddle(x); //<<data->yHermite(x)<<test(data,x)<<test3(data, x);
//  }
//  testIndexAfter(data,1.499);
//  testIndexAfter(data,1.5);
//  testIndexAfter(data,1.501);
//  testIndexAfter(data,2.3);
//  testIndexAfter(data,2.7);
//  testIndexAfter(data,3.499);
//  testIndexAfter(data,3.5);
//  testIndexAfter(data,3.501);
//  if(data->dataX(x, -1, 6))
//    print(x,y, 6);
  // UTF8 "f1 85 98 93" UTF16 d8d5 de13 UCS4 284179d
//  const char x[]="\u613";

//  const char x[]="\U00045613";
//  QString s(x);
//  qDebug()<<sizeof(x)<<QByteArray(x).toHex()<<QVector<QChar>(s.begin(), s.end())<<s.toUcs4();
//  unescape("\\U45613");
//  for(int i=1;i<257;i++)
//    qDebug()<<i<<QChar(i)<<escape(QChar(i));
//  QFile f("F:/amarzocchi/encode.txt");
//  if(f.open(QFile::Text|QFile::ReadWrite|QFile::Truncate))
//  {
//    for(int i=0;i<1030;i++)
//    {
//      writeChar(f, i);
//    }
//    for(int i=0x10b76;i<0x10b90;i++)
//    {
//      writeChar(f, i);
//    }
//  }
  //  unescape("Pi\\x4B3pp\\1070opaperino\\u47B3giovanni\\\"");
//  unescape("Pippo/Pluto");
//  unescape("Pippo\\/Plu\\X45to");
//  unescape("Pippo\\A/Pluto");
//  unescape("Pippo\\\\/Pluto");

  ui->setupUi(this);
//  ui->actionScale_test->setChecked(true);


//  ui->DewesoftVersion->setText(tr("Dewesoft version: %1").arg(DW7DataFile::libraryVersionString()));
//  auto *file= new DW7DataFile("F:/Public/Proto_Test02_0000.dxd");
//  openAcquisition("F:/Public/Proto_Test02_0000.dxd");
//  ui->mdiArea->addSubWindow(new TestScaleWidget());
  //  openAcquisition("F:/Public/TestManual.dxd");
  auto fn=QDir(QCoreApplication::applicationDirPath()).relativeFilePath("TestData.bin");
  // "F:/Public/Proto_Test02_0000.dxd/Data/MapV_X"
  // "F:/Public/Proto_Test02_0000.dxd/Data/ECU_RPM"
  const QString dataPath("F:/Public/Proto_Test02_0000.dxd/Data/MapV_Y"); //
  if(!QFileInfo(fn).isFile() )
  {
    auto entry=DataFile::open(dataPath); ///Coil V_X
    qDebug()<<"Failed to get data"<<fn<<"Reading from source"<<entry;
    QFile f(fn);
    if(entry && f.open(QIODevice::ReadWrite|QIODevice::Truncate))
    {
      auto data=entry->data().dynamicCast<DataFile::DataSource2D>();
      if(data)
      {
        qint64 size=data->size();
        if(size>=0)
        {
          constexpr int bufSize=10000;
          QScopedPointer<double, QScopedPointerArrayDeleter<double> > xBuffer(new double[bufSize]), yBuffer(new double[bufSize]);
          qint64 pos=0;
          while(size>0)
          {
            auto n=qMin(size,(qint64)bufSize);
            data->data(pos, n, xBuffer.data(), yBuffer.data());
            for(int j=0;j<n;j++)
            {
              f.write((const char *)(xBuffer.data()+j), sizeof(double));
              f.write((const char *)(yBuffer.data()+j), sizeof(double));
            }
            size-=n;
            pos+=n;
          }
        }
        qDebug()<<"Has data"<<entry<<data<<data->size();
      }
    }
  }
//  qDebug()<<"Integral"<<sqrt(HEF::integralSquared(0,2,3,4)/3);
  testSource2=(new DataFile::Source2DTestFile(fn))->sharedPtr();
  testSource1=(new DataFile::DataSourceTest2({0, 30, 80, 120}, {0, -20, 30, 10}))->sharedPtr();
//  for(int i=0;i<100;i++)
//    qDebug()<<testSource2->y(i);
//  auto entry=DataFile::open("F:/Public/Proto_Test02_0000.dxd/Data/MapV_X"); ///Coil V_X
//  auto entry=DataFile::open("F:/Public/Proto_Test02_0000.dxd/Data/ECU_RPM"); ///Coil V_X
//  if(entry)
//  {
//    auto data=entry->data().dynamicCast<DataFile::DataSource2D>();
//    qDebug()<<"Got data"<<data<<data->size();
//    testSource2=data;
//  }
// Still to check: fast stat integral mismatch
//  double min=qQNaN(), max=qQNaN(),integral=qQNaN(), deltaX=qQNaN();
//  const int sStart=146975, sSize=37;
//  double curMax=qQNaN();
//  for(int i=0;i<sSize+1;i++)
//  {
//    double v=testSource2->y(sStart+i);
//    curMax=fmax(curMax, v);
//    qDebug()<<sStart+i<<v;
//  }
//  qDebug()<<curMax;
//  testSource2->getStatFast(sStart, sSize, &min, &max, &integral, nullptr, &deltaX);
//  exit(0);

//  int i;

//  double xOld=qQNaN(), yOld=qQNaN();

//  HEF::KahanSum accum, sqAccum;
//  for(double x=30;x<=80;x+=1)
//  {
//    auto i=1;//testSource1->indexLE(x);
//    double y;
//    double intB, intA, sqIntB,sqIntA;
//    testSource1->yInterpolateFast(i, x, y, &intB, &intA, &sqIntB, &sqIntA);
//    if(!qIsNaN(xOld))
//    {
//      accum+=HEF::integralSawtooth(xOld,yOld, x,y);
//      sqAccum+=HEF::integralSquaredSawtooth(xOld, yOld, x,y);
//    }
//    xOld=x;
//    yOld=y;
////    testSource1->yStepMiddleFast(i, x, middle);
////    testSource1->yLinearFast(i, x, linear);
////    testSource1->yHermiteFast(i, x, hermite);

//    printf("%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\n",x, y, intB, accum.sum(), sqIntB, sqAccum.sum(), intB+intA, sqIntB+sqIntA);
//  }
//  exit(1);
//  // 3...166

//  testPlotStat2();
//  exit(1);

//  double min, max, avg, rms;
//  testSource1->getStat({HEF::Range(-7,1.4)}, &min, &max, &avg, &rms, nullptr, nullptr, nullptr);
//  qDebug()<<min<<max<<avg<<rms;

//  auto w=new PlotWidgetBasic();
  auto w=new PlotWidget(new Plot::ManagerBasic);
  ui->mdiArea->addSubWindow(w);

//  qDebug()<<testSource1->x(0)<<testSource1->y(0);
  //  openAcquisition("F:/Public/Proto_Test02_0000.dxd");
  //  DataFile::open("F:/pu")
//  auto x=DataFile::openFile("F:/Public/AAA/../Proto_Test02_0000.dxd");
//  qDebug()<<x;
}

void MainWindow::openAcquisition(const QString &filename)
{
  DataFile::FilePtr file=DataFile::openFile(filename); //DW7DataFile::openFile(filename);
  if(file)
  {
    auto w=new FileWidget(file);
    ui->mdiArea->addSubWindow(w);
  }
}
MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_actionScale_test_toggled(bool arg1)
{
  if(arg1)
  {
    if(!m_testScaleSub)
    {
      m_testScaleSub=ui->mdiArea->addSubWindow(new TestScaleWidget());
      m_testScaleSub->setWindowTitle("Test scale");
      m_testScaleSub->setAttribute(Qt::WA_DeleteOnClose);
      connect(m_testScaleSub, &QObject::destroyed, [this](){m_testScaleSub=nullptr; ui->actionScale_test->setChecked(false);});
    }
    m_testScaleSub->show();
  }
  else
  {
    if(m_testScaleSub)
      m_testScaleSub->close();
  }
}
