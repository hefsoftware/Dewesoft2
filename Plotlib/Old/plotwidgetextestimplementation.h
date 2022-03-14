#ifndef PLOTWIDGETEXTESTIMPLEMENTATION_H
#define PLOTWIDGETEXTESTIMPLEMENTATION_H

#include "Manager/singlearea.h"
class QMenu;
class PlotWidgetExTestImplementation : public Plot::SingleArea
{
public:
  PlotWidgetExTestImplementation(QWidget *parent = nullptr);
protected:
  bool dragStart(const Plot::MouseEvent &event) override;
  void dragEnd(const Plot::MouseEvent &event) override;
  void mouseClick(const Plot::MouseEvent &event) override;
  void prepareMenu(QMenu &menu, Plot::Band *band, const QString &suffix="");
  // QWidget interface
protected:
  void wheelEvent(const QWheelEvent &event) override;
};

#endif // PLOTWIDGETEXTESTIMPLEMENTATION_H
