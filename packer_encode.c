#include <stdio.h>
#include <stdlib.h>
#include "utils/jansson/includes/jansson.h"
#include "utils/uthash/includes/uthash.h"
#include "utils/tlv/includes/tlv.h"
#include <limits.h>
#include <string.h>
#include "packer.h"
#include "packer_encode.h"

static const char *help =
        "Supported file structure are:\n \
        \"key\" - strings only for keys\n \
        \"value\" - string, 42 - integer, true/TRUE/false/FALSE - boolean\n \
        { \"key1\":\"value\", \"key2\":42, \"key3\": TRUE }\n";

static void dictionary_encode_to_tlv(struct tlv *tlv, const char *value, int32_t key, int TYPE_TO_ENCODE)
{
        if (TYPE_TO_ENCODE == ENCODE_KEY)
                tlv_put_str(tlv, KEY | STRING, value);
        else if (TYPE_TO_ENCODE == ENCODE_VALUE)
                tlv_put_int32(tlv, VALUE | INTEGER, key);
}

static void json_encode_to_tlv(struct tlv *tlv, int32_t key, json_t *value, int TYPE_TO_ENCODE)
{
        if (TYPE_TO_ENCODE == ENCODE_KEY) {
                tlv_put_int32(tlv, KEY | INTEGER, key);
                return ;
        }

        switch (json_typeof(value)) {
        case JSON_STRING:
                tlv_put_str(tlv, VALUE | STRING, json_string_value(value));
                break;
        case JSON_INTEGER:
                tlv_put_int32(tlv, VALUE | INTEGER, (int32_t)json_integer_value(value));
                break;
        case JSON_FALSE:
                tlv_put_boolean(tlv, VALUE | BOOLEAN, 0);
                break;
        case JSON_TRUE:
                tlv_put_boolean(tlv, VALUE | BOOLEAN, 1);
                break;
        default:
                 printf("Unknown value\n");
        }
}

int json_encode_dictionary(struct dictionary *dictionary, struct tlv *tlv_data, struct files fptr)
{
        int                obj_size = HASH_COUNT(dictionary);
        int                total_encoded = 0;
        unsigned char     *encoded_buffer;
        struct dictionary *tmp, *item;

        tlv_data = malloc(sizeof(*tlv_data) * obj_size * DEFAULT_KEY_VALUE_SIZE);
        if (!tlv_data) {
                fprintf(stderr, "Failed to allocate memory during dictionary encoding. %d\n", __LINE__);
                return 1;
        }

        encoded_buffer = malloc(sizeof(*tlv_data) * obj_size * DEFAULT_KEY_VALUE_SIZE);
        if (!encoded_buffer) {
                fprintf(stderr, "Failed to allocate memory during dictionary encoding. %d\n", __LINE__);
        }

        /* This is the end of encoding process, we can free dictionary and encode it during clearing process */
        HASH_ITER(hh, dictionary, item, tmp) {
                dictionary_encode_to_tlv(&tlv_data[total_encoded++], NULL, item->key_number, ENCODE_VALUE);
                dictionary_encode_to_tlv(&tlv_data[total_encoded++], item->key_name, 0, ENCODE_KEY);

                HASH_DEL(dictionary, item);
                free(item);
        }

        tlv_encode(fptr.dict, tlv_data, encoded_buffer, total_encoded);
        fclose(fptr.dict);

        free(tlv_data);
        free(encoded_buffer);

        return 0;
}

int encode_json_data(struct files fptr, struct dictionary **dictionary, struct tlv *tlv_data)
{
        int          i = 1;
        int          rc = 0;
        char         line[PATH_MAX] = {0};
        struct       dictionary *item;
        json_t       *object = NULL;
        json_error_t err;
        unsigned char *encode_buffer;

        while(fgets(line, sizeof(line), fptr.in) != NULL) {
                int         obj_size;
                int         total_encoded = 0;
                json_t     *value = NULL;
                const char *key = NULL;

                /* Remove newline from buffer */
                line[strcspn(line, "\n")] = '\0';
                object = json_loads(line, 0, &err);

                if (!object) {
                        fprintf(stderr, "Failed to read file: not supported json format: %s.\n%s\n", err.text, help);
                        return (rc = 1), rc;
                }

                obj_size = json_object_size(object);

                tlv_data = malloc((obj_size * DEFAULT_KEY_VALUE_SIZE) * sizeof(struct tlv));
                if (!tlv_data) {
                        fprintf(stderr, "Failed to allocate memory during json_encode\n");
                        return (rc = 1), rc;
                }

                encode_buffer = malloc(sizeof(struct tlv) * (obj_size * DEFAULT_KEY_VALUE_SIZE));
                if (!encode_buffer) {
                        fprintf(stderr, "Failed to allocate memory during json_encode for buffer\n");
                        return (rc = 1), rc;
                }

                /* Since we are iterating over json keys, we can build dictionary and encode json data in place */
                json_object_foreach(object, key, value) {

                        HASH_FIND_STR(*dictionary, key, item);
                        if (item) {

                                /* if we found key which already exists in dictionary we can skip it
                                 * because we already map key to integer. Only encoding part is required.
                                 */
                                json_encode_to_tlv(&tlv_data[total_encoded++], item->key_number, NULL, ENCODE_KEY);
                                json_encode_to_tlv(&tlv_data[total_encoded++], 0, value, ENCODE_VALUE);

                                continue;

                        } else {

                                /* This key is not yet in dictionary let's add it and map key string to integer,
                                 * final result will be key:integer structure such notation will help during
                                 * decoding because json key strings are replaced with number:
                                 *
                                 * Dictionary:
                                 * "key1":1, "key2":2, "key3":3"
                                 *
                                 * JSON record will be encoded in this format:
                                 * 1:"value", 2:42, 3:TRUE
                                 */

                                item = malloc(sizeof(struct dictionary));
                                if (item == NULL) {
                                        fprintf(stderr, "Failed to allocate memory during json data encode\n");
                                        return (rc = 1), rc;
                                }

                                strcpy(item->key_name, key);
                                item->key_number = i;

                                HASH_ADD_STR(*dictionary, key_name, item);
                                json_encode_to_tlv(&tlv_data[total_encoded++], i++, NULL, ENCODE_KEY);
                                json_encode_to_tlv(&tlv_data[total_encoded++], 0, value, ENCODE_VALUE);
                        }
                }

                /* Build tlv file with encoded data */
                tlv_encode(fptr.tlv, tlv_data, encode_buffer, total_encoded);
                free(tlv_data);
                tlv_data = NULL;

                free(encode_buffer);
                memset(line, 0, sizeof(line));
        }

        fclose(fptr.tlv);

        return rc;
}
