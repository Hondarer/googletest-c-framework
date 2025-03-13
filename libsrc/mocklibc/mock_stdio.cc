#include <gmock/gmock.h>

#include <mock_stdio.h>

using namespace testing;

Mock_stdio *_mock_stdio = nullptr;

Mock_stdio::Mock_stdio()
{
    ON_CALL(*this, fclose(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_fclose));

    ON_CALL(*this, fflush(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_fflush));

    ON_CALL(*this, fopen(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_fopen));
    reset_fake_fopen();

    ON_CALL(*this, fprintf(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_fprintf));

    ON_CALL(*this, vfprintf(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_vfprintf));

    ON_CALL(*this, scanf(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_scanf));

    _mock_stdio = this;
}

void Mock_stdio::switch_to_mock_fileio()
{
    ON_CALL(*this, fclose(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_fclose));

    ON_CALL(*this, fflush(_, _, _, _))
        .WillByDefault(Invoke(delegate_fake_fflush));

    ON_CALL(*this, fopen(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_fopen));
    reset_fake_fopen();

    ON_CALL(*this, fprintf(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_fprintf));

    ON_CALL(*this, vfprintf(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_fake_vfprintf));
}

void Mock_stdio::switch_to_real_fileio()
{
    ON_CALL(*this, fclose(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_fclose));

    ON_CALL(*this, fflush(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_fflush));

    ON_CALL(*this, fopen(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_fopen));

    ON_CALL(*this, fprintf(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_fprintf));

    ON_CALL(*this, vfprintf(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_vfprintf));
}

Mock_stdio::~Mock_stdio()
{
    _mock_stdio = nullptr;
}
