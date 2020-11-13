#include <iostream>
#include <memory>
using namespace std;

#include "CwApplication.h"

int main() {
    uint16_t port = 8080;
    auto app = make_unique<CwApplication>();
    app
        ->use([](CwRequest* req, CwResponse* res, CwNextFunc next) {
            cout << "route 1 before" << endl;
            next();
            cout << "route 1 after" << endl;
        })
        ->use([](CwRequest* req, CwResponse* res, CwNextFunc next) {
            cout << "route 2 before" << endl;
            next();
            cout << "route 2 after" << endl;
        })
        ->set(CwHttpVerb::get, "/", [](CwRequest* req, CwResponse* res) {
            static string html = "<html><head><title>Catwalk</title></head><body><h1>Hello, world!</h1></body></html>";
            res
                ->setStatus(200)
                ->setHeader("Content-Type", "text/html")
                ->setBody(CwBody(html.begin(), html.end()))
                ->send();
            cout << "response here" << endl;
        });
    cout << "port " << port << ", and " << app->threads() << " thread" << (app->threads() > 1 ? "s" : "") << endl;
    app->start(port);
}
