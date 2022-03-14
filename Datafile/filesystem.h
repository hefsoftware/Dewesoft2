#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "entry.h"
namespace DataFile
{
  class FileSystem: public Entry
  {
  public:
    FileSystem(EntryPtr parent);
  };
}
#endif // FILESYSTEM_H
