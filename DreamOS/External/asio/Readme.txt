http://think-async.com/

****

This is 1.11.0 development release which is compatible with TLS, in a non-boost configuration.

tls.hpp ln:235 ifdef is causing a crash on:
SSL_set_tlsext_host_name
although according to the code this should be supported by the OpenSSL version we are using.
As a workaround this is commented out.
