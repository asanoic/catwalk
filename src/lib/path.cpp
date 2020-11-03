#include "path.h"

CwPath::CwPath(string path, bool fullMatch) :
    path(move(path)),
    fullMatch(fullMatch) {
    compile();
}

void CwPath::compile() noexcept {
}
