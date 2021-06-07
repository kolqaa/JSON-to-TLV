# JSON-to-TLV
Basic JSON to TLV encoder/decoder

# Requirements

```
cmake >= 3.1

```

# How to use
```
$ make
$ cd build

To encode JSON to TLV binary
$ ./packer --mode=encoder --input=json_test_file --output=result.bin --dictionary=dictionary.bin

To decode TLV binary:
$ ./packer --mode=decoder --input=result.bin --output=result_json --dictionary=dictionary.bin

```
# Notes
Currently only next format of JSON file is supported:
1. only three keys in a line
``` 
{ “key1”:“value”, “key2”:42, “key3”:TRUE }
{“sadsf”:”dsewtew”, “dsre”:3221, “sdfds”:”dsfewew”}
```
2. values can be { INTEGER, STRING, BOOLEAN }
3. No nested JSON structures

Not compatible with ASN.1 schema neither with EMV. Using a simple "handmade" T-L-V encoding algorithm for the learning purpose.
Tested on: debian 7.1.0, ubuntu 16.04 LTS
