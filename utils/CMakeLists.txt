target_sources(packer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/dump.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/error.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/hashtable.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/hashtable_seed.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/load.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/memory.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/pack_unpack.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/strbuffer.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/strconv.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/utf.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/value.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/version.c
                              ${CMAKE_CURRENT_SOURCE_DIR}/tlv/src/tlv.c)
