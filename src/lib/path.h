#ifndef CWPATH_H
#define CWPATH_H

#include <string>
#include <string_view>
#include <vector>

using namespace std;

class CwPath {
public:
    CwPath(string path, bool fullMatch = true);

private:
    void compile() noexcept;

private:
    string path;
    bool fullMatch;
    vector<string_view> tokens;
};

#endif // CWPATH_H
