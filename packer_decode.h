#ifndef __PACKER_PACKER_DECODE_H__
#define __PACKER_PACKER_DECODE_H__

/* since our json file has always 3 key and 3 value */
#define MAX_KEY_VALUE_PAIR 6

#define GET_SIZE_FROM_FILE(sz, fptr) \
do {                                 \
      fseek(fptr, 0L, SEEK_END);     \
      (sz) = ftell(fptr);            \
      fseek(fptr, 0L, SEEK_SET);     \
} while(0)

struct dictionary *decode_dictionary(const char *dictionary_file, struct tlv *desti);

#endif
