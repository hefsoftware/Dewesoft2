#ifndef MANAGERBASIC_H
#define MANAGERBASIC_H

#include "../singlearea.h"
namespace Plot
{
  class ManagerBasic : public SingleArea
  {
    Q_OBJECT
  public:
    ManagerBasic(QObject *parent=nullptr);
  protected:
    bool dragStart(const Plot::MouseEvent &event) override;
    void dragEnd(const Plot::MouseEvent &event) override;
    void mouseClick(const Plot::MouseEvent &event) override;
    void wheelEvent(const QWheelEvent &event) override;

  };
}
#endif // MANAGERBASIC_H
