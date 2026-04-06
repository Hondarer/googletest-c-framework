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
    va_list ap;
    va_start(ap, fmt);

    const char *fd_str = getenv("SYSLOG_TEST_FD");
    if (fd_str != NULL)
    {
        int fd = atoi(fd_str);
        if (fd >= 0)
        {
            /* スタックバッファに <priority>message\n を 1 度に書き込む。
             * 複数スレッドからの write() 呼び出しが interleave しないよう
             * 単一の write() で完結させる。 */
            char buf[4096];

            /* 先頭に <priority> プレフィックスを書く */
            int prefix_len = snprintf(buf, sizeof(buf), "<%d>", priority);
            if (prefix_len < 0 || (size_t)prefix_len >= sizeof(buf) - 2)
            {
                prefix_len = 0;
            }

            /* 残り領域にメッセージ本体を展開する */
            char *msg     = buf + prefix_len;
            size_t msg_sz = sizeof(buf) - (size_t)prefix_len - 1; /* \n 用に 1 バイト確保 */
            int n         = vsnprintf(msg, msg_sz, fmt, ap);
            if (n > 0)
            {
                if ((size_t)n >= msg_sz)
                {
                    n = (int)(msg_sz - 1);
                }
                msg[n]     = '\n';
                msg[n + 1] = '\0';
                write(fd, buf, (size_t)(prefix_len + n + 1));
            }
        }
    }

    va_end(ap);
}
