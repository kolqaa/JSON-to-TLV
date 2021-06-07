#include <stdio.h>
#include <stdlib.h>
#include "utils/uthash/includes/uthash.h"
#include "utils/tlv/includes/tlv.h"
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include "packer.h"
#include "packer_encode.h"
#include "packer_decode.h"

/* Encoding:
 * tlv_binary_data - this is the final file where json data will be encoded.
 * input_file_name - json data file.
 * dict_output     - this is final file where dictionary will be encoded.
 * out_file_name   - not used.
 *
 * Decoding:
 * tlv_binary_data - not used.
 * input_file_name - tlv encoded json data
 * dict_output     - tlv encoded dictionary data
 * out_file_name   - decoded result file.
 */
static struct files fopen_files(const char *tlv_binary_file, const char *input_file_name, const char *dict_output,
                         const char *out_file_name)
{
        struct files fptr;

        if (tlv_binary_file) {
                fptr.tlv = fopen(tlv_binary_file, "a+b");
                if (!fptr.tlv) {
                        fprintf(stderr, "Failed to open file: [%s]\n", tlv_binary_file);
                        exit(1);
                }
        }

        if (input_file_name) {
               fptr.in = fopen(input_file_name, "rb");
               if (!fptr.in) {
                       fprintf(stderr, "Failed to open file: [%s]\n", input_file_name);
                       exit(1);
               }
        }

        if (dict_output) {
               fptr.dict = fopen(dict_output, "a+b");
               if (!fptr.dict) {
                       fprintf(stderr, "Failed to open file: [%s]\n", dict_output);
                       exit(1);
               }
        }

        if (out_file_name) {
                fptr.out = fopen(out_file_name, "a+");
                if (!fptr.out) {
                        fprintf(stderr, "Failed to open file: [%s]\n", out_file_name);
                        exit(1);
                }
        }

        return fptr;
}

static inline void usage(void)
{
        printf("Usage: packer [options] \n"
               "Encoder mode:"
               "./packer --mode=\"encoder\""
               "--input=\"json_data_file.json\", --output=\"result.bin\", --dictionary=\"my_dictionary.dat\" \n"
               "Decoder mode:"
               "./packer --mode=\"decoder\""
               "--input=\"result.bin\", --dictionary=\"my_dictionary.dat\"\n");
}

static int json_encoder(const char *input_file_name, const char *tlv_binary_file, const char *dict_out_name)
{
        int                err;
        struct files       fptr;
        struct tlv        *tlv_data = NULL;
        struct dictionary *dictionary = NULL;

        fptr = fopen_files(tlv_binary_file, input_file_name, dict_out_name, NULL);

        err = encode_json_data(fptr, &dictionary, tlv_data);
        if (err) {
                fprintf(stderr, "Failed to encode json data\n");
                goto out;
        }

        err = json_encode_dictionary(dictionary, tlv_data, fptr);
        if (err) {
                fprintf(stderr, "Failed to encode dictionary data\n");
                goto out;
        }

        out:
        return err;
}

static void json_decoder(const char *tlv_binary_file, const char *dictionary, const char *output_file)
{
        struct files       fptr;
        int                counter = 0;
        int                total_decoded = 0;
        ssize_t            offset = 0;
        ssize_t            sz;
        int8_t            *src = NULL;
        struct dictionary *item, *tmp, *dict;
        struct tlv        dest;

        fptr = fopen_files(NULL, tlv_binary_file, dictionary, output_file);
        dict = decode_dictionary(dictionary, &dest);

        GET_SIZE_FROM_FILE(sz, fptr.in);

        src = malloc(sizeof(src) * sz);
        if (!src) {
                fprintf(stderr, "Failed to allocate memory during decode %d\n", __LINE__);
                exit(1);
        }

        char chunk[sz];

        fread(src, sz, 1, fptr.in);

        while (counter < sz) {
                int32_t x = 0;
                int8_t  boolean = 0;

                /* this var is used only for formatting purpose since we are writing to the file in the following way:
                 * { "key":"value", "key2":42, "key3":TRUE" }
                 * { "sdfsa":"value", "key5":32, "newkey":FALSE }
                 *  ....
                 *  ....
                 *  and since we are know that our file always have 3 keys in a line, we can write to the file
                 *  according to this info.
                 */
                if (total_decoded == 0) {
                        offset += snprintf(chunk + offset, sz, "%s", "{ ");
                }

                dest.type = src[counter];
                counter++;

                memcpy(&dest.size, &src[counter], 2);
                counter += 2;

                if (dest.size > 0) {
                        dest.data = malloc(dest.size);
                        memcpy(dest.data, &src[counter], dest.size);
                        counter += dest.size;

                        if ((dest.type & VALUE) && (dest.type & STRING)) {
                                offset += snprintf(chunk + offset, sz, "\"%s\", ", (char*)dest.data);
                        }
                        if ((dest.type & VALUE) && (dest.type & INTEGER)) {
                                memcpy(&x, dest.data, sizeof(int32_t));
                                offset += snprintf(chunk + offset, sz, "%d, ", x);
                        }

                        if ((dest.type & VALUE) && (dest.type & BOOLEAN)) {
                                memcpy(&boolean, dest.data, sizeof(int8_t));
                                offset += snprintf(chunk + offset, sz, "%s", (boolean ? "TRUE, " : "FALSE, "));
                        }

                        if ((dest.type & KEY) && (dest.type & INTEGER)) {
                                memcpy(&x, dest.data, sizeof(int32_t));

                                HASH_FIND_INT(dict, &x, item);
                                if (item) {
                                        offset += snprintf(chunk + offset, PATH_MAX, "\"%s\": ", item->key_name);
                                } else {
                                        fprintf(stderr, "Failed to decode file key");
                                        free(src);
                                        return ;
                                }

                        }
                }
                total_decoded++;
                if (total_decoded == MAX_KEY_VALUE_PAIR) {
                        /* all the way we are appending to the chunk buffer
                         * in the format "key: value, " in the last element
                         * we need to replace comma + space(", "). (offset - 2) - helps us
                         * to achieve it.
                         */
                        snprintf(chunk + (offset - 2), sz, "%s", " }");
                        offset = 0;
                        total_decoded = 0;

                        fprintf(fptr.out, "%s\n", chunk);
                        memset(chunk, 0, sizeof(chunk));

                }
        }

        HASH_ITER(hh, dict, item, tmp) {
                HASH_DEL(dict, item);
                free(item);
        }

        fclose(fptr.out);
        fclose(fptr.in);
}

int main(int argc, char **argv)
{
        int        c, opt_index;
        char       *mode = NULL;
        const char *in_file = NULL;
        const char *out_file = NULL;
        const char *dict_file = NULL;

        opt_index = 0;
        while ((c = getopt_long(argc, argv, "I:M:O", long_options, &opt_index)) != -1) {

                switch (c) {
                case 'I':
                        in_file = optarg;
                        if (strlen(in_file) == 0) {
                                fprintf(stderr, "option --input requires an argument\n");
                                exit(1);
                        }
                        break;
                case 'M':
                        mode = optarg;
                        if (strlen(mode) == 0) {
                                fprintf(stderr, "option --mode requires an argument\n");
                                exit(1);
                        }
                        break;
                case 'O':
                        out_file = optarg;
                        if (strlen(out_file) == 0) {
                                fprintf(stderr, "option --output requires an argument\n");
                                exit(1);
                        }
                        break;
                case 'D':
                        if (strlen(out_file) == 0) {
                                fprintf(stderr, "option --dictionary requires an argument\n");
                                exit(1);
                        }
                        dict_file = optarg;
                        break;
                case '?':
                        printf("USAGE:");
                        exit(0);
                default:
                        exit(1);
                }
        }

        if (!strcmp(mode, "encoder")) {
                json_encoder(in_file, out_file, dict_file);
        } else if (!strcmp(mode, "decoder")) {
                json_decoder(in_file, dict_file, out_file);
        } else {
                fprintf(stderr, "Unsupported mode: %s\n", mode);
                usage();
        }

        return 0;
}
