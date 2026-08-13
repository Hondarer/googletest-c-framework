#include <mock_stdio.h>

using namespace testing;

Mock_stdio *_mock_stdio = nullptr;

Mock_stdio::Mock_stdio()
{
    switch_to_real_fileio();

    ON_CALL(*this, printf(_, _, _, _)).WillByDefault(Invoke(delegate_real_printf));

    ON_CALL(*this, scanf(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_scanf));

    ON_CALL(*this, vscanf(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_vscanf));

    ON_CALL(*this, vfscanf(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_vfscanf));

    _mock_stdio = this;
}

void Mock_stdio::switch_to_mock_fileio()
{
    ON_CALL(*this, fclose(_, _, _, _)).WillByDefault(Invoke(delegate_fake_fclose));

    ON_CALL(*this, feof(_, _, _, _)).WillByDefault(Invoke(delegate_fake_feof));

    ON_CALL(*this, ferror(_, _, _, _)).WillByDefault(Invoke(delegate_fake_ferror));

    ON_CALL(*this, fflush(_, _, _, _)).WillByDefault(Invoke(delegate_fake_fflush));

    ON_CALL(*this, fopen(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_fopen));

#ifdef _WIN32
    ON_CALL(*this, fopen_s(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake_fopen_s));
    ON_CALL(*this, _wfopen_s(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake__wfopen_s));
    ON_CALL(*this, _wfsopen(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake__wfsopen));
#endif

    ON_CALL(*this, fprintf(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_fprintf));

    ON_CALL(*this, vfprintf(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_vfprintf));

    /* snprintf はファイル I/O ではないため、fake モードでも実物へ委譲する */
    ON_CALL(*this, snprintf(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_snprintf));

    ON_CALL(*this, vsnprintf(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake_vsnprintf));

    ON_CALL(*this, fgets(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake_fgets));

    ON_CALL(*this, fread(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake_fread));

    ON_CALL(*this, fwrite(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake_fwrite));

    ON_CALL(*this, freopen(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake_freopen));

    ON_CALL(*this, remove(_, _, _, _)).WillByDefault(Invoke(delegate_fake_remove));

    ON_CALL(*this, rename(_, _, _, _, _)).WillByDefault(Invoke(delegate_fake_rename));

#ifndef _WIN32
    ON_CALL(*this, fseeko(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake_fseeko));

    ON_CALL(*this, ftello(_, _, _, _)).WillByDefault(Invoke(delegate_fake_ftello));
#else
    ON_CALL(*this, _fseeki64(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_fake__fseeki64));

    ON_CALL(*this, _ftelli64(_, _, _, _)).WillByDefault(Invoke(delegate_fake__ftelli64));
#endif
}

void Mock_stdio::switch_to_real_fileio()
{
    ON_CALL(*this, fclose(_, _, _, _)).WillByDefault(Invoke(delegate_real_fclose));

    ON_CALL(*this, feof(_, _, _, _)).WillByDefault(Invoke(delegate_real_feof));

    ON_CALL(*this, ferror(_, _, _, _)).WillByDefault(Invoke(delegate_real_ferror));

    ON_CALL(*this, fflush(_, _, _, _)).WillByDefault(Invoke(delegate_real_fflush));

    ON_CALL(*this, fopen(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_fopen));

#ifdef _WIN32
    ON_CALL(*this, fopen_s(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_fopen_s));
    ON_CALL(*this, _wfopen_s(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real__wfopen_s));
    ON_CALL(*this, _wfsopen(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real__wfsopen));
#endif

    ON_CALL(*this, fprintf(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_fprintf));

    ON_CALL(*this, vfprintf(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_vfprintf));

    ON_CALL(*this, snprintf(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_snprintf));

    ON_CALL(*this, vsnprintf(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_vsnprintf));

    ON_CALL(*this, fgets(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_fgets));

    ON_CALL(*this, fread(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_fread));

    ON_CALL(*this, fwrite(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_fwrite));

    ON_CALL(*this, freopen(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_freopen));

    ON_CALL(*this, remove(_, _, _, _)).WillByDefault(Invoke(delegate_real_remove));

    ON_CALL(*this, rename(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_rename));

#ifndef _WIN32
    ON_CALL(*this, fseeko(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_fseeko));

    ON_CALL(*this, ftello(_, _, _, _)).WillByDefault(Invoke(delegate_real_ftello));
#else
    ON_CALL(*this, _fseeki64(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real__fseeki64));

    ON_CALL(*this, _ftelli64(_, _, _, _)).WillByDefault(Invoke(delegate_real__ftelli64));
#endif
}

Mock_stdio::~Mock_stdio()
{
    _mock_stdio = nullptr;
}
