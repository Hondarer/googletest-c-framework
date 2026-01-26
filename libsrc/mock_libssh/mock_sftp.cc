#include <test_com.h>
#include <mock_libssh.h>
#include <stdlib.h>
#include <string.h>

using namespace testing;

/* ========================================
 * sftp_new
 * ======================================== */

sftp_session delegate_fake_sftp_new(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;
    (void)session;

    return (sftp_session)malloc(sizeof(struct sftp_session_struct *));
}

sftp_session delegate_real_sftp_new(const char *file, const int line, const char *func, ssh_session session)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_new(session);
#else
    return delegate_fake_sftp_new(file, line, func, session);
#endif
}

sftp_session mock_sftp_new(const char *file, const int line, const char *func, ssh_session session)
{
    sftp_session result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_new(file, line, func, session);
    }
    else
    {
        result = delegate_real_sftp_new(file, line, func, session);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_new %p", (void *)session);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %p\n", file, line, (void *)result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_free
 * ======================================== */

void delegate_fake_sftp_free(const char *file, const int line, const char *func, sftp_session sftp)
{
    (void)file;
    (void)line;
    (void)func;

    if (sftp != nullptr)
    {
        free(sftp);
    }
}

void delegate_real_sftp_free(const char *file, const int line, const char *func, sftp_session sftp)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    sftp_free(sftp);
#else
    delegate_fake_sftp_free(file, line, func, sftp);
#endif
}

void mock_sftp_free(const char *file, const int line, const char *func, sftp_session sftp)
{
    if (_mock_libssh != nullptr)
    {
        _mock_libssh->sftp_free(file, line, func, sftp);
    }
    else
    {
        delegate_real_sftp_free(file, line, func, sftp);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_free %p", (void *)sftp);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }
}

/* ========================================
 * sftp_init
 * ======================================== */

int delegate_fake_sftp_init(const char *file, const int line, const char *func, sftp_session sftp)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;

    return SSH_OK;
}

int delegate_real_sftp_init(const char *file, const int line, const char *func, sftp_session sftp)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_init(sftp);
#else
    return delegate_fake_sftp_init(file, line, func, sftp);
#endif
}

int mock_sftp_init(const char *file, const int line, const char *func, sftp_session sftp)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_init(file, line, func, sftp);
    }
    else
    {
        result = delegate_real_sftp_init(file, line, func, sftp);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_init %p", (void *)sftp);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_get_error
 * ======================================== */

int delegate_fake_sftp_get_error(const char *file, const int line, const char *func, sftp_session sftp)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;

    return SSH_FX_OK;
}

int delegate_real_sftp_get_error(const char *file, const int line, const char *func, sftp_session sftp)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_get_error(sftp);
#else
    return delegate_fake_sftp_get_error(file, line, func, sftp);
#endif
}

int mock_sftp_get_error(const char *file, const int line, const char *func, sftp_session sftp)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_get_error(file, line, func, sftp);
    }
    else
    {
        result = delegate_real_sftp_get_error(file, line, func, sftp);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_get_error %p", (void *)sftp);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_open
 * ======================================== */

sftp_file delegate_fake_sftp_open(const char *file, const int line, const char *func,
                                   sftp_session sftp, const char *filename, int accesstype, mode_t mode)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;
    (void)filename;
    (void)accesstype;
    (void)mode;

    return (sftp_file)malloc(sizeof(struct sftp_file_struct *));
}

sftp_file delegate_real_sftp_open(const char *file, const int line, const char *func,
                                   sftp_session sftp, const char *filename, int accesstype, mode_t mode)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_open(sftp, filename, accesstype, mode);
#else
    return delegate_fake_sftp_open(file, line, func, sftp, filename, accesstype, mode);
#endif
}

sftp_file mock_sftp_open(const char *file, const int line, const char *func,
                          sftp_session sftp, const char *filename, int accesstype, mode_t mode)
{
    sftp_file result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_open(file, line, func, sftp, filename, accesstype, mode);
    }
    else
    {
        result = delegate_real_sftp_open(file, line, func, sftp, filename, accesstype, mode);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_open %s, %d, %o", filename, accesstype, mode);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %p\n", file, line, (void *)result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_close
 * ======================================== */

int delegate_fake_sftp_close(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;

    if (sftpfile != nullptr)
    {
        free(sftpfile);
    }

    return SSH_OK;
}

int delegate_real_sftp_close(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_close(sftpfile);
#else
    return delegate_fake_sftp_close(file, line, func, sftpfile);
#endif
}

int mock_sftp_close(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_close(file, line, func, sftpfile);
    }
    else
    {
        result = delegate_real_sftp_close(file, line, func, sftpfile);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_close %p", (void *)sftpfile);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_read
 * ======================================== */

ssize_t delegate_fake_sftp_read(const char *file, const int line, const char *func,
                                 sftp_file sftpfile, void *buf, size_t count)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftpfile;
    (void)buf;
    (void)count;

    return 0; /* EOF */
}

ssize_t delegate_real_sftp_read(const char *file, const int line, const char *func,
                                 sftp_file sftpfile, void *buf, size_t count)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_read(sftpfile, buf, count);
#else
    return delegate_fake_sftp_read(file, line, func, sftpfile, buf, count);
#endif
}

ssize_t mock_sftp_read(const char *file, const int line, const char *func,
                        sftp_file sftpfile, void *buf, size_t count)
{
    ssize_t result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_read(file, line, func, sftpfile, buf, count);
    }
    else
    {
        result = delegate_real_sftp_read(file, line, func, sftpfile, buf, count);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_read %p, %zu", (void *)sftpfile, count);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %zd\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_write
 * ======================================== */

ssize_t delegate_fake_sftp_write(const char *file, const int line, const char *func,
                                  sftp_file sftpfile, const void *buf, size_t count)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftpfile;
    (void)buf;

    return (ssize_t)count;
}

ssize_t delegate_real_sftp_write(const char *file, const int line, const char *func,
                                  sftp_file sftpfile, const void *buf, size_t count)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_write(sftpfile, buf, count);
#else
    return delegate_fake_sftp_write(file, line, func, sftpfile, buf, count);
#endif
}

ssize_t mock_sftp_write(const char *file, const int line, const char *func,
                         sftp_file sftpfile, const void *buf, size_t count)
{
    ssize_t result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_write(file, line, func, sftpfile, buf, count);
    }
    else
    {
        result = delegate_real_sftp_write(file, line, func, sftpfile, buf, count);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_write %p, %zu", (void *)sftpfile, count);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %zd\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_seek
 * ======================================== */

int delegate_fake_sftp_seek(const char *file, const int line, const char *func,
                             sftp_file sftpfile, uint32_t new_offset)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftpfile;
    (void)new_offset;

    return SSH_OK;
}

int delegate_real_sftp_seek(const char *file, const int line, const char *func,
                             sftp_file sftpfile, uint32_t new_offset)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_seek(sftpfile, new_offset);
#else
    return delegate_fake_sftp_seek(file, line, func, sftpfile, new_offset);
#endif
}

int mock_sftp_seek(const char *file, const int line, const char *func,
                    sftp_file sftpfile, uint32_t new_offset)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_seek(file, line, func, sftpfile, new_offset);
    }
    else
    {
        result = delegate_real_sftp_seek(file, line, func, sftpfile, new_offset);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_seek %p, %u", (void *)sftpfile, new_offset);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_seek64
 * ======================================== */

int delegate_fake_sftp_seek64(const char *file, const int line, const char *func,
                               sftp_file sftpfile, uint64_t new_offset)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftpfile;
    (void)new_offset;

    return SSH_OK;
}

int delegate_real_sftp_seek64(const char *file, const int line, const char *func,
                               sftp_file sftpfile, uint64_t new_offset)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_seek64(sftpfile, new_offset);
#else
    return delegate_fake_sftp_seek64(file, line, func, sftpfile, new_offset);
#endif
}

int mock_sftp_seek64(const char *file, const int line, const char *func,
                      sftp_file sftpfile, uint64_t new_offset)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_seek64(file, line, func, sftpfile, new_offset);
    }
    else
    {
        result = delegate_real_sftp_seek64(file, line, func, sftpfile, new_offset);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_seek64 %p, %lu", (void *)sftpfile, (unsigned long)new_offset);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_tell
 * ======================================== */

unsigned long delegate_fake_sftp_tell(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftpfile;

    return 0;
}

unsigned long delegate_real_sftp_tell(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_tell(sftpfile);
#else
    return delegate_fake_sftp_tell(file, line, func, sftpfile);
#endif
}

unsigned long mock_sftp_tell(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    unsigned long result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_tell(file, line, func, sftpfile);
    }
    else
    {
        result = delegate_real_sftp_tell(file, line, func, sftpfile);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_tell %p", (void *)sftpfile);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lu\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_tell64
 * ======================================== */

uint64_t delegate_fake_sftp_tell64(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftpfile;

    return 0;
}

uint64_t delegate_real_sftp_tell64(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_tell64(sftpfile);
#else
    return delegate_fake_sftp_tell64(file, line, func, sftpfile);
#endif
}

uint64_t mock_sftp_tell64(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    uint64_t result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_tell64(file, line, func, sftpfile);
    }
    else
    {
        result = delegate_real_sftp_tell64(file, line, func, sftpfile);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_tell64 %p", (void *)sftpfile);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %lu\n", file, line, (unsigned long)result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_rewind
 * ======================================== */

void delegate_fake_sftp_rewind(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftpfile;
}

void delegate_real_sftp_rewind(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    sftp_rewind(sftpfile);
#else
    delegate_fake_sftp_rewind(file, line, func, sftpfile);
#endif
}

void mock_sftp_rewind(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    if (_mock_libssh != nullptr)
    {
        _mock_libssh->sftp_rewind(file, line, func, sftpfile);
    }
    else
    {
        delegate_real_sftp_rewind(file, line, func, sftpfile);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_rewind %p", (void *)sftpfile);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }
}

/* ========================================
 * sftp_fstat
 * ======================================== */

sftp_attributes delegate_fake_sftp_fstat(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftpfile;

    sftp_attributes attr = (sftp_attributes)calloc(1, sizeof(struct sftp_attributes_struct));
    return attr;
}

sftp_attributes delegate_real_sftp_fstat(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_fstat(sftpfile);
#else
    return delegate_fake_sftp_fstat(file, line, func, sftpfile);
#endif
}

sftp_attributes mock_sftp_fstat(const char *file, const int line, const char *func, sftp_file sftpfile)
{
    sftp_attributes result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_fstat(file, line, func, sftpfile);
    }
    else
    {
        result = delegate_real_sftp_fstat(file, line, func, sftpfile);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_fstat %p", (void *)sftpfile);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %p\n", file, line, (void *)result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_opendir
 * ======================================== */

sftp_dir delegate_fake_sftp_opendir(const char *file, const int line, const char *func,
                                     sftp_session sftp, const char *path)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;
    (void)path;

    return (sftp_dir)malloc(sizeof(struct sftp_dir_struct *));
}

sftp_dir delegate_real_sftp_opendir(const char *file, const int line, const char *func,
                                     sftp_session sftp, const char *path)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_opendir(sftp, path);
#else
    return delegate_fake_sftp_opendir(file, line, func, sftp, path);
#endif
}

sftp_dir mock_sftp_opendir(const char *file, const int line, const char *func,
                            sftp_session sftp, const char *path)
{
    sftp_dir result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_opendir(file, line, func, sftp, path);
    }
    else
    {
        result = delegate_real_sftp_opendir(file, line, func, sftp, path);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_opendir %s", path);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %p\n", file, line, (void *)result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_readdir
 * ======================================== */

sftp_attributes delegate_fake_sftp_readdir(const char *file, const int line, const char *func,
                                            sftp_session sftp, sftp_dir dir)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;
    (void)dir;

    return nullptr; /* End of directory */
}

sftp_attributes delegate_real_sftp_readdir(const char *file, const int line, const char *func,
                                            sftp_session sftp, sftp_dir dir)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_readdir(sftp, dir);
#else
    return delegate_fake_sftp_readdir(file, line, func, sftp, dir);
#endif
}

sftp_attributes mock_sftp_readdir(const char *file, const int line, const char *func,
                                   sftp_session sftp, sftp_dir dir)
{
    sftp_attributes result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_readdir(file, line, func, sftp, dir);
    }
    else
    {
        result = delegate_real_sftp_readdir(file, line, func, sftp, dir);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_readdir %p", (void *)dir);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %p\n", file, line, (void *)result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_closedir
 * ======================================== */

int delegate_fake_sftp_closedir(const char *file, const int line, const char *func, sftp_dir dir)
{
    (void)file;
    (void)line;
    (void)func;

    if (dir != nullptr)
    {
        free(dir);
    }

    return SSH_OK;
}

int delegate_real_sftp_closedir(const char *file, const int line, const char *func, sftp_dir dir)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_closedir(dir);
#else
    return delegate_fake_sftp_closedir(file, line, func, dir);
#endif
}

int mock_sftp_closedir(const char *file, const int line, const char *func, sftp_dir dir)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_closedir(file, line, func, dir);
    }
    else
    {
        result = delegate_real_sftp_closedir(file, line, func, dir);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_closedir %p", (void *)dir);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_mkdir
 * ======================================== */

int delegate_fake_sftp_mkdir(const char *file, const int line, const char *func,
                              sftp_session sftp, const char *directory, mode_t mode)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;
    (void)directory;
    (void)mode;

    return SSH_OK;
}

int delegate_real_sftp_mkdir(const char *file, const int line, const char *func,
                              sftp_session sftp, const char *directory, mode_t mode)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_mkdir(sftp, directory, mode);
#else
    return delegate_fake_sftp_mkdir(file, line, func, sftp, directory, mode);
#endif
}

int mock_sftp_mkdir(const char *file, const int line, const char *func,
                     sftp_session sftp, const char *directory, mode_t mode)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_mkdir(file, line, func, sftp, directory, mode);
    }
    else
    {
        result = delegate_real_sftp_mkdir(file, line, func, sftp, directory, mode);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_mkdir %s, %o", directory, mode);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_rmdir
 * ======================================== */

int delegate_fake_sftp_rmdir(const char *file, const int line, const char *func,
                              sftp_session sftp, const char *directory)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;
    (void)directory;

    return SSH_OK;
}

int delegate_real_sftp_rmdir(const char *file, const int line, const char *func,
                              sftp_session sftp, const char *directory)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_rmdir(sftp, directory);
#else
    return delegate_fake_sftp_rmdir(file, line, func, sftp, directory);
#endif
}

int mock_sftp_rmdir(const char *file, const int line, const char *func,
                     sftp_session sftp, const char *directory)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_rmdir(file, line, func, sftp, directory);
    }
    else
    {
        result = delegate_real_sftp_rmdir(file, line, func, sftp, directory);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_rmdir %s", directory);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_unlink
 * ======================================== */

int delegate_fake_sftp_unlink(const char *file, const int line, const char *func,
                               sftp_session sftp, const char *filename)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;
    (void)filename;

    return SSH_OK;
}

int delegate_real_sftp_unlink(const char *file, const int line, const char *func,
                               sftp_session sftp, const char *filename)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_unlink(sftp, filename);
#else
    return delegate_fake_sftp_unlink(file, line, func, sftp, filename);
#endif
}

int mock_sftp_unlink(const char *file, const int line, const char *func,
                      sftp_session sftp, const char *filename)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_unlink(file, line, func, sftp, filename);
    }
    else
    {
        result = delegate_real_sftp_unlink(file, line, func, sftp, filename);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_unlink %s", filename);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_rename
 * ======================================== */

int delegate_fake_sftp_rename(const char *file, const int line, const char *func,
                               sftp_session sftp, const char *original, const char *newname)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;
    (void)original;
    (void)newname;

    return SSH_OK;
}

int delegate_real_sftp_rename(const char *file, const int line, const char *func,
                               sftp_session sftp, const char *original, const char *newname)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_rename(sftp, original, newname);
#else
    return delegate_fake_sftp_rename(file, line, func, sftp, original, newname);
#endif
}

int mock_sftp_rename(const char *file, const int line, const char *func,
                      sftp_session sftp, const char *original, const char *newname)
{
    int result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_rename(file, line, func, sftp, original, newname);
    }
    else
    {
        result = delegate_real_sftp_rename(file, line, func, sftp, original, newname);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_rename %s -> %s", original, newname);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_stat
 * ======================================== */

sftp_attributes delegate_fake_sftp_stat(const char *file, const int line, const char *func,
                                         sftp_session sftp, const char *path)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;
    (void)path;

    sftp_attributes attr = (sftp_attributes)calloc(1, sizeof(struct sftp_attributes_struct));
    return attr;
}

sftp_attributes delegate_real_sftp_stat(const char *file, const int line, const char *func,
                                         sftp_session sftp, const char *path)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_stat(sftp, path);
#else
    return delegate_fake_sftp_stat(file, line, func, sftp, path);
#endif
}

sftp_attributes mock_sftp_stat(const char *file, const int line, const char *func,
                                sftp_session sftp, const char *path)
{
    sftp_attributes result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_stat(file, line, func, sftp, path);
    }
    else
    {
        result = delegate_real_sftp_stat(file, line, func, sftp, path);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_stat %s", path);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %p\n", file, line, (void *)result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_lstat
 * ======================================== */

sftp_attributes delegate_fake_sftp_lstat(const char *file, const int line, const char *func,
                                          sftp_session sftp, const char *path)
{
    (void)file;
    (void)line;
    (void)func;
    (void)sftp;
    (void)path;

    sftp_attributes attr = (sftp_attributes)calloc(1, sizeof(struct sftp_attributes_struct));
    return attr;
}

sftp_attributes delegate_real_sftp_lstat(const char *file, const int line, const char *func,
                                          sftp_session sftp, const char *path)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    return sftp_lstat(sftp, path);
#else
    return delegate_fake_sftp_lstat(file, line, func, sftp, path);
#endif
}

sftp_attributes mock_sftp_lstat(const char *file, const int line, const char *func,
                                 sftp_session sftp, const char *path)
{
    sftp_attributes result;

    if (_mock_libssh != nullptr)
    {
        result = _mock_libssh->sftp_lstat(file, line, func, sftp, path);
    }
    else
    {
        result = delegate_real_sftp_lstat(file, line, func, sftp, path);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_lstat %s", path);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %p\n", file, line, (void *)result);
        }
        else
        {
            printf("\n");
        }
    }

    return result;
}

/* ========================================
 * sftp_attributes_free
 * ======================================== */

void delegate_fake_sftp_attributes_free(const char *file, const int line, const char *func, sftp_attributes attr)
{
    (void)file;
    (void)line;
    (void)func;

    if (attr != nullptr)
    {
        if (attr->name != nullptr) free(attr->name);
        if (attr->longname != nullptr) free(attr->longname);
        if (attr->owner != nullptr) free(attr->owner);
        if (attr->group != nullptr) free(attr->group);
        if (attr->acl != nullptr) free(attr->acl);
        if (attr->extended_type != nullptr) free(attr->extended_type);
        if (attr->extended_data != nullptr) free(attr->extended_data);
        free(attr);
    }
}

void delegate_real_sftp_attributes_free(const char *file, const int line, const char *func, sftp_attributes attr)
{
    (void)file;
    (void)line;
    (void)func;

#ifdef USE_REAL_LIBSSH
    sftp_attributes_free(attr);
#else
    delegate_fake_sftp_attributes_free(file, line, func, attr);
#endif
}

void mock_sftp_attributes_free(const char *file, const int line, const char *func, sftp_attributes attr)
{
    if (_mock_libssh != nullptr)
    {
        _mock_libssh->sftp_attributes_free(file, line, func, attr);
    }
    else
    {
        delegate_real_sftp_attributes_free(file, line, func, attr);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > sftp_attributes_free %p", (void *)attr);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }
}
