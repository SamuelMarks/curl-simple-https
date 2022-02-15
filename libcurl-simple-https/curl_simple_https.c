#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curl_simple_https.h"

/* Mostly https://curl.se/libcurl/c/postinmemory.html */

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp) {
  const size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if (!ptr) {
    /* out of memory! */
    fputs("not enough memory (realloc returned NULL)\n", stderr);
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

struct WriteThis {
  const char *readptr;
  size_t sizeleft;
};

static size_t read_callback(char *dest, size_t size, size_t nmemb,
                            void *userp) {
  struct WriteThis *wt = (struct WriteThis *)userp;
  size_t buffer_size = size * nmemb;

  if (wt->sizeleft) {
    /* copy as much as possible from the source to the destination */
    size_t copy_this_much = wt->sizeleft;
    if (copy_this_much > buffer_size)
      copy_this_much = buffer_size;
    memcpy(dest, wt->readptr, copy_this_much);

    wt->readptr += copy_this_much;
    wt->sizeleft -= copy_this_much;
    return copy_this_much; /* we copied this many bytes */
  }

  return 0; /* no more data left to deliver */
}

void debug_response(struct ServerResponse *response) {
  printf("body\n\n\"%s\"\n\n"
         "code\n\n%d\n\n"
         "status_code\n\n%ld\n\n",
         (*response).body, (*response).code, (*response).status_code);
}

void debug_request(CURLU *urlp, const char *body, struct curl_slist *headers);

struct ServerResponse https_wrapper(CURLU *urlp, CURL *(*curl_modifier)(CURL *),
                                    const char *body,
                                    struct curl_slist *headers) {
#define _res_error_handle                                                      \
  if (res != CURLE_OK) {                                                       \
    fprintf(stderr, "curl operation failed: %s\n", curl_easy_strerror(res));   \
    result.code = CURLE_FAILED_INIT;                                           \
    goto cleanup;                                                              \
  } else

  struct ServerResponse result = {CURLE_OK, NULL, 0L, NULL};
  struct MemoryStruct chunk;
  CURLcode res = CURLE_OK;
  CURL *curl;
  struct WriteThis wt;

  chunk.memory = malloc(1); /* will be grown as needed by realloc above */
  chunk.size = 0;           /* no data at this point */

  /* TODO: Stop calling this all the time */
  res = curl_global_init(CURL_GLOBAL_DEFAULT);
  _res_error_handle;

  curl = curl_easy_init();
  if (!curl) {
    fprintf(stderr, "curl_easy_init() failed: %s\n", curl_easy_strerror(res));
    result.code = CURLE_FAILED_INIT;
    goto cleanup;
  }

  curl_easy_setopt(curl, CURLOPT_CURLU, urlp);
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
  if (body != NULL) {
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    wt.readptr = body;
    wt.sizeleft = strlen(body);
    curl_easy_setopt(curl, CURLOPT_READDATA, &wt);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)wt.sizeleft);
  } else {
    wt.readptr = NULL;
    wt.sizeleft = 0;
    curl_easy_setopt(curl, CURLOPT_READDATA, &wt);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)wt.sizeleft);
  }
  if (headers != NULL)
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  if (curl_modifier != NULL)
    curl_modifier(curl);

  debug_request(urlp, body, headers);

  res = curl_easy_perform(curl);
  printf("curl_easy_perform res: %d\n", res);
  _res_error_handle;

  result.body = chunk.memory;
  {
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    result.status_code = response_code;
  }
  result.code = res;

cleanup:
  curl_easy_cleanup(curl);

  /* TODO: Stop calling this all the time */
  curl_global_cleanup();

  return result;
#undef _res_error_handle
}

void debug_request(CURLU *urlp, const char *body, struct curl_slist *headers) {
  char *url;
  assert(curl_url_get(urlp, CURLUPART_URL, &url, 0) == CURLE_OK);
  printf("URL:\n%s\n", url);
  puts("\nHEADERS:");
  if (headers == NULL)
    puts("(NULL)");
  else {
    struct curl_slist *item = headers;
    for (; item->next; item = item->next)
      puts(item->next->data);
  }
  printf("\nBODY:\n%s\n", body == NULL ? "(null)" : "");
}

CURL *make_request_post(CURL *curl) {
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  return curl;
}

CURL *make_request_put(CURL *curl) {
  curl_easy_setopt(curl, CURLOPT_PUT, 1L);
  return curl;
}

struct curl_slist *set_json_headers(struct curl_slist *headers) {
  headers = curl_slist_append(headers, "Expect:");
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");
  return headers;
}

struct ServerResponse https_post(CURLU *urlp, const char *body,
                                 struct curl_slist *headers) {
  return https_wrapper(urlp, make_request_post, body, headers);
}

struct ServerResponse https_put(CURLU *urlp, const char *body,
                                struct curl_slist *headers) {
  return https_wrapper(urlp, make_request_put, body, headers);
}

struct ServerResponse https_get(CURLU *urlp, const char *ignore,
                                struct curl_slist *headers) {
  return https_wrapper(urlp, NULL, NULL, headers);
}

struct ServerResponse https_json_post(CURLU *urlp, const char *body,
                                      struct curl_slist *headers) {
  return https_wrapper(urlp, make_request_post, body,
                       set_json_headers(headers));
}

struct ServerResponse https_json_put(CURLU *urlp, const char *body,
                                     struct curl_slist *headers) {
  return https_wrapper(urlp, make_request_put, body, set_json_headers(headers));
}

struct ServerResponse https_json_get(CURLU *urlp, const char *ignore,
                                     struct curl_slist *headers) {
  return https_wrapper(urlp, NULL, NULL, set_json_headers(headers));
}
