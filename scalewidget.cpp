#include "scalewidget.h"
#include <QPainter>
#include "Plotlib/tickalgorithmbasic.h"

ScaleWidget::ScaleWidget(QWidget *parent) : QWidget(parent)
{
  m_function=HEF::logarithmicTwoPoints(1,0,100,2);
  m_function=HEF::linearTwoPoints(0, 0, 1000, 1000);
  m_dirty=true;
}

QSize ScaleWidget::minimumSizeHint() const
{
  return QSize(m_lineX+20, m_border*2);
}

void ScaleWidget::recalculateScale()
{
  if(m_function)
  {
    TickAlgorithmBasic alg(m_function, m_border,size().height()-m_border);
    alg.setMinTickDistance(50, 15);
    alg.recalculate();
    m_ticks=alg.mainTicks();
  }
}


void ScaleWidget::paintEvent(QPaintEvent *event)
{
  QPainter p(this);
  int top=m_border;
  int bottom=size().height()-m_border;
  p.drawLine(20,top, 20, bottom);
  if(m_dirty)
  {
    m_dirty=false;
    recalculateScale();
  }
}


void ScaleWidget::resizeEvent(QResizeEvent *event)
{
  m_dirty=true;
}
