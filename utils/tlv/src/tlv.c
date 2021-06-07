#include "tlv.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int32_t tlv_encode(FILE *fp, struct tlv *a, unsigned char *dest, int32_t total_encoded)
{
        int     i = 0;
        int     bytes_written = 0;
        int32_t counter = 0;

        while (i < total_encoded) {
                dest[counter] = a[i].type;
                counter++;

                memcpy(&dest[counter], &a[i].size, 2);
                counter += 2;

                memcpy(&dest[counter], a[i].data, a[i].size);
                counter += a[i].size;
                i++;
        }

        bytes_written = fwrite(dest, sizeof(unsigned char), counter, fp);

        return bytes_written;
}

int32_t __tlv_put(struct tlv *a, unsigned char type, int16_t size, const void *bytes)
{
        if(a == NULL || bytes == NULL)
                return -1;

        a->type = type;
        a->size = size;
        a->data = malloc(size);
        memcpy(a->data, bytes, size);
}

int32_t tlv_put_boolean(struct tlv *a, unsigned int tflags, int8_t value)
{
        return __tlv_put(a, tflags, sizeof(value), &value);
}

int32_t tlv_put_int32(struct tlv *a, unsigned int tflags, int32_t x)
{
        return __tlv_put(a, tflags, 4, &x);
}

int32_t tlv_put_str(struct tlv *a, unsigned int tflags, const char *str)
{
        return __tlv_put(a, tflags, strlen(str) + 1, str);
}

