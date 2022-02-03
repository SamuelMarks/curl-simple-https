curl-simple-https
=================
[![CI for Linux, Windows, macOS](https://github.com/SamuelMarks/curl-simple-https/actions/workflows/linux-Windows-macOS.yml/badge.svg)](https://github.com/SamuelMarks/curl-simple-https/actions/workflows/linux-Windows-macOS.yml)
[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)

Very simple HTTPS interface built atop libcurl. It's based off a few [examples in libcurl's docs](https://curl.se/libcurl/c/example.html).

Highlights:

  - HTTPS 1.2 enabled
  - Always get a `struct` with response: https code; curl code; and body

## vcpkg

[`vcpkg`](https://vcpkg.io) is an open-source cross-platform library package management system from [Microsoft](https://microsoft.com); targeting macOS, Linux, and Windows.

It's very popular, and has strong CMake integration. Henceforth, it is chosen for this project as an example of third-party library integration.

[Install vcpkg](https://vcpkg.io/en/getting-started.html), configure it with your system. Then run:

    [root]/vcpkg/vcpkg install curl

Or do the same with your system package manager, `conan`, or whatever else you use.

## Build

    $ mkdir 'cmake-build-debug' && cd "$_"
    $ cmake -DCMAKE_TOOLCHAIN_FILE='[root]/vcpkg/scripts/buildsystems/vcpkg.cmake' \
            -DCMAKE_BUILD_TYPE='Debug' \
            ..
    $ cmake --build .

### CLI interface

If you want to change the CLI options, note that they are generated with `docopt`, use `python -m pip install docopt-c` then:
```bash
$ python -m docopt_c ".docopt" -o "libcurl_simple_https/curl_simple_https/cli"
```

## Usage
Use [libcurl](https://curl.se/libcurl/c)'s [URL API](https://everything.curl.dev/libcurl/url):

```c
#include <stdlib.h>
#include <stdio.h>

#include <curl/curl.h>
#include <curl_simple_https.h>

#define CLIENT_ID "foo"
#define CLIENT_SECRET "bar"

int main(void) {
    CURLU *urlp = curl_url();
    
    CURLUcode rc = curl_url_set(urlp, CURLUPART_SCHEME, "https", 0);
    rc = curl_url_set(urlp, CURLUPART_HOST, "oauth2.googleapis.com", 0);
    rc = curl_url_set(urlp, CURLUPART_PATH, "/token", 0);
    rc = curl_url_set(urlp, CURLUPART_QUERY, "grant_type=authorization_code", 0);
    rc = curl_url_set(urlp, CURLUPART_QUERY, "client_id=" CLIENT_ID, CURLU_APPENDQUERY);
    rc = curl_url_set(urlp, CURLUPART_QUERY, "client_secret=" CLIENT_SECRET, CURLU_APPENDQUERY);
    if(rc != CURLUE_OK) return EXIT_FAILURE;

    struct ServerResponse response = https_post(&urlp, /* extra headers */ NULL);
    curl_url_cleanup(urlp);
    if (response.code == CURLE_OK) {
        printf("status_code: %l\n# response\n%s", response.code, response.body);
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
        return EXIT_SUCCESS;
    }
}
```

## TODO

  - CTest
  - CPack
  - More docs
  - Headers support (input & output)

## See also

-  https://github.com/offscale/libacquire

---

## License

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or <https://www.apache.org/licenses/LICENSE-2.0>)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or <https://opensource.org/licenses/MIT>)

at your option.

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall be
dual licensed as above, without any additional terms or conditions.
