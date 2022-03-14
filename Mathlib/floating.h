// Floating point encoding/decoding utilities
#pragma once
#include <Qt>
namespace HEF
{
  // These functions will check if simple typecasting of a value is feasible and use this fast method if possible
  quint64 encodeFloating64(double value);
  double decodeFloating64(quint64 value);

  // Slow but portable method
  quint64 encodeFloating64Portable(double value);
  double decodeFloating64Portable(quint64 value);

  // Following function should only be called for testing purpouses. Use encodeFloating/decodeFloating instead
  quint64 encodeFloating64Fast(double value);
  double decodeFloating64Fast(quint64 value);
}

