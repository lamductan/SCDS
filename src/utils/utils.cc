#include <string.h>
#include <sys/stat.h>
#include <stdexcept>

#include "utils/utils.h"

std::string join_path(const std::vector<std::string> &tokens) {
    std::string path = "";
    for(int i = 0; i < tokens.size() - 1; ++i)
        path += tokens[i] + "/";
    path += tokens.back();
    return path;
}

void splitFileName(const char *pathname, std::string& dir, std::string& fnameonly) {
    if (!pathname || !*pathname) {
        dir = ".";
        fnameonly = "";
        return;
    }

    // find last "/" or "\"
    const char *s = pathname + strlen(pathname) - 1;
    s--;  // ignore potential trailing "/"
    while (s > pathname && *s != '\\' && *s != '/')
        s--;
    const char *sep = s <= pathname ? nullptr : s;

    // split along that
    if (!sep) {
        // no slash or colon
        if (strchr(pathname, ':') || strcmp(pathname, ".") == 0 || strcmp(pathname, "..") == 0) {
            fnameonly = "";
            dir = pathname;
        }
        else {
            fnameonly = pathname;
            dir = ".";
        }
    }
    else {
        fnameonly = s+1;
        dir = std::string(pathname, s-pathname+1);
    }
}

bool fileExists(const char *pathname) {
    // Note: stat("foo/") ==> error, even when "foo" exists and is a directory!
    struct stat statbuf;
    return stat(pathname, &statbuf) == 0;
}


void mkPath(const char *pathname)
{
    if (!fileExists(pathname)) {
        std::string pathprefix, dummy;
        splitFileName(pathname, pathprefix, dummy);
        mkPath(pathprefix.c_str());
        // note: anomaly with slash-terminated dirnames: stat("foo/") says
        // it does not exist, and mkdir("foo/") says cannot create (EEXIST):
        if (mkdir(pathname, 0755) != 0 && errno != EEXIST) {
            char error_msg[1024];
            snprintf(error_msg, 1024, "Cannot create directory '%s': %s", pathname, strerror(errno));
            throw std::runtime_error(error_msg);
        }
    }
}