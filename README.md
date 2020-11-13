# catwalk

catwalk is a high-performance and easy-to-use C++ http web framework

Inspired by Sinatra and Express.js, now simple web framework are now in C++ with high-performance.

the simple hello world server likes

```C++
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
```
and when you access `http://localhost:8080/`
hello world page will return and console output is
```
port 8080, and 8 threads 
route 1 before
route 2 before
response here
route 2 after
route 1 after
```

## how to build

Catwalk need boost and C++17, build following classic cmake project
```
md b
cmake . -B b -DCMAKE_BUILD_TYPE=Release
cmake --build b -j8
```
you might need change a little bit for you local boost install folder.

### Have func with super-duper high-performance C++ web server:
### Catwalk
