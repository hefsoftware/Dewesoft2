#pragma once
#include "plotarea.h"
#include "../../../Datafile/datasource2d.h"
#include <QSharedPointer>

namespace Plot
{

  class PlotArea2DSpanPainterV3
  {
  public:
    PlotArea2DSpanPainterV3(double minY, double maxY): m_minY(minY), m_maxY(maxY) { }
    void feed(QPainter &painter, double x, const HEF::Range &yRange, double yVal);
    void finish(QPainter &painter) { feed(painter, qQNaN(), HEF::Range(), qQNaN()); }
    static void simplifyMax(QVector<double> &x, QVector<double> &y, double maxSlope);
    static void simplify(QVector<double> &x, QVector<double> &y, double maxSlope, bool max);
  protected:
    // Simplify the top part of a series of points limiting the slope between them.
    double m_minY;
    double m_maxY;
    double m_firstX, m_firstY;
    QVector<double> m_x;
    QVector<double> m_min;
    QVector<double> m_max;
  };

  class PlotArea2DSpanPainterV2
  {
  public:
    PlotArea2DSpanPainterV2(double minY, double maxY): m_minY(minY), m_maxY(maxY) { }
    void feed(QPainter &painter, double x, const HEF::Range &yRange, double yVal);
    void finish(QPainter &painter) { feed(painter, qQNaN(), HEF::Range(), qQNaN()); }
  protected:
    double m_minY;
    double m_maxY;
    double m_firstX, m_firstY;
    QList<double> m_x;
    QList<double> m_min;
    QList<double> m_max;
  };

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
  class PlotArea2DPointPainter
  {
  public:
    PlotArea2DPointPainter(double minY, double maxY): m_minY(minY), m_maxY(maxY), m_lastX(qQNaN()), m_lastY(qQNaN()) { }
    void feed(QPainter &painter, double x, double y, qint64 numRanges);
    void finish(QPainter &painter);
  protected:
    void strokePoint(QPainter &painter, double x, double y);
    double m_minY;
    double m_maxY;
    double m_lastX;
    double m_lastY;
  };
  class Area2D: public Area
  {
  public:
    Area2D();
    void paint(QPainter &painter, bool screen) override;
  protected:
    QSharedPointer<DataFile::DataSource2D> m_data;
    double m_plotResolution;
    qint64 m_maxPrepare, m_maxPaint, m_maxGetStat;
    void paintOptimized(QPainter &painter, bool screen);
  };
}
