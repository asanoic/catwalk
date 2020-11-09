#include <memory>
using namespace std;

#include "CwApplication.h"

int main() {
    make_unique<CwApplication>()->start(8080);
}
