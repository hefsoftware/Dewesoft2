#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QWidget>
#include <QMap>
#include <QMouseEvent>
class BaseWidget : public QWidget
{
  Q_OBJECT
public:
  explicit BaseWidget(QWidget *parent = nullptr);
protected:
  virtual bool dragStart(const QMouseEvent &event);
  virtual void dragEnd(const QMouseEvent &event);
  virtual void mouseClick(const QMouseEvent &event);
  virtual void mouseMove(const QMouseEvent &event);

  static constexpr const int m_dragDistance=4;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  QMap<Qt::MouseButton, QMouseEvent> m_dragStart;
  QMap<Qt::MouseButton, QMouseEvent> m_dragActive;
};

#endif // BASEWIDGET_H
