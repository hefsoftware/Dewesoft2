#include "event.h"
using namespace Plot;
QPointF MouseEvent::pos() const
{
  return m_pos;
}

Qt::MouseButtons MouseEvent::buttons() const
{
  return m_buttons;
}

Qt::MouseButton MouseEvent::button() const
{
  return m_button;
}

Qt::KeyboardModifiers MouseEvent::modifiers() const
{
  return m_modifiers;
}

QPointF MouseEvent::screenPos() const
{
  return m_screenPos;
}
