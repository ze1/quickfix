/*
sha1.h - header of
 
 ============
 SHA-1 in C++
 ============
  
  100% Public Domain.
   
   Original C Code
   -- Steve Reid <steve@edmweb.com>
   Small changes to fit into bglibs
   -- Bruce Guenter <bruce@untroubled.org>
   Translation to simpler C++ Code
   -- Volker Grabsch <vog@notjusthosting.com>
   */

#ifndef SHA1_HPP
#define SHA1_HPP


#include <iostream>
#include <string>

class SHA1 {
public:
    SHA1();
    void update(const std::string &s);
    void update(std::istream &is);
    std::string final();
    static std::string from_file(const std::string &filename);

private:
    typedef unsigned long int uint32;   /* just needs to be at least 32bit */
    typedef unsigned long long uint64;  /* just needs to be at least 64bit */

    static const unsigned int DIGEST_INTS = 5;  /* number of 32bit integers per SHA1 digest */
    static const unsigned int BLOCK_INTS = 16;  /* number of 32bit integers per SHA1 block */
    static const unsigned int BLOCK_BYTES = BLOCK_INTS * 4;

    uint32 digest[DIGEST_INTS];
    std::string buffer;
    uint64 transforms;

    void reset();
    void transform(uint32 block[BLOCK_BYTES]);

    static void buffer_to_block(const std::string &buffer, uint32 block[BLOCK_BYTES]);
    static void read(std::istream &is, std::string &s, int max);
};

static std::string sha1(const std::string &string);

/* ---- Base64 Encoding/Decoding Table --- */
char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* encodeblock - encode 3 8-bit binary bytes as 4 '6-bit' characters */
static void encodeblock(unsigned char in[], char b64str[], int len) {
    unsigned char out[5];
    out[0] = b64[in[0] >> 2];
    out[1] = b64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
    out[2] = (unsigned char)(len > 1 ? b64[((in[1] & 0x0f) << 2) |
        ((in[2] & 0xc0) >> 6)] : '=');
    out[3] = (unsigned char)(len > 2 ? b64[in[2] & 0x3f] : '=');
    out[4] = '\0';
    const char* outc = reinterpret_cast<const char*>(&out[0]);
    char *str = &b64str[strnlen_s(b64str, 1000000000)];
    for (int i = 0; i < 5; ++i) *(str++) = out[i];
}

/* encode - base64 encode a stream, adding padding if needed */
static void b64_encode(char *clrstr, char *b64dst) {
    unsigned char in[3];
    int i, len = 0;
    int j = 0;

    b64dst[0] = '\0';
    while (clrstr[j]) {
        len = 0;
        for (i = 0; i<3; i++) {
            in[i] = (unsigned char)clrstr[j];
            if (clrstr[j]) {
                len++; j++;
            }
            else in[i] = 0;
        }
        if (len) {
            encodeblock(in, b64dst, len);
        }
    }
}

#endif /* SHA1_HPP */
