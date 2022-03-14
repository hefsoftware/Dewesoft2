#include "testscalewidget2.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
#include <QDebug>
#include "Plotlib/tickalgorithmbasic.h"
#include <QResizeEvent>
#include <QStyle>
TestScaleWidget2::TestScaleWidget2(QWidget *parent): QWidget(parent)
{
//  m_function=HEF::linearFromTwoPoints(0, 0, 1000, 1000);
  m_function=HEF::linearTwoPoints(0, 0, 2000, 1000);
  m_scroll.setNotifyFunc([this](double scale, double offset){ m_scale=scale; m_offset=offset; m_dirty=true; update(); return true; });
  m_scale=1.;
  m_offset=0.;
  m_dirty=true;
  m_ticksDistance=70;
  m_leftMargin=m_rightMargin=20;
  m_mainTickDist=50;
  m_subTickDist=15;
  m_okCalculation=false;
  m_background=Qt::white;
}

void TestScaleWidget2::paintEvent(QPaintEvent *event)
{
  QPainter p(this);
  p.fillRect(0,0,width(),height(),QBrush(m_background));
  double x;

  const int topBand=50;
  p.drawRect(m_leftMargin,0,width()-m_leftMargin-m_rightMargin-1, topBand);
  p.drawRect(m_leftMargin,topBand,width()-m_leftMargin-m_rightMargin-1, height()-topBand-1);
//  if(map(0, x))
//  {
//    p.setPen(QPen(Qt::black, 2));
//    p.drawLine(x,0,x,5);
//    p.drawLine(x,30,x,height()-1);
//  }
  double ticksWidth=m_ticksDistance/m_scale;
  int order=ceil(std::log10(ticksWidth));
  double orderPow=std::pow(10,order);
  double scaledTicks=ticksWidth/orderPow;
  if(scaledTicks<=0.2)
  {
    scaledTicks=orderPow*0.2;
  }
  else if(scaledTicks<=0.5)
  {
    scaledTicks=orderPow*0.5;
  }
  else
    scaledTicks=orderPow;
  double val=std::ceil(((m_leftMargin-m_offset)/m_scale)/scaledTicks)*scaledTicks;
  p.setPen(QPen(Qt::black, 0));
  for(;;)
  {
    x=val*m_scale+m_offset;
    if(x>width()-m_rightMargin)
      break;
    p.drawLine(x,topBand,x,topBand+5);
    p.drawLine(x,topBand+30,x,height()-1);
    p.drawText(x-100,topBand+10, 200, 20, Qt::AlignCenter, QString::number(val));
    val+=scaledTicks;
  }
  if(m_dirty)
  {
    m_okCalculation=false;
    auto f=HEF::linearGainOffset(m_function, m_scale, m_offset);
    m_ticks.clear();
    m_calcTicks.clear();
    if(f)
    {
      TickAlgorithmBasic alg(f, m_leftMargin, width()-m_rightMargin);
      alg.setMinTickDistance(m_mainTickDist, m_subTickDist);
      m_okCalculation=alg.recalculate();
      m_calcTicks=alg.ticks();

      if(m_manualMain.isEmpty())
      {
        m_ticks=m_calcTicks;
      }
      else
      {
        for(auto it=m_manualMain.begin();it!=m_manualMain.end();it++)
        {
          double x=*it;
          double y=f->f(x);
          if(!qIsNaN(x) && !qIsNaN(y))
            m_ticks.addMain(y,x);
        }
        for(auto it=m_manualSub.begin();it!=m_manualSub.end();it++)
        {
          double x=*it;
          double y=f->f(x);
          if(!qIsNaN(x) && !qIsNaN(y))
            m_ticks.addSub(y, x);
        }
      }
//      m_ticks=alg.mainTicks();
    }
    m_dirty=false;
    changedLayout();
  }
  bool odd=true;
  bool ok=true;
  for(int i=0;i<m_ticks.size();i++)
//  for(auto it=m_mainTicks.begin();it!=m_mainTicks.end();it++)
  {
    double x=m_ticks.x(i), y=m_ticks.y(i);
    if(y>=m_leftMargin-0.1 && y<=width()+0.1-(double)m_rightMargin)
    {
      odd=!odd;
      double l=qMax(y-m_mainTickDist, (double)m_leftMargin);
      double r=qMin(y+m_mainTickDist, width()-(double)m_rightMargin);
      p.fillRect(l, odd*10, r-l, 10, QBrush(QColor(255,128,128,128)));
      p.drawLine(y,0,y,20);
      p.drawText(y-100,20, 200, 15, Qt::AlignCenter, QString::number(x));
      int precision;
      if(m_ticks.hasPrecision(i, &precision))
      {
        p.drawText(y-100,30, 200, 15, Qt::AlignCenter, QString::number(pow(10,precision)));
        p.drawLine(y,45,y,topBand);
      }
      else
        p.drawLine(y,35,y,topBand);

    }
    else
      ok=false;
  }
  odd=true;
  auto &sub=m_ticks.subTicks();
  for(auto it=sub.begin();it!=sub.end();it++)
  {
    double y=it.key();
    if(y>=m_leftMargin-0.1 && y<=width()+0.1-(double)m_rightMargin)
    {
      odd=!odd;
      double l=qMax(y-m_subTickDist, (double)m_leftMargin);
      double r=qMin(y+m_subTickDist, width()-(double)m_rightMargin);
      p.fillRect(l, topBand-5-odd*5, r-l, 5, QBrush(odd?QColor(255,128,128,128):QColor(128,255,128,128)));
      p.drawLine(y,topBand-10,y,topBand);
    }
    else
      ok=false;
  }
  if(!ok)
  {

    style()->standardIcon(QStyle::SP_MessageBoxWarning).paint(&p, 4, height()-36, 32, 32);
  }
  else if(!m_okCalculation)
  {

    style()->standardIcon(QStyle::SP_MessageBoxCritical).paint(&p, 4, height()-36, 32, 32);
  }
}

void TestScaleWidget2::mousePressEvent(QMouseEvent *event)
{
  if(event->buttons()==Qt::LeftButton)
  {
    m_scroll.startScroll(event->x());
  }
}

void TestScaleWidget2::mouseReleaseEvent(QMouseEvent *event)
{
  if(m_scroll.isScrolling())
    m_scroll.endScroll(event->x());
}

void TestScaleWidget2::mouseMoveEvent(QMouseEvent *event)
{
  if(m_scroll.isScrolling())
    m_scroll.updateScroll(event->x());
}

void TestScaleWidget2::wheelEvent(QWheelEvent *event)
{
  m_scroll.scaleWheelUnit(event->x(), event->angleDelta().y()*((event->modifiers()&Qt::ShiftModifier)?0.1:1));
}


QSize TestScaleWidget2::sizeHint() const
{
  return QSize(350,150);
}

void TestScaleWidget2::setFunction(const HEF::FunctionPtr &function)
{
  m_function = function;
  m_dirty=true;
  update();
}

void TestScaleWidget2::setScale(double scale)
{
  m_scale = scale;
}

double TestScaleWidget2::offset() const
{
    return m_offset;
}

void TestScaleWidget2::setOffset(double offset)
{
//  m_scroll.
  m_offset = offset;
}

void TestScaleWidget2::setScroll(double scale, double offset)
{
  m_scroll.setScroll(scale, offset);
}

double TestScaleWidget2::scale() const
{
    return m_scale;
}

void TestScaleWidget2::setBackground(const QColor &background)
{
    m_background = background;
}

QColor TestScaleWidget2::background() const
{
  return m_background;
}

QMap<double, double> TestScaleWidget2::subTicks() const
{
  return m_calcTicks.subTicks();
}

QMap<double, double> TestScaleWidget2::mainTicks() const
{
  return m_calcTicks.mainTicks();
}

double TestScaleWidget2::subTickDist() const
{
  return m_subTickDist;
}

void TestScaleWidget2::setSubTickDist(double subTickDist)
{
  m_subTickDist = subTickDist;
  m_dirty=true;
  update();
}

void TestScaleWidget2::setManual(const QSet<double> &ticks, const QSet<double> &ticksSub)
{
  m_manualMain=ticks;
  m_manualSub=ticksSub;
  m_dirty=true;
  update();
}

int TestScaleWidget2::leftPosition()
{
  return m_leftMargin;
}

int TestScaleWidget2::rightPosition()
{
  return width()-m_rightMargin;
}

double TestScaleWidget2::mainTickDist() const
{
  return m_mainTickDist;
}

void TestScaleWidget2::setMainTickDist(double mainTickDist)
{
  m_mainTickDist = mainTickDist;
  m_dirty=true;
  update();
}

int TestScaleWidget2::rightMargin() const
{
  return m_rightMargin;
}

void TestScaleWidget2::setRightMargin(int rightMargin)
{
  m_rightMargin = rightMargin;
}

void TestScaleWidget2::setLeftMargin(int leftMargin)
{
  m_leftMargin = leftMargin;
  m_dirty=true;
  update();
}

int TestScaleWidget2::leftMargin() const
{
  return m_leftMargin;
}

double TestScaleWidget2::ticksDistance() const
{
  return m_ticksDistance;
}

void TestScaleWidget2::setTicksDistance(double ticksDistance)
{
  m_ticksDistance = ticksDistance;
  m_dirty=true;
  update();
}


void TestScaleWidget2::resizeEvent(QResizeEvent *event)
{
  if(event->size().width()!=event->oldSize().width())
    m_dirty=true;
  update();
}
