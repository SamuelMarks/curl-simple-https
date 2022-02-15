#ifndef CURL_SIMPLE_HTTPS
#define CURL_SIMPLE_HTTPS

#ifdef __cplusplus
extern "C" {
#endif

#include <curl/curl.h>

#include "libcurl-simple-https_export.h"

struct ServerResponse {
  CURLcode code;
  /* What else would be good? */
  struct curl_slist *headers;
  long status_code;
  const char *body;
};

void LIBCURL_SIMPLE_HTTPS_EXPORT
debug_response(struct ServerResponse *response);

struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT
https_post(CURLU *, const char *, struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT
https_put(CURLU *, const char *, struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT
https_get(CURLU *, const char *, struct curl_slist *);

struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT
https_json_post(CURLU *, const char *, struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT
https_json_put(CURLU *, const char *, struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT
https_json_get(CURLU *, const char *, struct curl_slist *);

LIBCURL_SIMPLE_HTTPS_EXPORT
CURL *make_request_post(CURL *);

LIBCURL_SIMPLE_HTTPS_EXPORT
CURL *make_request_put(CURL *);

LIBCURL_SIMPLE_HTTPS_EXPORT
struct curl_slist *set_json_headers(struct curl_slist *);
struct ServerResponse LIBCURL_SIMPLE_HTTPS_EXPORT
https_wrapper(CURLU *, CURL *(*)(CURL *), const char *, struct curl_slist *);

#ifdef __cplusplus
}
#endif

#endif /* CURL_SIMPLE_HTTPS */
