#ifndef CURL_SIMPLE_HTTPS
#define CURL_SIMPLE_HTTPS

#ifdef	__cplusplus
extern "C" {
#endif

#include <curl/curl.h>

#include "libcurl_simple_https_export.h"

struct ServerResponse {
    CURLcode code;
    /* What else would be good? */
    struct curl_slist *headers;
    long status_code;
    const char *body;
};

void LIBCURL_SIMPLE_HTTPS_EXPORT debug_response(struct ServerResponse *response);

struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT https_post(CURLU *, struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT https_put(CURLU *, struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT https_get(CURLU *, struct curl_slist *);

struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT https_json_post(CURLU *, struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT https_json_put(CURLU *, struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT https_json_get(CURLU *, struct curl_slist *);

struct curl_slist * LIBCURL_SIMPLE_HTTPS_EXPORT set_json_headers(struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT https_wrapper(CURLU *, CURL *(*)(CURL*), struct curl_slist *);

#ifdef    __cplusplus
}
#endif

#endif /* CURL_SIMPLE_HTTPS */