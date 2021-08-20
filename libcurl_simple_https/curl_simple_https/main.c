#include <stdio.h>
#include <stdlib.h>

#include <curl_simple_https.h>
#include <CurlSimpleHttpsConfig.h>
#include "cli.h"

bool is_url(const char *maybe_url) {
    if (strlen(maybe_url) < 8) return false;
    else if (maybe_url[0] == 'h' && maybe_url[1] == 't' && maybe_url[2] == 't' && maybe_url[3] == 'p')
        return (maybe_url[4] == ':' && maybe_url[5] == '/' && maybe_url[6] == '/')
               || (maybe_url[4] == 's' && maybe_url[5] == ':' && maybe_url[6] == '/' && maybe_url[7] == '/');
    else if (maybe_url[0] == 'f' && maybe_url[1] == 't' && maybe_url[2] == 'p')
        return (maybe_url[3] == ':' && maybe_url[4] == '/' && maybe_url[5] == '/')
               || (maybe_url[3] == 's' && maybe_url[4] == ':' && maybe_url[5] == '/' && maybe_url[6] == '/');
    return false /* strchr(maybe_url, '/') != NULL */;
}


int main(int argc, char *argv[]) {
    struct DocoptArgs args = docopt(argc, argv, /* help */ 1, /* version */ CurlSimpleHttps_VERSION);
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
        }
        printf("`args.url`:\t\"%s\"\n", args.url);
    }

    url = curl_url();
    rc = curl_url_set(url, CURLUPART_URL, args.url, 0);
    if (!rc) {
        struct ServerResponse response = https_get(url, NULL);
        puts(response.body);
        curl_url_cleanup(url);
        return EXIT_SUCCESS;
    } else {
        return E_OUTOFMEMORY;
    }
}
