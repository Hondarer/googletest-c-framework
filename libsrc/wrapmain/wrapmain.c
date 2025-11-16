#include <wrapmain.h>

#ifdef _WIN32
int main(int argc, char **argv)
{
    return __wrap_main(argc, argv);
}
#endif // _WIN32
