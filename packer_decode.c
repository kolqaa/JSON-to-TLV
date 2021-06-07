#include <stdio.h>
#include <stdlib.h>
#include "utils/uthash/includes/uthash.h"
#include "utils/tlv/includes/tlv.h"
#include <string.h>
#include "packer.h"
#include "packer_decode.h"

struct dictionary *decode_dictionary(const char *dictionary_file, struct tlv *dest)
{
        FILE              *fp_d;
        int                counter = 0;
        int                tmp_val = 0;
        ssize_t            sz;
        int8_t             *src;
        struct dictionary *item, *dictionary = NULL;

        fp_d = fopen(dictionary_file, "rb");

        GET_SIZE_FROM_FILE(sz, fp_d);

        src = malloc(sizeof(src) * sz);
        fread(src, sz,1, fp_d);

        while (counter < sz) {
                dest->type = src[counter];
                counter++;

                memcpy(&dest->size, &src[counter], 2);
                counter += 2;

                /* decode data if it's not empty */
                if (dest->size > 0) {
                        dest->data = malloc(dest->size);
                        memcpy(dest->data, &src[counter], dest->size);
                        counter += dest->size;

                        if ((dest->type & VALUE) && (dest->type & INTEGER)) {
                                int32_t x = 0;
                                memcpy(&x, dest->data, sizeof(int32_t));

                                tmp_val = x;

                                item = malloc(sizeof(struct dictionary));
                                if (item == NULL)
                                        exit(-1);

                                item->key_name_int = x;
                                HASH_ADD_INT(dictionary, key_name_int, item);
                        }

                        if ((dest->type & KEY) && (dest->type & STRING)) {
                                HASH_FIND_INT(dictionary, &tmp_val, item);
                                if (item) {
                                        strcpy(item->key_name, (char*)dest->data);
                                } else {
                                        fprintf(stderr, "Error during dictionary decoding: key find failed \n");
                                        exit(1);
                                }
                                tmp_val = 0;
                        }
                        free((void*)dest->data);
                }
        }

        free(src);
        fclose(fp_d);
        return dictionary;
}