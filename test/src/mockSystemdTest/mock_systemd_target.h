#ifndef MOCK_SYSTEMD_TARGET_H
#define MOCK_SYSTEMD_TARGET_H

#ifndef _WIN32

    #include <systemd/sd-bus.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    int mock_systemd_target_notify(const char *state);
    int mock_systemd_target_event_roundtrip(void);
    int mock_systemd_target_bus_is_error(sd_bus_message *m, const char *name);

    #ifdef __cplusplus
}
    #endif

#endif /* _WIN32 */

#endif /* MOCK_SYSTEMD_TARGET_H */
