#include <stdio.h>

#include "event-test.h"

int main(void)
{
  int errors = event_lf_test();

  printf("Number of errors is %i\n", errors);
}

