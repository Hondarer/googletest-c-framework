/* syslog モックライブラリ
 *
 * LD_PRELOAD でロードすることで syslog() をインターセプトする。
 * 環境変数 SYSLOG_TEST_FD にパイプの書き込み端 FD 番号を設定すると、
 * syslog() の出力をそのパイプに書き込む。
 */
#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

/* syslog() の差し替え実装 */
void syslog(int priority, const char *fmt, ...)
{
    (void)priority;

    va_list ap;
    va_start(ap, fmt);

    const char *fd_str = getenv("SYSLOG_TEST_FD");
    if (fd_str != NULL)
    {
        int fd = atoi(fd_str);
        if (fd >= 0)
        {
            char buf[4096];
            int n = vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
            if (n > 0)
            {
                if ((size_t)n >= sizeof(buf) - 1) {
                    n = (int)(sizeof(buf) - 2);
                }
                buf[n]     = '\n';
                buf[n + 1] = '\0';
                write(fd, buf, (size_t)(n + 1));
            }
        }
    }

    va_end(ap);
}
