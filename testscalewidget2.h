#ifndef TESTSCALEWIDGET2_H
#define TESTSCALEWIDGET2_H

#include <QWidget>
#include <Plotlib/scrollmanager.h>
#include "Plotlib/ticklist.h"
#include "Mathlib/function.h"
#include <QMap>
#include <QSet>
class TestScaleWidget2 : public QWidget
{
  Q_OBJECT
public:
  TestScaleWidget2(QWidget *parent=nullptr);
  double ticksDistance() const;
  void setTicksDistance(double ticksDistance);

  int leftMargin() const;
  void setLeftMargin(int leftMargin);
  int rightMargin() const;
  void setRightMargin(int rightMargin);

  double mainTickDist() const;
  void setMainTickDist(double mainTickDist);

  double subTickDist() const;
  void setSubTickDist(double subTickDist);

  void setManual(const QSet<double> &ticks=QSet<double>(), const QSet<double> &ticksSub=QSet<double>());
  int leftPosition();
  int rightPosition();
  QMap<double, double> mainTicks() const;
  QMap<double, double> subTicks() const;

  QColor background() const;

  void setBackground(const QColor &background);

  double scale() const;
  void setScale(double scale);

  double offset() const;
  void setOffset(double offset);
  void setScroll(double scale, double offset);

  void setFunction(const HEF::FunctionPtr &function);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override ;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  QSize sizeHint() const override;
  int m_leftMargin;
  int m_rightMargin;
//   void keyPressEvent(QKeyEvent *event);
  double m_scale;
  double m_offset;
  double m_ticksDistance;
  double m_mainTickDist;
  double m_subTickDist;

  ScrollManager m_scroll;
  QSet<double> m_manualMain;
  QSet<double> m_manualSub;
  QColor m_background;
  bool m_dirty;
  bool m_okCalculation;
//  QMap<double, double> m_mainTicks, m_subTicks;
//  QMap<double, double> m_calcMainTicks, m_calcSubTicks;

  TickList m_calcTicks;
  TickList m_ticks;


  HEF::FunctionPtr m_function;


    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
signals:
    void changedLayout();
};

#endif // TESTSCALEWIDGET2_H
