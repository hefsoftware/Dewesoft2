#pragma once
#include <Qt>
namespace DataFile
{
  quint64 encodeFloatingPointPortable(double value);
  quint64 encodeFloatingPoint(double value);
  double decodeFloatingPointPortable(quint64 value);
  double decodeFloatingPoint(quint64 value);

  quint64 encodeFloatingPointFast(double value);
  double decodeFloatingPointFast(quint64 value);
}
