#ifndef SCALEWIDGET_H
#define SCALEWIDGET_H

#include <QWidget>
#include <QMap>
#include "Mathlib/function.h"

class ScaleWidget : public QWidget
{
  Q_OBJECT
public:
  explicit ScaleWidget(QWidget *parent = nullptr);
  QSize minimumSizeHint() const override;
protected:
  void recalculateScale();
  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  static constexpr int m_border=25;
  static constexpr int m_lineX=40;
  HEF::FunctionPtr m_function;
  QMap<double, double> m_ticks;
  bool m_dirty;
};

#endif // SCALEWIDGET_H
