#include "common\common_pch.h"
#include "common\direct_xtk12\handle_closer.h"

void HandleCloser::operator()(HANDLE in_handle)
{
    if (in_handle)
    {
        CloseHandle(in_handle);
    }
    return;
}

