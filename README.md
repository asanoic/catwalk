# Catwalk

Catwalk is a high-performance and easy-to-use C++ http web framework

Inspired by Sinatra and Express.js, simple web framework are now in C++ with high-performance.

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

boost 1.75 is required for new JSON support.

Catwalk can be built using GCC (Linux/Windows(MinGW-w64)), you might need change a little of other compilers


## Not a benchmark
My PC is pretty old, it is [Mac-mini late 2012 I5](https://support.apple.com/kb/sp659?locale=en_US), when I use Catwalk, the [Apache Bench](https://httpd.apache.org/docs/2.4/programs/ab.html)
```
ab -n 30000 -c 500 http://localhost:8080/
```
result is
```
This is ApacheBench, Version 2.3 <$Revision: 1843412 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 3000 requests
Completed 6000 requests
Completed 9000 requests
Completed 12000 requests
Completed 15000 requests
Completed 18000 requests
Completed 21000 requests
Completed 24000 requests
Completed 27000 requests
Completed 30000 requests
Finished 30000 requests


Server Software:        
Server Hostname:        localhost
Server Port:            8080

Document Path:          /
Document Length:        83 bytes

Concurrency Level:      500
Time taken for tests:   1.876 seconds
Complete requests:      30000
Failed requests:        0
Total transferred:      4980000 bytes
HTML transferred:       2490000 bytes
Requests per second:    15993.07 [#/sec] (mean)
Time per request:       31.264 [ms] (mean)
Time per request:       0.063 [ms] (mean, across all concurrent requests)
Transfer rate:          2592.63 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0   18  77.2     11    1026
Processing:     6   13   3.8     13      38
Waiting:        1   10   3.1      9      38
Total:         14   31  77.6     21    1045

Percentage of the requests served within a certain time (ms)
  50%     21
  66%     30
  75%     31
  80%     31
  90%     31
  95%     32
  98%     35
  99%     41
 100%   1045 (longest request)
```
for nodejs express.js hello world example
```
ab -n 30000 -c 500 http://localhost:3000/
```
result is
```
This is ApacheBench, Version 2.3 <$Revision: 1843412 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 3000 requests
Completed 6000 requests
Completed 9000 requests
Completed 12000 requests
Completed 15000 requests
Completed 18000 requests
Completed 21000 requests
Completed 24000 requests
Completed 27000 requests
Completed 30000 requests
Finished 30000 requests


Server Software:        
Server Hostname:        localhost
Server Port:            3000

Document Path:          /
Document Length:        12 bytes

Concurrency Level:      500
Time taken for tests:   9.844 seconds
Complete requests:      30000
Failed requests:        0
Total transferred:      6330000 bytes
HTML transferred:       360000 bytes
Requests per second:    3047.42 [#/sec] (mean)
Time per request:       164.073 [ms] (mean)
Time per request:       0.328 [ms] (mean, across all concurrent requests)
Transfer rate:          627.94 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0  104 304.4      2    1035
Processing:    32   54  35.8     48     487
Waiting:        8   32  33.8     26     461
Total:         34  157 322.2     50    1514

Percentage of the requests served within a certain time (ms)
  50%     50
  66%     57
  75%     61
  80%     63
  90%    314
  95%   1089
  98%   1107
  99%   1309
 100%   1514 (longest request)
```
### **Catwalk has 5x times RPS than Express.js**


### Have fun with super-duper high-performance C++ web framework: Catwalk
