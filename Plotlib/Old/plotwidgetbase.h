#ifndef PLOTWIDGETBASE_H
#define PLOTWIDGETBASE_H

#include "basewidget.h"
#include "../Mathlib/function.h"
#include "ticklist.h"
#include <QMap>
#include <QMouseEvent>
class PlotArea;
class PlotWidgetBase : public BaseWidget
{
  Q_OBJECT
public:  
  enum class Area
  {
    None=0,
    LeftScale=1,
    LeftLabel=2,
    BottomScale=4,
    BottomLabel=8,
    TopScale=16,
    TopLabel=32,
    RightScale=64,
    RightLabel=128,
    GraphLabel=256,
    PlotArea=512,
    Scales=LeftScale|BottomScale|RightScale|TopScale,
    All=1023
  };
  Q_DECLARE_FLAGS(Areas, Area)
  explicit PlotWidgetBase(QWidget *parent = nullptr);
  QSize minimumSizeHint() const override;
  Area plotArea(QPointF pt);
protected:
  void addPlotArea(PlotArea *plot);
  void paintRect(QPainter &p, QRectF r, int baseW, const QString &text="");
  void hideArea(const Areas &area);
  void showArea(const Areas &area);
  void paintEvent(QPaintEvent *event) override;
  virtual HEF::FunctionPtr scaleX(int i)=0;
  virtual HEF::FunctionPtr scaleY(int i)=0;
//  virtual HEF::FunctionPtr scaleX(int i);
//  virtual HEF::FunctionPtr scaleY(int i);
  QMargins m_bands;
  QList<PlotArea *> m_plots;
//  HEF::FunctionPtr m_baseScaleX, m_baseScaleY;
//  bool m_dirtyX, m_dirtyY;
  Areas m_showedAreas;
  Areas m_optionalAreas;

  virtual double labelHeight() { return 20; }
  virtual double topLabelHeight() { return 15; }
  virtual double bottomLabelHeight() { return 15; }
  virtual double leftLabelHeight() { return 15; }
  virtual double rightLabelHeight() { return 15; }
  virtual double topScaleHeight() { return 15; }
  virtual double bottomScaleHeight() { return 15; }
  virtual double leftScaleHeight() { return 15; }
  virtual double rightScaleHeight() { return 15; }

  virtual void paintMainLabel(QPainter &p, const QRectF &r, double w) { Q_UNUSED(p); Q_UNUSED(r); Q_UNUSED(w); }
  virtual void paintTopLabel(QPainter &p, const QRectF &r, double w) { Q_UNUSED(p); Q_UNUSED(r); Q_UNUSED(w); }
  virtual void paintBottomLabel(QPainter &p, const QRectF &r, double w) { Q_UNUSED(p); Q_UNUSED(r); Q_UNUSED(w); }
  virtual void paintLeftLabel(QPainter &p, const QRectF &r, double w) { Q_UNUSED(p); Q_UNUSED(r); Q_UNUSED(w); }
  virtual void paintRightLabel(QPainter &p, const QRectF &r, double w) { Q_UNUSED(p); Q_UNUSED(r); Q_UNUSED(w); }
  virtual void paintTopScale(QPainter &p, const QRectF &r, double w) { Q_UNUSED(p); Q_UNUSED(r); Q_UNUSED(w); }
  virtual void paintBottomScale(QPainter &p, const QRectF &r, double w) { Q_UNUSED(p); Q_UNUSED(r); Q_UNUSED(w); }
  virtual void paintLeftScale(QPainter &p, const QRectF &r, double w) { Q_UNUSED(p); Q_UNUSED(r); Q_UNUSED(w); }
  virtual void paintRightScale(QPainter &p, const QRectF &r, double w) { Q_UNUSED(p); Q_UNUSED(r); Q_UNUSED(w); }

  void drawTicks(QPainter &p, const TickList &tickList, int w=0, bool inverted=false);

  // QWidget interface
public:
  QSize sizeHint() const override;
  static void drawTicks(QPainter &p, const TickList &tickList, const QRectF &r, bool ticksOnTop);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(PlotWidgetBase::Areas)
#endif // PLOTWIDGETBASE_H
