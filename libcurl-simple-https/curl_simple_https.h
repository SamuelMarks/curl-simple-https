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

extern LIBCURL_SIMPLE_HTTPS_EXPORT void
debug_response(struct ServerResponse *response);

extern LIBCURL_SIMPLE_HTTPS_EXPORT struct ServerResponse
https_post(CURLU *, const char *, struct curl_slist *);
extern LIBCURL_SIMPLE_HTTPS_EXPORT struct ServerResponse
https_put(CURLU *, const char *, struct curl_slist *);
extern LIBCURL_SIMPLE_HTTPS_EXPORT struct ServerResponse
https_get(CURLU *, const char *, struct curl_slist *);
extern LIBCURL_SIMPLE_HTTPS_EXPORT struct ServerResponse
https_delete(CURLU *, const char *, struct curl_slist *);

extern LIBCURL_SIMPLE_HTTPS_EXPORT struct ServerResponse
https_json_post(CURLU *, const char *, struct curl_slist *);
extern LIBCURL_SIMPLE_HTTPS_EXPORT struct ServerResponse
https_json_put(CURLU *, const char *, struct curl_slist *);
extern LIBCURL_SIMPLE_HTTPS_EXPORT struct ServerResponse
https_json_get(CURLU *, const char *, struct curl_slist *);
extern LIBCURL_SIMPLE_HTTPS_EXPORT struct ServerResponse
https_json_delete(CURLU *, const char *, struct curl_slist *);

extern LIBCURL_SIMPLE_HTTPS_EXPORT CURL *make_request_post(CURL *);

extern LIBCURL_SIMPLE_HTTPS_EXPORT CURL *make_request_put(CURL *);

extern LIBCURL_SIMPLE_HTTPS_EXPORT CURL *make_request_delete(CURL *);

extern LIBCURL_SIMPLE_HTTPS_EXPORT struct curl_slist *
set_json_headers(struct curl_slist *);
extern LIBCURL_SIMPLE_HTTPS_EXPORT struct ServerResponse
https_wrapper(CURLU *, CURL *(*)(CURL *), const char *, struct curl_slist *);

#ifdef __cplusplus
}
#endif

#endif /* !CURL_SIMPLE_HTTPS */
