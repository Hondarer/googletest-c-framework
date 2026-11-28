#include <mock_systemd.h>

#ifndef _WIN32

int delegate_real_sd_notify(const char *file, const int line, const char *func, int unset_environment,
                            const char *state)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_notify(unset_environment, state);
}

int mock_sd_notify(const char *file, const int line, const char *func, int unset_environment, const char *state)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_notify(file, line, func, unset_environment, state);
    }
    else
    {
        mock_ret = delegate_real_sd_notify(file, line, func, unset_environment, state);
    }

    return mock_ret;
}

int delegate_real_sd_watchdog_enabled(const char *file, const int line, const char *func, int unset_environment,
                                      uint64_t *usec)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_watchdog_enabled(unset_environment, usec);
}

int mock_sd_watchdog_enabled(const char *file, const int line, const char *func, int unset_environment, uint64_t *usec)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_watchdog_enabled(file, line, func, unset_environment, usec);
    }
    else
    {
        mock_ret = delegate_real_sd_watchdog_enabled(file, line, func, unset_environment, usec);
    }

    return mock_ret;
}

#endif // _WIN32
