#include <memory>
using namespace std;

#include "CwApplication.h"

int main() {
    auto p = make_unique<CwApplication>();
    p->start(8080);
}
