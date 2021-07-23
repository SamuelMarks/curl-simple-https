#ifndef CURL_SIMPLE_HTTPS
#define CURL_SIMPLE_HTTPS

#ifdef	__cplusplus
extern "C" {
#endif

#include <curl/curl.h>

#include "curl_simple_https_export.h"

struct ServerResponse {
    CURLcode code;
    /* What else would be good? - Headers? */
    long status_code;
    const char *body;
};

struct ServerResponse CURL_SIMPLE_HTTPS_EXPORT https_post(CURLU *);
struct ServerResponse CURL_SIMPLE_HTTPS_EXPORT https_put(CURLU *);
struct ServerResponse CURL_SIMPLE_HTTPS_EXPORT https_get(CURLU *);

#ifdef    __cplusplus
}
#endif

#endif /* CURL_SIMPLE_HTTPS */
