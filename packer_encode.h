#ifndef __PACKER_ENCODE_H__
#define __PACKER_ENCODE_H__

#define ENCODE_KEY               1
#define ENCODE_VALUE             2

/* means how much usually we have elements in the key:value chain
*   1    2
*   |    |
*  key:value
*/
#define DEFAULT_KEY_VALUE_SIZE 2

int json_encode_dictionary(struct dictionary *dictionary, struct tlv *tlv_data, struct files fptr);
int encode_json_data(struct files fptr, struct dictionary **dictionary, struct tlv *tlv_data);

#endif
