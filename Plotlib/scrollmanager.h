#ifndef SCROLLMANAGER_H
#define SCROLLMANAGER_H

#include <functional>

class ScrollManager /*: public QObject*/
{
public:
  explicit ScrollManager(std::function<bool(double, double)> notifyFunc=nullptr);
  void setNotifyFunc(const std::function<bool (double, double)> &notifyFunc);
  void setNotifyFunc(const std::function<bool ()> &notifyFunc);
  void setNotifyFunc(const std::function<void ()> &notifyFunc);

  void startScroll(double mousePos);
  void updateScroll(double mousePos);
  void endScroll(double mousePos);
  void endScroll();
  void abortScroll(double mousePos);

  void scale(double mousePos, double scale);
  void scaleRelative(double mousePos, double scaleMul);
  void scaleWheelUnit(double mousePos, double unit);
  void scaleWheelAngle(double mousePos, double angle);

  bool isScrolling() const;

  double currentScale();
  double currentOffset();
//signals:
//  void changedView(double scale, double offset);

  void setScale(double scale);
  void setOffset(double offset);
  void setScroll(double scale, double offset);

protected:
  void recalculateScroll(double mousePos, double newScale);

  std::function<bool(double, double)> m_notifyFunc;

  double m_scale;      // Current scale
  double m_offset;     // Current offset

  bool m_isScrolling;

  double m_scrollMouseStart; // Coordinate of mouse at start of scrolling

  double m_scrollScaleBackup; // Copy of m_scale at the moment of scroll start
  double m_scrollOffsetBackup; // Copy of m_offset at the moment of scroll start
};

#endif // SCROLLMANAGER_H
