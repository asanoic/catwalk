#include <memory>
#include <iostream>
using namespace std;

#include "CwApplication.h"

int main() {
    uint16_t port = 8080;
    auto app = make_unique<CwApplication>();
    app->use("", bind(&CwApplication::demo, app.get(), placeholders::_1, placeholders::_2, placeholders::_3));
    cout << "port " << port << ", and " << app->threads() << " thread" << (app->threads() > 1 ? "s" : "") << endl;
    app->start(port);
}
