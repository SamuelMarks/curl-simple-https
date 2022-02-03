FROM alpine

RUN apk add --no-cache gcc cmake make musl-dev linux-headers pkgconfig libcurl

COPY . /curl-simple-https

WORKDIR /curl-simple-https/build

RUN cmake -DCMAKE_BUILD_TYPE="Debug" .. && \
    cmake --build .
