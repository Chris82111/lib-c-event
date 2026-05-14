#include <iostream>
#include <fstream>

#include "event-test.h"

using namespace std;

#include "event_lf.h"

int main()
{
  int errors = event_lf_test();

  cout << "The number of errors is " << errors << endl;
} 
