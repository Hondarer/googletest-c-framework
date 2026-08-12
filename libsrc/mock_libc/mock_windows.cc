#ifdef _WIN32

    #include <mock_windows.h>

using namespace testing;

Mock_windows *_mock_windows = nullptr;

Mock_windows::Mock_windows()
{
    switch_to_real_time();

    ON_CALL(*this, GetStdHandle(_, _, _, _)).WillByDefault(Invoke(delegate_real_GetStdHandle));
    ON_CALL(*this, GetConsoleMode(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_GetConsoleMode));
    ON_CALL(*this, SetConsoleMode(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_SetConsoleMode));
    ON_CALL(*this, WaitForSingleObject(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_WaitForSingleObject));
    ON_CALL(*this, ReadFile(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_ReadFile));
    ON_CALL(*this, CreateFileMappingA(_, _, _, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_CreateFileMappingA));
    ON_CALL(*this, MapViewOfFile(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_MapViewOfFile));
    ON_CALL(*this, UnmapViewOfFile(_, _, _, _)).WillByDefault(Invoke(delegate_real_UnmapViewOfFile));
    ON_CALL(*this, FlushViewOfFile(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_FlushViewOfFile));
    ON_CALL(*this, FlushFileBuffers(_, _, _, _)).WillByDefault(Invoke(delegate_real_FlushFileBuffers));
    ON_CALL(*this, CloseHandle(_, _, _, _)).WillByDefault(Invoke(delegate_real_CloseHandle));
    ON_CALL(*this, GetLastError(_, _, _)).WillByDefault(Invoke(delegate_real_GetLastError));

    _mock_windows = this;
}

void Mock_windows::switch_to_real_time()
{
    ON_CALL(*this, GetTickCount64(_, _, _)).WillByDefault(Invoke(delegate_real_GetTickCount64));

    ON_CALL(*this, GetSystemTimeAsFileTime(_, _, _, _)).WillByDefault(Invoke(delegate_real_GetSystemTimeAsFileTime));
}

void Mock_windows::switch_to_mock_time()
{
    ON_CALL(*this, GetTickCount64(_, _, _)).WillByDefault(Invoke(delegate_fake_GetTickCount64));

    ON_CALL(*this, GetSystemTimeAsFileTime(_, _, _, _)).WillByDefault(Invoke(delegate_fake_GetSystemTimeAsFileTime));
}

Mock_windows::~Mock_windows()
{
    _mock_windows = nullptr;
}

#endif // _WIN32
