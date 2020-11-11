#include <memory>
using namespace std;

#include "CwApplication.h"

int main() {
    auto app = make_unique<CwApplication>();
    app->use("", bind(&CwApplication::demo, app.get(), placeholders::_1, placeholders::_2, placeholders::_3));
    app->start(8080);
}
