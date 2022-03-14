#ifndef PLOTWIDGETBASIC_H
#define PLOTWIDGETBASIC_H

#include "plotwidgetbase.h"

class PlotWidgetBasic : public PlotWidgetBase
{
  Q_OBJECT
  /*
    Plot area=50 to infinite
    Label height=10 to 20
  */
public:
  PlotWidgetBasic(QWidget *parent=nullptr);
  enum ScaleResizePolicy
  {
    KeepRange, // Keeps minimum and maximum showed values (rescales)
    KeepScale  // Keeps the scaling
  };

  // BaseWidget interface
protected:
  bool dragStart(const QMouseEvent &event) override;
  void dragEnd(const QMouseEvent &event) override;
  void mouseClick(const QMouseEvent &event) override;
  void mouseMove(const QMouseEvent &event) override;

  // PlotWidgetBase interface
protected:
  HEF::FunctionPtr m_scaleX, m_scaleY,m_scaleX2, m_scaleY2;
  HEF::FunctionPtr m_realScaleX, m_realScaleY,m_realScaleX2, m_realScaleY2;
  ScaleResizePolicy m_policyX, m_policyY;

  TickList m_ticksX, m_ticksY;
  TickList m_ticksX2, m_ticksY2;
  bool m_dirtyX, m_dirtyY, m_dirtyX2, m_dirtyY2;

  void paintMainLabel(QPainter &p, const QRectF &r, double w) override;
  void paintTopLabel(QPainter &p, const QRectF &r, double w) override;
  void paintBottomLabel(QPainter &p, const QRectF &r, double w) override;
  void paintLeftLabel(QPainter &p, const QRectF &r, double w) override;
  void paintRightLabel(QPainter &p, const QRectF &r, double w) override;
  void paintBottomScale(QPainter &p, const QRectF &r, double w) override;
  void paintTopScale(QPainter &p, const QRectF &r, double w) override;
  void paintLeftScale(QPainter &p, const QRectF &r, double w) override;
  void paintRightScale(QPainter &p, const QRectF &r, double w) override;

  void paintLabel(QPainter &p, const QRectF &r, double w, const QString &text);
  void paintScale(QPainter &p, const QRectF &r, double w, const HEF::FunctionPtr &f, HEF::FunctionPtr &realFunction, TickList &ticks, bool &dirty, ScaleResizePolicy policy, bool mirrorX, bool mirrorY);

  /* Note function controlling the showing and hide of elements */
  bool hasLabel();
  int numVerticalScales();
  int numVerticalLabels();
  int numHorizontalScales();
  int numHorizontalLabels();

  double minRequiredWidth();
  double minRequiredHeight();
  double labelHeight() override;
  double topLabelHeight() override;
  double bottomLabelHeight() override;
  double leftLabelHeight() override;
  double rightLabelHeight() override;
  double topScaleHeight() override;
  double bottomScaleHeight() override;
  double leftScaleHeight() override;
  double rightScaleHeight() override;

  virtual double plotMinHeight() { return 50; }
  virtual double plotMinWidth() { return 100; }
  virtual double mainLabelMinHeight() { return 10; }
  virtual double mainLabelMaxHeight() { return 25; }
  virtual double labelMinHeight() { return 8; }
  virtual double labelMaxHeight() { return 15; }
  virtual double scaleMinHeight() { return 15; }
  virtual double scaleMaxHeight() { return 20; }

  // QWidget interface
protected:
  void resizeEvent(QResizeEvent *event) override;

  // PlotWidgetBase interface
protected:
  HEF::FunctionPtr scaleX(int i) override;
  HEF::FunctionPtr scaleY(int i) override;
};

#endif // PLOTWIDGETBASIC_H
