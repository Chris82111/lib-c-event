/*---------------------------------------------------------------------*
 *  private: include files                                             *
 *---------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <array>

#include "EventLf.h"

using namespace std;


/*---------------------------------------------------------------------*
 *  private: definitions                                               *
 *---------------------------------------------------------------------*/

#define UNUSED(x) (void)(x)


/*---------------------------------------------------------------------*
 *  private: macros like functions                                     *
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: typedefs                                                  *
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: variables                                                 *
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  public:  variables                                                 *
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: function prototypes                                       *
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: inline functions                                          *
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: functions                                                 *
 *---------------------------------------------------------------------*/

static void handler1(void * sender, void * e) { cout << "1" << endl; UNUSED(sender); UNUSED(e); }
static void handler2(void * sender, void * e) { cout << "2" << endl; UNUSED(sender); UNUSED(e); }
static void handler3(void * sender, void * e) { cout << "3" << endl; UNUSED(sender); UNUSED(e); }

void event_lf_example(void)
{
  event_lf_item_t data3[3];
  event_lf_item_t data7[7];

  // Direct initialization or initialization via a function can be used
  event_lf_memory_t memory = EVENT_LF_MEMORY_INIT();
  event_lf_memory.Init(&memory);

  // Adds the data to memory struct
  event_lf_memory.Add(&memory, data3, countof(data3));
  event_lf_memory.Add(&memory, data7, countof(data7));

  // Direct initialization or initialization via a function can be used
  event_lf_t e1 = EVENT_LF_INIT(&memory);
  event_lf.Init(&e1, &memory);

  event_lf_t e2 = EVENT_LF_INIT(&memory);
  event_lf.Init(&e2, &memory);

  // Optional adds a trailing item
  event_lf.Add(&e1, NULL);

  event_lf.Add(&e1, handler1);
  event_lf.Add(&e1, handler2);
  event_lf.Add(&e1, handler3);
  event_lf.Invoke(&e1, NULL, NULL); // 1, 2, 3

  event_lf.Add(&e2, handler2);
  event_lf.Add(&e2, handler1);
  event_lf.Invoke(&e2, NULL, NULL); // 2, 1

  event_lf.Sub(&e1, handler2);
  event_lf.Invoke(&e1, NULL, NULL); // 1, 3

  // Release all items, including trailing item
  // Prevent memory leaks when using global memory
  event_lf.Dispose(&e1);
  event_lf.Dispose(&e2);
}

void EventLf_example1(void)
{
  std::array<event_lf_item_t, 10> data10;

  auto memory = EventLfMemory(data10);
  auto e1 = EventLf(memory);

  e1.Add(handler1);
  e1.Add(handler2);
  e1.Add(handler3);
  e1.Invoke(NULL, NULL); // 1, 2, 3

  e1.Sub(handler2);
  e1.Invoke(NULL, NULL); // 1, 3
}

void EventLf_example2(void)
{
  std::array<event_lf_item_t, 3> data3;
  event_lf_item_t data7[7];

  auto memory = EventLfMemory(data3);
  memory.Add(data7, countof(data7));

  auto e1 = EventLf(memory);

  // Direct initialization or initialization via a function can be used
  event_lf_t e2 = EVENT_LF_INIT(memory.c_object);
  event_lf.Init(&e2, memory.c_object);

  e1.Add(handler1);
  e1.Add(handler2);
  e1.Add(handler3);
  e1.Invoke(NULL, NULL); // 1, 2, 3

  event_lf.Add(&e2, handler2);
  event_lf.Add(&e2, handler1);
  event_lf.Invoke(&e2, NULL, NULL); // 2, 1

  e1.Sub(handler2);
  e1.Invoke(NULL, NULL); // 1, 3

  // Release all items, including trailing item
  // Prevent memory leaks when using global memory
  event_lf.Dispose(&e2);
}

int event_lf_test(void)
{
  event_lf_example();
  EventLf_example1();
  EventLf_example2();

  return 0;
}


/*---------------------------------------------------------------------*
 *  eof                                                                *
 *---------------------------------------------------------------------*/
