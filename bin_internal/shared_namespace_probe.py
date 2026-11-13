#!/usr/bin/env python3
"""Compile and run a library namespace probe in both global load orders."""
import os
import subprocess
import tempfile
from pathlib import Path


# see: https://man7.org/linux/man-pages/man3/dlopen.3.html
# see: https://man7.org/linux/man-pages/man3/dladdr.3.html
def verify_namespace(app_dir, library, old_symbol, body):
    """Check renamed API calls against an unversioned original-name provider."""
    app_dir = Path(app_dir).resolve()
    library_path = app_dir / "prod" / "lib" / ("lib" + library + ".so")
    if not library_path.is_file():
        raise AssertionError(f"required built library is missing: {library_path}")
    with tempfile.TemporaryDirectory(prefix="namespace-coexistence-") as directory:
        work = Path(directory)
        (work / "stub.c").write_text(
            f"int {old_symbol}(void) {{ return 73; }}\n", encoding="utf-8")
        (work / "probe.c").write_text(body, encoding="utf-8")
        (work / "runner.c").write_text(r"""
#define _GNU_SOURCE
#include <dlfcn.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
static void *load(const char *path)
{
    void *handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    if (handle == NULL) { fprintf(stderr, "%s\n", dlerror()); exit(2); }
    return handle;
}
int main(int argc, char **argv)
{
    void *stub;
    void *probe;
    if (argc != 7) return 3;
    if (argv[1][0] == '0') { stub = load(argv[2]); probe = load(argv[3]); }
    else { probe = load(argv[3]); stub = load(argv[2]); }
    int (*old_function)(void) = (int (*)(void))dlsym(stub, argv[4]);
    int (*run)(void) = (int (*)(void))dlsym(probe, "namespace_probe");
    if (old_function == NULL || run == NULL || old_function() != 73) return 4;
    Dl_info info;
    char actual[PATH_MAX], expected[PATH_MAX];
    void *address = dlsym(probe, argv[5]);
    if (address == NULL || dladdr(address, &info) == 0 ||
        realpath(info.dli_fname, actual) == NULL || realpath(argv[6], expected) == NULL ||
        strcmp(actual, expected) != 0) return 5;
    return run();
}
""", encoding="utf-8")
        compiler = os.environ.get("CC", "cc")
        commands = [
            [compiler, "-shared", "-fPIC", str(work / "stub.c"), "-o", str(work / "stub.so")],
            [compiler, "-shared", "-fPIC", "-Werror", "-I" + str(app_dir / "prod" / "include"),
             str(work / "probe.c"), "-L" + str(library_path.parent), "-l" + library,
             "-Wl,-rpath," + str(library_path.parent), "-o", str(work / "probe.so")],
            [compiler, str(work / "runner.c"), "-ldl", "-o", str(work / "runner")],
        ]
        for command in commands:
            subprocess.run(command, check=True, capture_output=True, encoding="utf-8")
        for order in ("0", "1"):
            result = subprocess.run([str(work / "runner"), order, str(work / "stub.so"),
                                     str(work / "probe.so"), old_symbol, library + "_" + old_symbol,
                                     str(library_path)],
                                    capture_output=True, encoding="utf-8")
            if result.returncode:
                raise AssertionError(f"load order {order}: exit {result.returncode}: {result.stderr}")
