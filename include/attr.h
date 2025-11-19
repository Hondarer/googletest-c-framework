#ifndef _ATTR_H_
#define _ATTR_H_

#if defined(_WIN32)
  #include <sal.h>
  #define PRINTF_FMT   _Printf_format_string_
  #define PRINTF_ATTR(fmt_index, first_arg) /* none on MSVC */
  #define SCANF_FMT    _Scanf_format_string_
  #define SCANF_ATTR(fmt_index, first_arg)  /* none on MSVC */
#else // _WIN32
  #define PRINTF_FMT
  #define PRINTF_ATTR(fmt_index, first_arg) __attribute__((format(printf, fmt_index, first_arg)))
  #define SCANF_FMT
  #define SCANF_ATTR(fmt_index, first_arg)  __attribute__((format(scanf, fmt_index, first_arg)))
#endif // _WIN32

#endif // _ATTR_H_
