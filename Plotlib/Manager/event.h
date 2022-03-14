#pragma once
#include <Qt>
#include <QPointF>
namespace Plot
{
  class MouseEvent
  {
  public:
    MouseEvent(const QPointF &screenPos, const QPointF &pos, const Qt::MouseButton &button, const Qt::MouseButtons &buttons, const Qt::KeyboardModifiers &modifiers): m_screenPos(screenPos), m_pos(pos), m_button(button), m_buttons(buttons), m_modifiers(modifiers) { }
    QPointF screenPos() const;
    QPointF pos() const;
    Qt::MouseButton button() const;
    Qt::MouseButtons buttons() const;
    Qt::KeyboardModifiers modifiers() const;


  protected:
    QPointF m_screenPos;
    QPointF m_pos;
    Qt::MouseButton m_button;
    Qt::MouseButtons m_buttons;
    Qt::KeyboardModifiers m_modifiers;
  };
}
