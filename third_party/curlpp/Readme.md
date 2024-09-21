[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) [![Travis CI](https://img.shields.io/travis/jpbarrette/curlpp.svg)](https://travis-ci.org/jpbarrette/curlpp) [![AppVeyor CI](https://img.shields.io/appveyor/ci/jpbarrette/curlpp.svg)](https://ci.appveyor.com/project/jpbarrette/curlpp)

# Description

[cURLpp](http://www.curlpp.org) is a C++ wrapper for libcURL. libcURL is described as:

<cite>
a free and easy-to-use client-side URL transfer library, supporting FTP, FTPS, HTTP, HTTPS, GOPHER, TELNET, DICT, FILE and LDAP. libcurl supports HTTPS certificates, HTTP POST, HTTP PUT, FTP uploading, kerberos, HTTP form based upload, proxies, cookies, user+password authentication, file transfer resume, http proxy tunneling and more!
</cite>

<cite>
libcurl is highly portable, it builds and works identically on numerous platforms, including Solaris, NetBSD, FreeBSD, OpenBSD, Darwin, HPUX, IRIX, AIX, Tru64, Linux, Windows, Amiga, OS/2, BeOs, Mac OS X, Ultrix, QNX, OpenVMS, RISC OS, Novell NetWare, DOS and more...
</cite>

<cite>
libcurl is free, thread-safe, IPv6 compatible, feature rich, well supported and fast.
</cite>

First, I need to quote Daniel Stenberg, the maintener of libcURL:
<cite>You can use libcURL instantly from within your C++ programs. You don't need cURLpp for that, cURLpp just adds an OO'ified layer that libcURL doesn't normally provide.</cite> It means that you need to be sure that you need, or want, cURLpp features. If not, I suggest to use directly the libcURL library. So, what are thoses features that cURLpp offers?

* You can query handles for option values (version 0.5.x and newer only).
* It use the C++ standard library structures instead of home made ones.
* It is exception safe.
* It is type safe.

# Download

Latest version is available on GitHub [here](https://github.com/jpbarrette/curlpp/releases/latest).

# Documentation

The programming guide is [here](https://github.com/jpbarrette/curlpp/tree/master/doc/guide.pdf). This guide is greatly inspired by the libcURL guide, that I strongly suggest to read. There's also some examples in the "[examples](http://github.com/jpbarrette/curlpp/tree/master/examples)/" source directory of cURLpp.

