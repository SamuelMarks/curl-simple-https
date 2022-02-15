#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include <curl_simple_https.h>
#include <libcurl-simple-httpsConfig.h>

bool is_url(const char *maybe_url) {
  if (strlen(maybe_url) < 8)
    return false;
  else if (maybe_url[0] == 'h' && maybe_url[1] == 't' && maybe_url[2] == 't' &&
           maybe_url[3] == 'p')
    return (maybe_url[4] == ':' && maybe_url[5] == '/' &&
            maybe_url[6] == '/') ||
           (maybe_url[4] == 's' && maybe_url[5] == ':' && maybe_url[6] == '/' &&
            maybe_url[7] == '/');
  else if (maybe_url[0] == 'f' && maybe_url[1] == 't' && maybe_url[2] == 'p')
    return (maybe_url[3] == ':' && maybe_url[4] == '/' &&
            maybe_url[5] == '/') ||
           (maybe_url[3] == 's' && maybe_url[4] == ':' && maybe_url[5] == '/' &&
            maybe_url[6] == '/');
  return false /* strchr(maybe_url, '/') != NULL */;
}

struct ServerResponse (*get_simple_curl_function(
    const struct DocoptArgs *args))(CURLU *, const char *,
                                    struct curl_slist *) {
  if (args->POST)
    return args->json ? https_json_post : https_post;
  else if (args->PUT)
    return args->json ? https_json_put : https_put;
  /*else if (args.GET)*/
  return args->json ? https_json_get : https_get;
}

int main(int argc, char *argv[]) {
  struct DocoptArgs args = docopt(argc, argv, /* help */ 1,
                                  /* version */ LIBCURL_SIMPLE_HTTPS_VERSION);
  CURLUcode rc;
  CURLU *url;

  if (args.url == 0) {
    switch (argc) {
    case 2:
      if (is_url(argv[1])) {
        args.url = argv[1];
        break;
      }
    case 1:
      return EXIT_FAILURE;
    default:
      if (is_url(argv[1]))
        args.url = argv[1];
      else if (is_url(argv[argc - 1]))
        args.url = argv[argc - 1];
      else
        return EXIT_FAILURE;
    }
  }

  url = curl_url();
  rc = curl_url_set(url, CURLUPART_URL, args.url, 0);
  if (rc == CURLE_OK) {
    struct ServerResponse response =
        get_simple_curl_function(&args)(url, NULL, NULL);
    /* debug_response(&response); */
    puts(response.body);
    curl_url_cleanup(url);
    return EXIT_SUCCESS;
  } else {
    return CURLE_OUT_OF_MEMORY;
  }
}
