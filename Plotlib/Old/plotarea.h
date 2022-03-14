#ifndef PLOTAREA_H
#define PLOTAREA_H

#include <QObject>
#include <QPainter>
#include <Mathlib/function.h>
class PlotArea : public QObject
{
  Q_OBJECT
public:
  explicit PlotArea(QObject *parent = nullptr);
  virtual void plot(QPainter &p, QSizeF size)=0;
  inline HEF::FunctionPtr scaleX() const { return m_scaleX; }
  inline HEF::FunctionPtr scaleY() const { return m_scaleY; }
  void setScaleX(const HEF::FunctionPtr &scaleX);
  void setScaleY(const HEF::FunctionPtr &scaleY);

protected:
  HEF::FunctionPtr m_scaleX;
  HEF::FunctionPtr m_scaleY;
};

#endif // PLOTAREA_H
