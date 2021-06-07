#ifndef __PACKER_H__
#define __PACKER_H__

#include <stdio.h>
#include <stdlib.h>
#include "utils/jansson/includes/jansson.h"
#include "utils/uthash/includes/uthash.h"
#include "utils/tlv/includes/tlv.h"
#include <limits.h>
#include <string.h>
#include <getopt.h>

static struct option long_options[] = {
        { "input",      required_argument, 0, 'I' },
        { "mode",       required_argument, 0, 'M' },
        { "output",     required_argument, 0, 'O' },
        { "dictionary", required_argument, 0, 'D' },
        { NULL, 0, 0, 0 }
};

struct dictionary {
        char key_name[NAME_MAX];
        int  key_number;
        int  key_name_int;
        UT_hash_handle hh;
};

struct files {
       FILE *tlv;
       FILE *out;
       FILE *in;
       FILE *dict;
};

#endif
