/* test-conf.c: Tests the configuration subsystem.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/nssd.h>
#include <nssd/conf.h>

#include <string.h>
#include <stdio.h>

#define TEST_PATH "test-conf.conf"

#define CHECK(errors, condition)                                        \
  do {                                                                  \
    printf("  CHECK(%s): ", #condition);                                \
    if(!(condition)) {                                                  \
      (errors)++;                                                       \
      printf("Assertion failed!\n");                                    \
    }                                                                   \
    else                                                                \
      printf("Passed.\n");                                              \
  } while(0)

#define TEST_HEADER(function)                   \
  printf("%s:\n", #function)

#define TEST_FOOTER(function)                   \
  printf("\n")

int test_conf(void) {
  int errors = 0;

  TEST_HEADER(test_conf);
  
  nssd_conf_file_t conf;
  nssd_conf_file_initialize(&conf, TEST_PATH, strlen(TEST_PATH) + 1);

  CHECK(errors, !nssd_conf_file_uptodate(&conf));
  CHECK(errors, nssd_conf_file_parse(&conf));
  CHECK(errors, nssd_conf_file_uptodate(&conf));

  char *adapter = NULL;
  size_t adapter_length = 0;
  CHECK(errors, nssd_conf_item_has(&conf.conf, "client:adapter"));
  CHECK(errors, nssd_conf_item_get(&conf.conf, "client:adapter", &adapter, &adapter_length));
  CHECK(errors, adapter_length == strlen("unsock") + 1 && strcmp(adapter, "unsock") == 0);
  nssd_conf_item_free(&conf.conf, &adapter);

  nssd_conf_file_finalize(&conf);

  TEST_FOOTER(test_conf);

  return errors;
}

int main(void) {
  int errors = 0;

  errors += test_conf();

  printf("%d error(s) occurred while testing.\n", errors);
  
  return errors ? 1 : 0;
}
