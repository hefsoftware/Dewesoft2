#pragma once
namespace HEF
{
  template <class T> T roundUp(T value, T div) { return ((value+div-1)/div)*div; }
  template <class T> T roundDown(T value, T div) { return (value/div)*div; }
}
