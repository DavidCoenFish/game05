#pragma once

class HandleCloser
{
public:
    typedef HANDLE pointer;
    void operator()(HANDLE in_handle);
};

