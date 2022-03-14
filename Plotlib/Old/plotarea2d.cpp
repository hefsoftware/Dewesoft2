#include "plotarea2d.h"
#include <QDebug>
extern QSharedPointer<DataFile::DataSource2D> testSource1;

PlotArea2D::PlotArea2D()
{
  m_plotResolution=2.5;
  m_data=testSource1;
}

void PlotArea2D::plot(QPainter &p, QSizeF size)
{
  static bool first=true;
  if(m_scaleX && m_scaleY && m_data)
  {
    p.setClipRect(QRectF(QPointF(0,0), size), Qt::IntersectClip);
    qint64 nDiv=qBound(1., std::ceil(size.width()/m_plotResolution), 5000.);
    double delta=size.width()/nDiv;
    QList<double> boundary, center;
    for(int j=0;j<=nDiv+1;j++)
      boundary.append((j-0.5)*delta);
    for(int j=0;j<=nDiv;j++)
      center.append(j*delta);
    auto ranges=m_scaleX->preimage(boundary);
    Q_ASSERT(ranges.size()==center.size());
    QVector<double> avg, min, max;
    m_data->getStat(ranges, &min, &max, &avg);
    PlotArea2DSpanPainter span(0, size.width());
    PlotArea2DLinePainter stroke(0, size.width());
    for(int i=0;i<center.size();i++)
    {
      span.feed(p, center[i], m_scaleY->image(HEF::Range(min[i], max[i])));
      stroke.feed(p, center[i], m_scaleY->f(avg[i]));

    }
    span.finish(p);
    stroke.finish(p);
  }
  first=false;
}

void PlotArea2DSpanPainter::feed(QPainter &painter, double x, const HEF::Range &range)
{
  if(!range.isEmpty())
  {
    double d=m_maxY-m_minY;
    double curMin=qBound(range.min(), m_minY-2*d, m_maxY+2*d), curMax=qBound(range.max(), m_minY-2*d, m_maxY+2*d);
    m_min.append(curMin);
    m_max.append(curMax);
    m_x.append(x);
  }
  else if(!m_x.isEmpty())
  {
    QPainterPath filled;
    filled.moveTo(m_x[0], m_min[0]);
    for(int i=1;i<m_x.size();i++)
    {
      filled.lineTo(m_x[i], m_min[i]);
    }
    for(int i=m_x.size()-1;i>=0;i--)
    {
      filled.lineTo(m_x[i], m_max[i]);
    }
    filled.lineTo(m_x[0], m_min[0]);
    painter.fillPath(filled, QBrush(QColor(255,0,0,64)));
    m_x.clear();
    m_min.clear();
    m_max.clear();
  }
}

void PlotArea2DSpanPainter::finish(QPainter &painter)
{
  feed(painter, qQNaN(), HEF::Range());
}

void PlotArea2DLinePainter::feed(QPainter &painter, double x, double y)
{
  if(!qIsNaN(y))
  {
    double d=m_maxY-m_minY;
    y=qBound(y, m_minY-2*d, m_maxY+2*d);
    m_x.append(x);
    m_y.append(y);
  }
  else if(!m_x.isEmpty())
  {
    QPainterPath stroked;
    stroked.moveTo(m_x[0], m_y[0]);
    for(int i=(m_x.size()>1);i<m_x.size();i++)
    {
      stroked.lineTo(m_x[i], m_y[i]);
    }
    painter.strokePath(stroked, QPen(QColor(255,0,0),2));
    m_x.clear();
    m_y.clear();
  }
}

void PlotArea2DLinePainter::finish(QPainter &painter)
{
  feed(painter, qQNaN(), qQNaN());
}
