#include <sstream>

#include "ssl_socket_impl.h"

namespace linear {

SSLSocketImpl::SSLSocketImpl(const std::string& host, int port,
                             const SSLContext& context,
                             const shared_ptr<EventLoopImpl>& loop,
                             const weak_ptr<HandlerDelegate>& delegate)
  : SocketImpl(host, port, loop, delegate, Socket::SSL),
    context_(context) {
}

SSLSocketImpl::SSLSocketImpl(tv_stream_t* stream,
                             const SSLContext& context,
                             const shared_ptr<EventLoopImpl>& loop,
                             const weak_ptr<HandlerDelegate>& delegate)
  : SocketImpl(stream, loop, delegate, Socket::SSL),
    context_(context) {
}

SSLSocketImpl::~SSLSocketImpl() {
}

Error SSLSocketImpl::Connect() {
  stream_ = static_cast<tv_stream_t*>(malloc(sizeof(tv_ssl_t)));
  if (stream_ == NULL) {
    return Error(LNR_ENOMEM);
  }
  int ret = tv_ssl_init(loop_->GetHandle(), reinterpret_cast<tv_ssl_t*>(stream_), context_.GetHandle());
  if (ret) {
    free(stream_);
    return Error(ret);
  }
  if (!bind_ifname_.empty()) {
    ret = tv_bindtodevice(stream_, bind_ifname_.c_str());
    if (ret != 0) {
      free(stream_);
      return Error(ret);
    }
  }
  stream_->data = ev_;
  std::ostringstream port_str;
  port_str << peer_.port;
  ret = tv_connect(stream_, peer_.addr.c_str(), port_str.str().c_str(), EventLoopImpl::OnConnect);
  if (ret) {
    assert(false); // never reach now
    free(stream_);
    return Error(ret);
  }
  return Error(LNR_OK);
}

Error SSLSocketImpl::GetVerifyResult() {
  lock_guard<mutex> state_lock(state_mutex_);
  if (state_ != Socket::CONNECTED && state_ != Socket::CONNECTING) {
    return Error(LNR_ENOTCONN);
  }
  int ret = tv_ssl_get_verify_result(reinterpret_cast<tv_ssl_t*>(stream_));
  if (ret) {
    return Error(ret, stream_->ssl_err);
  } else {
    return Error(LNR_OK);
  }
}

bool SSLSocketImpl::PresentPeerCertificate() {
  lock_guard<mutex> state_lock(state_mutex_);
  if (state_ != Socket::CONNECTED && state_ != Socket::CONNECTING) {
    return false;
  }
  X509* xcert = tv_ssl_get_peer_certificate(reinterpret_cast<tv_ssl_t*>(stream_));
  if (xcert == NULL) {
    return false;
  }
  X509_free(xcert);
  return true;
}

X509Certificate SSLSocketImpl::GetPeerCertificate() {
  lock_guard<mutex> state_lock(state_mutex_);
  if (state_ != Socket::CONNECTED && state_ != Socket::CONNECTING) {
    throw std::runtime_error("peer certificate does not exist");
  }
  // SSL_get_peer_certificate returns a new X509* object
  X509* xcert = tv_ssl_get_peer_certificate(reinterpret_cast<tv_ssl_t*>(stream_));
  if (xcert == NULL) {
    throw std::runtime_error("peer certificate does not exist");
  }
  X509Certificate cert(xcert);
  X509_free(xcert);
  return cert;
}

std::vector<X509Certificate> SSLSocketImpl::GetPeerCertificateChain() {
  lock_guard<mutex> state_lock(state_mutex_);
  if (state_ != Socket::CONNECTED && state_ != Socket::CONNECTING) {
    throw std::runtime_error("peer certificate does not exist");
  }
  // SSL_get_peer_cert_chain returns a X509* array and frees them when onclose
  STACK_OF(X509*) xcerts = tv_ssl_get_peer_certificate_chain(reinterpret_cast<tv_ssl_t*>(stream_));
  if (xcerts == NULL) {
    throw std::runtime_error("peer certificate does not exist");
  }
  // if we use sk_X509_pop, we can't ref them after
  std::vector<X509Certificate> certs;
  for (int i = 0; i < sk_X509_num(xcerts); i++) {
    certs.push_back(X509Certificate(sk_X509_value(xcerts, i)));
    // NOTICE: DONOT call X509_free( (X509*)sk_X509_value(xcerts, i) );
  }
  return certs;
}

}  // namespace linear
