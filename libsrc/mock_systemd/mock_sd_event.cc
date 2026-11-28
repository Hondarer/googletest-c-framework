#include <mock_systemd.h>

#ifndef _WIN32

int delegate_real_sd_event_new(const char *file, const int line, const char *func, sd_event **e)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_new(e);
}

int mock_sd_event_new(const char *file, const int line, const char *func, sd_event **e)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_new(file, line, func, e);
    }
    else
    {
        mock_ret = delegate_real_sd_event_new(file, line, func, e);
    }

    return mock_ret;
}

sd_event *delegate_real_sd_event_unref(const char *file, const int line, const char *func, sd_event *e)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_unref(e);
}

sd_event *mock_sd_event_unref(const char *file, const int line, const char *func, sd_event *e)
{
    sd_event *mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_unref(file, line, func, e);
    }
    else
    {
        mock_ret = delegate_real_sd_event_unref(file, line, func, e);
    }

    return mock_ret;
}

int delegate_real_sd_event_add_io(const char *file, const int line, const char *func, sd_event *e, sd_event_source **s,
                                  int fd, uint32_t events, sd_event_io_handler_t callback, void *userdata)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_add_io(e, s, fd, events, callback, userdata);
}

int mock_sd_event_add_io(const char *file, const int line, const char *func, sd_event *e, sd_event_source **s, int fd,
                         uint32_t events, sd_event_io_handler_t callback, void *userdata)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_add_io(file, line, func, e, s, fd, events, callback, userdata);
    }
    else
    {
        mock_ret = delegate_real_sd_event_add_io(file, line, func, e, s, fd, events, callback, userdata);
    }

    return mock_ret;
}

int delegate_real_sd_event_add_time(const char *file, const int line, const char *func, sd_event *e,
                                    sd_event_source **s, clockid_t clock, uint64_t usec, uint64_t accuracy,
                                    sd_event_time_handler_t callback, void *userdata)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_add_time(e, s, clock, usec, accuracy, callback, userdata);
}

int mock_sd_event_add_time(const char *file, const int line, const char *func, sd_event *e, sd_event_source **s,
                           clockid_t clock, uint64_t usec, uint64_t accuracy, sd_event_time_handler_t callback,
                           void *userdata)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_add_time(file, line, func, e, s, clock, usec, accuracy, callback, userdata);
    }
    else
    {
        mock_ret = delegate_real_sd_event_add_time(file, line, func, e, s, clock, usec, accuracy, callback, userdata);
    }

    return mock_ret;
}

int delegate_real_sd_event_now(const char *file, const int line, const char *func, sd_event *e, clockid_t clock,
                               uint64_t *usec)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_now(e, clock, usec);
}

int mock_sd_event_now(const char *file, const int line, const char *func, sd_event *e, clockid_t clock, uint64_t *usec)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_now(file, line, func, e, clock, usec);
    }
    else
    {
        mock_ret = delegate_real_sd_event_now(file, line, func, e, clock, usec);
    }

    return mock_ret;
}

int delegate_real_sd_event_loop(const char *file, const int line, const char *func, sd_event *e)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_loop(e);
}

int mock_sd_event_loop(const char *file, const int line, const char *func, sd_event *e)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_loop(file, line, func, e);
    }
    else
    {
        mock_ret = delegate_real_sd_event_loop(file, line, func, e);
    }

    return mock_ret;
}

int delegate_real_sd_event_exit(const char *file, const int line, const char *func, sd_event *e, int code)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_exit(e, code);
}

int mock_sd_event_exit(const char *file, const int line, const char *func, sd_event *e, int code)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_exit(file, line, func, e, code);
    }
    else
    {
        mock_ret = delegate_real_sd_event_exit(file, line, func, e, code);
    }

    return mock_ret;
}

sd_event *delegate_real_sd_event_source_get_event(const char *file, const int line, const char *func,
                                                  sd_event_source *s)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_get_event(s);
}

sd_event *mock_sd_event_source_get_event(const char *file, const int line, const char *func, sd_event_source *s)
{
    sd_event *mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_source_get_event(file, line, func, s);
    }
    else
    {
        mock_ret = delegate_real_sd_event_source_get_event(file, line, func, s);
    }

    return mock_ret;
}

int delegate_real_sd_event_source_set_time(const char *file, const int line, const char *func, sd_event_source *s,
                                           uint64_t usec)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_set_time(s, usec);
}

int mock_sd_event_source_set_time(const char *file, const int line, const char *func, sd_event_source *s, uint64_t usec)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_source_set_time(file, line, func, s, usec);
    }
    else
    {
        mock_ret = delegate_real_sd_event_source_set_time(file, line, func, s, usec);
    }

    return mock_ret;
}

int delegate_real_sd_event_source_set_enabled(const char *file, const int line, const char *func, sd_event_source *s,
                                              int enabled)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_set_enabled(s, enabled);
}

int mock_sd_event_source_set_enabled(const char *file, const int line, const char *func, sd_event_source *s,
                                     int enabled)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_source_set_enabled(file, line, func, s, enabled);
    }
    else
    {
        mock_ret = delegate_real_sd_event_source_set_enabled(file, line, func, s, enabled);
    }

    return mock_ret;
}

int delegate_real_sd_event_source_set_priority(const char *file, const int line, const char *func, sd_event_source *s,
                                               int64_t priority)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_set_priority(s, priority);
}

int mock_sd_event_source_set_priority(const char *file, const int line, const char *func, sd_event_source *s,
                                      int64_t priority)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_source_set_priority(file, line, func, s, priority);
    }
    else
    {
        mock_ret = delegate_real_sd_event_source_set_priority(file, line, func, s, priority);
    }

    return mock_ret;
}

sd_event_source *delegate_real_sd_event_source_unref(const char *file, const int line, const char *func,
                                                     sd_event_source *s)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_unref(s);
}

sd_event_source *mock_sd_event_source_unref(const char *file, const int line, const char *func, sd_event_source *s)
{
    sd_event_source *mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_source_unref(file, line, func, s);
    }
    else
    {
        mock_ret = delegate_real_sd_event_source_unref(file, line, func, s);
    }

    return mock_ret;
}

int delegate_real_sd_event_add_signal(const char *file, const int line, const char *func, sd_event *e,
                                      sd_event_source **s, int sig, sd_event_signal_handler_t callback, void *userdata)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_add_signal(e, s, sig, callback, userdata);
}

int mock_sd_event_add_signal(const char *file, const int line, const char *func, sd_event *e, sd_event_source **s,
                             int sig, sd_event_signal_handler_t callback, void *userdata)
{
    int mock_ret;

    if (_mock_systemd != nullptr)
    {
        mock_ret = _mock_systemd->sd_event_add_signal(file, line, func, e, s, sig, callback, userdata);
    }
    else
    {
        mock_ret = delegate_real_sd_event_add_signal(file, line, func, e, s, sig, callback, userdata);
    }

    return mock_ret;
}

#endif // _WIN32
