#ifndef PLOTAREA2D_H
#define PLOTAREA2D_H
#include "plotarea.h"
#include "../Datafile/datasource2d.h"
class PlotArea2DSpanPainter
{
public:
  PlotArea2DSpanPainter(double minY, double maxY): m_minY(minY), m_maxY(maxY) { }
  void feed(QPainter &painter, double x, const HEF::Range &range);
  void finish(QPainter &painter);
protected:
  double m_minY;
  double m_maxY;
  QList<double> m_x;
  QList<double> m_min;
  QList<double> m_max;
};

class PlotArea2DLinePainter
{
public:
  PlotArea2DLinePainter(double minY, double maxY): m_minY(minY), m_maxY(maxY) { }
  void feed(QPainter &painter, double x, double y);
  void finish(QPainter &painter);
protected:
  double m_minY;
  double m_maxY;
  QList<double> m_x;
  QList<double> m_y;
};

class PlotArea2D: public PlotArea
{
  Q_OBJECT
public:
  PlotArea2D();

  // PlotArea interface
public:
  void plot(QPainter &p, QSizeF size) override;
protected:
  QSharedPointer<DataFile::DataSource2D> m_data;
  double m_plotResolution;
};



#endif // PLOTAREA2D_H
