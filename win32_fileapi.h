/* date = May 22nd 2023 9:38 pm */

#ifndef WIN32_FILEAPI_H
#define WIN32_FILEAPI_H

#include <windows.h>
#include "typinggame_types.h"
struct win32_file{
    u8 *Data;
    HANDLE Handle;
    DWORD Size;
};


#endif //WIN32_FILEAPI_H
