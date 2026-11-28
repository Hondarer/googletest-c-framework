#ifndef _WIN32

    #define _POSIX_C_SOURCE 200809L

    #include <systemd/sd-bus.h>
    #include <systemd/sd-daemon.h>
    #include <systemd/sd-event.h>

    #include "mock_systemd_target.h"

int mock_systemd_target_notify(const char *state)
{
    return sd_notify(0, state);
}

int mock_systemd_target_event_roundtrip(void)
{
    sd_event *ev = NULL;
    int ret = sd_event_new(&ev);
    if (ret < 0)
    {
        return ret;
    }
    sd_event_unref(ev);
    return 0;
}

int mock_systemd_target_bus_is_error(sd_bus_message *m, const char *name)
{
    return sd_bus_message_is_method_error(m, name);
}

#endif /* _WIN32 */
