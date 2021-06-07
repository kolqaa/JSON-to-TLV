#ifndef __TLV_H__
#define __TLV_H__

#include <stdint.h>
#include <stdio.h>

#define KEY     0x01
#define VALUE   0x02
#define STRING  0x04
#define BOOLEAN 0x08
#define INTEGER 0x20

struct tlv {
        int8_t  type;    // type
        void    *data; // pointer to data
        int16_t size;   // size of data
};

int32_t tlv_put_str(struct tlv *a, unsigned int tflags, const char *str);
int32_t tlv_put_int32(struct tlv *a, unsigned int tflags, int32_t x);
int32_t __tlv_put(struct tlv *a, unsigned char type, int16_t size, const void *bytes);
int32_t tlv_decode(FILE *fp, unsigned char *src, struct tlv *dest);
int32_t tlv_encode(FILE *fp, struct tlv *a, unsigned char *dest, int32_t total_encoded);
int32_t tlv_put_boolean(struct tlv *a, unsigned int tflags, int8_t value);

#endif
