#ifndef ERROR_H
#define ERROR_H
namespace DataFile
{
  enum class Error: int
  {
    NeedExtraProcess=-1, // Special return value. Calling function should calculate the value.
    Success=0,
    Generic, // Generic error
    Params, // Function called with invalid parameters
    Interrupted,
    InvalidIndex,
    Memory,
    Disk,
    Lock,
    Unimplemented
  };
}
#endif // ERROR_H
