#include <cstring>

#include "linear/addrinfo.h"

namespace linear {

Addrinfo::Addrinfo() : addr("undefined"), port(-1), proto(UNKNOWN) {
}

Addrinfo::Addrinfo(const std::string& a, int p) : addr("undefined"), port(-1), proto(UNKNOWN) {
  struct addrinfo hints;
  struct addrinfo* res = NULL;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  int r = getaddrinfo(a.c_str(), NULL, &hints, &res);
  if (r == 0) {
    addr = a;
    port = p;
    proto = IPv4;
    freeaddrinfo(res);
    return;
  }
  hints.ai_family = AF_INET6;
  r = getaddrinfo(a.c_str(), NULL, &hints, &res);
  if (r == 0) {
    addr = a;
    port = p;
    proto = IPv6;
    freeaddrinfo(res);
  }
  return;
}

// ref: http://stackoverflow.com/questions/35551879/cast-from-sockaddr-to-sockaddr-in-increases-required-alignment
Addrinfo::Addrinfo(const struct sockaddr* sa) : addr("undefined"), port(-1), proto(UNKNOWN) {
  switch (sa->sa_family) {
  case AF_INET:
    {
      char host[NI_MAXHOST];
      socklen_t slen = sizeof(struct sockaddr_in);
      if (getnameinfo(sa, slen, host, sizeof(host), NULL, 0, NI_NUMERICHOST) == 0) {
        addr = std::string(host);
      } else {
        break;
      }
      const struct sockaddr_in* src = reinterpret_cast<const struct sockaddr_in*>((const void*)sa);
      port = ntohs(src->sin_port);
      proto = IPv4;
      break;
    }
  case AF_INET6:
    {
      char host[NI_MAXHOST];
      socklen_t slen = sizeof(struct sockaddr_in6);
      if (getnameinfo(sa, slen, host, sizeof(host), NULL, 0, NI_NUMERICHOST) == 0) {
        addr = std::string(host);
      } else {
        break;
      }
      const struct sockaddr_in6* src = reinterpret_cast<const struct sockaddr_in6*>((const void*)sa);
      port = ntohs(src->sin6_port);
      proto = IPv6;
      break;
    }
  default:
    return;
  }
}

Addrinfo::~Addrinfo() {
}

}  // namespace linear
