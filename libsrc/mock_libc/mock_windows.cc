#include <mock_instance.h>
#ifdef _WIN32

    #include <mock_windows.h>

using namespace testing;

Mock_windows *_mock_windows = nullptr;

Mock_windows::Mock_windows()
{
    switch_to_real_time();

    ON_CALL(*this, GetStdHandle(_, _, _, _)).WillByDefault(Invoke(delegate_real_GetStdHandle));
    ON_CALL(*this, GetConsoleMode(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_GetConsoleMode));
    ON_CALL(*this, GetConsoleScreenBufferInfo(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_GetConsoleScreenBufferInfo));
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
    ON_CALL(*this, GetModuleFileNameW(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_GetModuleFileNameW));
    ON_CALL(*this, GetExitCodeProcess(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_GetExitCodeProcess));
    ON_CALL(*this, TerminateProcess(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_TerminateProcess));
    ON_CALL(*this, InitializeProcThreadAttributeList(_, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_InitializeProcThreadAttributeList));
    ON_CALL(*this, UpdateProcThreadAttribute(_, _, _, _, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_UpdateProcThreadAttribute));
    ON_CALL(*this, DeleteProcThreadAttributeList(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_DeleteProcThreadAttributeList));
    ON_CALL(*this, CreateProcessW(_, _, _, _, _, _, _, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_CreateProcessW));
    ON_CALL(*this, GetCurrentProcess(_, _, _)).WillByDefault(Invoke(delegate_real_GetCurrentProcess));
    ON_CALL(*this, DuplicateHandle(_, _, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_DuplicateHandle));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_windows);
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
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_windows);
}

#endif // _WIN32
