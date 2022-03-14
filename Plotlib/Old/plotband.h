#ifndef PLOTBAND_H
#define PLOTBAND_H

class QPainter;
//class PlotScale : public QObject
//{
//  Q_OBJECT
//public:
//  explicit PlotScale(QObject *parent = nullptr);
//  virtual void setWidth(double width)=0;
//  virtual HEF::FunctionPtr scale()=0;
//  virtual TickList ticks()=0;
//signals:
//  void layoutDirty();
//};

//class PlotScaleDefault: public PlotScale
//{
//  Q_OBJECT
//public:
//  explicit PlotScaleDefault(HEF::FunctionPtr function): m_function(function) { m_dirty=true; m_width=0.; }
//  virtual void setWidth(double width) { m_width=width; m_dirty=true; }
//  virtual HEF::FunctionPtr scale() { return m_function; }
//  virtual TickList ticks() { if(m_dirty){ recalculate(); m_dirty=false; } return m_tickList; }
//protected:
//  void recalculate();
//  double m_width;
//  bool m_dirty;
//  HEF::FunctionPtr m_function;
//  TickList m_tickList;
//};


#endif // PLOTBAND_H
