#include <stdio.h>
#include <stdlib.h>

#include "curl_simple_https.h"

/* Mostly https://curl.se/libcurl/c/postinmemory.html */

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
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

struct ServerResponse
https_wrapper(CURLU *urlp, CURL *(*curl_modifier)(CURL*)) {
#define _res_error_handle if(res != CURLE_OK) {\
        fprintf(stderr, "curl operation failed: %s\n",\
                curl_easy_strerror(res));\
        result.code = CURLE_FAILED_INIT;\
        goto cleanup;\
    }

    struct ServerResponse result;
    struct MemoryStruct chunk;
    CURLcode res = CURLE_OK;
    chunk.memory = malloc(1);  /* will be grown as needed by realloc above */
    chunk.size = 0;    /* no data at this point */

    /* TODO: Stop calling this all the time */
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    _res_error_handle;

    CURL *curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "curl_easy_init() failed: %s\n",
                curl_easy_strerror(res));
        result.code = CURLE_FAILED_INIT;
        goto cleanup;
    }
    curl_easy_setopt(curl, CURLOPT_URL, urlp);
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    if (curl_modifier != NULL) curl_modifier(curl);

    res = curl_easy_perform(curl);
    _res_error_handle;

    result.body = chunk.memory;
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    result.status_code = response_code;
    result.code = res;

cleanup:
    curl_easy_cleanup(curl);
    curl_url_cleanup(urlp);

    /* TODO: Stop calling this all the time */
    curl_global_cleanup();

    return result;
#undef _res_error_handle
}

CURL *
make_request_post(CURL *curl) {
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    return curl;
}

CURL *
make_request_put(CURL *curl) {
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);
    return curl;
}

struct ServerResponse
https_post(CURLU *urlp) {
    return https_wrapper(urlp, make_request_post);
}

struct ServerResponse
https_put(CURLU *urlp) {
    return https_wrapper(urlp, make_request_put);
}

struct ServerResponse
https_get(CURLU *urlp) {
    return https_wrapper(urlp, NULL);
}
