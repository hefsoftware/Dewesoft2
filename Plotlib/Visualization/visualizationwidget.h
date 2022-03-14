#pragma once
#include <QWidget>
#include "visualization.h"

class PlotWidget: public QWidget, Plot::PlotVisualizationPublic
{
  Q_OBJECT
public:
  PlotWidget(QWidget *parent=nullptr);
  PlotWidget(Plot::Manager *manager, QWidget *parent=nullptr);
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

protected:
  void layoutChanged() override; // Called by manager when layout has changed
  void notifyUpdate(QSet<Plot::Band *> band) override; // Called by manager when one or more bands needs updating
  bool updateSizeCache() const; // Updates m_bestSize and m_minSize. Returns true if one or both were changed
  mutable QSize m_bestSize;
  mutable QSize m_minSize;

  // QWidget interface
protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
};
