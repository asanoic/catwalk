#ifndef LISTENER_H
#define LISTENER_H

#include <memory>
#include <string>

#include "boost-headers.h"

using namespace std;

class CwListener : public enable_shared_from_this<CwListener> {
    CwIoContext& ioc_;
    CwAcceptor acceptor_;
    shared_ptr<const string> doc_root_;

public:
    CwListener(CwIoContext& ioc, CwEndPoint endpoint,shared_ptr<const string> const& doc_root);
    void run();

private:
    void do_accept();
    void on_accept(CwErrorCode ec, CwSocket socket);
};
#endif // LISTENER_H
