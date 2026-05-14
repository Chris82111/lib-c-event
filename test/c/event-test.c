/*---------------------------------------------------------------------*
 *  private: include files                                             *
 *---------------------------------------------------------------------*/

#include "event-test.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "event_lf.h"


/*---------------------------------------------------------------------*
 *  private: definitions                                               *
 *---------------------------------------------------------------------*/

#define UNUSED(x) (void)(x)


#ifndef UNUSED_ATTR
  #if defined(__GNUC__) || defined(__clang__)
    #define UNUSED_ATTR __attribute__((unused))
  #else
    #define UNUSED_ATTR
  #endif
#endif

/*---------------------------------------------------------------------*
 *  private: macros like functions                                     *
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: typedefs                                                  *
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: variables                                                 *
 *---------------------------------------------------------------------*/

char out[128+1];
uint8_t out_index = 0;


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

#define REGIN_EVENT_LF_FUNCTIONS
#ifdef REGIN_EVENT_LF_FUNCTIONS

uint16_t event_lf_memory_init_test(void)
{
  uint16_t errors = 0;

  uint8_t data[sizeof(event_lf_memory_t)];
  memset(data, 0xff, sizeof(event_lf_memory_t));

  event_lf_memory_t * p = (event_lf_memory_t *)data;

  event_lf_memory_init(NULL);

  event_lf_memory_init(p);

  if (NULL != p->next) { errors++; }
  if (0 != p->used) { errors++; }
  if (0 != p->capacity) { errors++; }



  event_lf_memory_t o = { 0 };
  memset(&o, 0xff, sizeof(event_lf_memory_t));

  // compound literal (C99 and later)
  o = (event_lf_memory_t)EVENT_LF_MEMORY_INIT();

  if (NULL != o.next) { errors++; }
  if (0 != o.used) { errors++; }
  if (0 != o.capacity) { errors++; }



  return errors;
}

uint16_t event_lf_memory_add_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data4[4];
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();

  if (0 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }

  event_lf_memory_add(NULL, data4, countof(data4));

  event_lf_memory_add(&m, data4, countof(data4));

  if (4 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }

  event_lf_item_t * d;
  event_lf_item_t * i;
  
  d = data4 + 0;
  i = m.next;
  if (d != i) { errors++; }

  d = data4 + 1;
  i = m.next->next;
  if (d != i) { errors++; }

  d = data4 + 2;
  i = m.next->next->next;
  if (d != i) { errors++; }

  d = data4 + 3;
  i = m.next->next->next->next;
  if (d != i) { errors++; }

  d = NULL;
  i = m.next->next->next->next->next;
  if (d != i) { errors++; }


  
  event_lf_item_t data2[2];
  event_lf_memory_add(&m, data2, countof(data2));

  if (6 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }

  d = data2 + 0;
  i = m.next;
  if (d != i) { errors++; }

  d = data2 + 1;
  i = m.next->next;
  if (d != i) { errors++; }

  d = data4 + 0;
  i = m.next->next->next;
  if (d != i) { errors++; }

  d = data4 + 1;
  i = m.next->next->next->next;
  if (d != i) { errors++; }

  d = data4 + 2;
  i = m.next->next->next->next->next;
  if (d != i) { errors++; }

  d = data4 + 3;
  i = m.next->next->next->next->next->next;
  if (d != i) { errors++; }

  d = NULL;
  i = m.next->next->next->next->next->next->next;
  if (d != i) { errors++; }



  event_lf_item_t data1[1];
  event_lf_memory_add(&m, data1, countof(data1));

  if (7 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }

  d = data1 + 0;
  i = m.next;
  if (d != i) { errors++; }

  d = data2 + 0;
  i = m.next->next;
  if (d != i) { errors++; }



  event_lf_item_t data0[0];
  event_lf_memory_add(&m, data0, countof(data0));
  
  if (7 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }

  d = data1 + 0;
  i = m.next;
  if (d != i) { errors++; }

  d = data2 + 0;
  i = m.next->next;
  if (d != i) { errors++; }


  
  event_lf_memory_add(&m, data1, 0);
  
  if (7 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }

  d = data1 + 0;
  i = m.next;
  if (d != i) { errors++; }

  d = data2 + 0;
  i = m.next->next;
  if (d != i) { errors++; }


  
  event_lf_memory_add(&m, NULL, 1);
  
  if (7 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }

  d = data1 + 0;
  i = m.next;
  if (d != i) { errors++; }

  d = data2 + 0;
  i = m.next->next;
  if (d != i) { errors++; }



  return errors;
}

uint16_t event_lf_memory_allocate_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data4[4];
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();

  event_lf_memory_add(&m, data4, countof(data4));

  if (4 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }

  event_lf_item_t * n = event_lf_memory_allocate(NULL);
  if (NULL != n) { errors++; }

  event_lf_item_t * d1;
  event_lf_item_t * d2;
  event_lf_item_t * d3;
  event_lf_item_t * d4;
  event_lf_item_t * d5;
  event_lf_item_t * d6;
  event_lf_item_t * d7;

  d1 = event_lf_memory_allocate(&m);
  if (d1 != data4 + 0) { errors++; }
  if (4 != m.capacity) { errors++; }
  if (1 != m.used) { errors++; }

  d2 = event_lf_memory_allocate(&m);
  if (d2 != data4 + 1) { errors++; }
  if (4 != m.capacity) { errors++; }
  if (2 != m.used) { errors++; }

  d3 = event_lf_memory_allocate(&m);
  if (d3 != data4 + 2) { errors++; }
  if (4 != m.capacity) { errors++; }
  if (3 != m.used) { errors++; }

  d4 = event_lf_memory_allocate(&m);
  if (d4 != data4 + 3) { errors++; }
  if (4 != m.capacity) { errors++; }
  if (4 != m.used) { errors++; }

  d5 = event_lf_memory_allocate(&m);
  if (NULL != d5) { errors++; }
  if (4 != m.capacity) { errors++; }
  if (4 != m.used) { errors++; }
  
  event_lf_item_t data2[2];
  event_lf_memory_add(&m, data2, countof(data2));

  d5 = event_lf_memory_allocate(&m);
  if (d5 != data2 + 0) { errors++; }
  if (6 != m.capacity) { errors++; }
  if (5 != m.used) { errors++; }

  d6 = event_lf_memory_allocate(&m);
  if (d6 != data2 + 1) { errors++; }
  if (6 != m.capacity) { errors++; }
  if (6 != m.used) { errors++; }

  d7 = event_lf_memory_allocate(&m);
  if (NULL != d7) { errors++; }
  if (6 != m.capacity) { errors++; }
  if (6 != m.used) { errors++; }


  return errors;
}

uint16_t event_lf_memory_free_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data4[4];
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();

  event_lf_memory_add(&m, data4, countof(data4));
  
  event_lf_item_t * d1 = NULL;
  event_lf_item_t * d2 = NULL;
  event_lf_item_t * d3 = NULL;
  event_lf_item_t * d4 = NULL;
  event_lf_item_t * d5 = NULL;
  event_lf_item_t * d6 = NULL;
  event_lf_item_t * d7 = NULL;
  event_lf_item_t * i;


  event_lf_memory_free(NULL, d1);
  event_lf_memory_free(&m, NULL);

  if (4 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }

  d1 = event_lf_memory_allocate(&m);
  d2 = event_lf_memory_allocate(&m);
  d3 = event_lf_memory_allocate(&m);
  d4 = event_lf_memory_allocate(&m);
  d5 = event_lf_memory_allocate(&m);

  i = m.next;
  if (NULL != i) { errors++; }

  event_lf_memory_free(&m, d4);
  if (4 != m.capacity) { errors++; }
  if (3 != m.used) { errors++; }
  
  i = m.next;
  if (d4 != i) { errors++; }

  i = m.next->next;
  if (NULL != i) { errors++; }

  event_lf_memory_free(&m, d3);
  if (4 != m.capacity) { errors++; }
  if (2 != m.used) { errors++; }
  
  i = m.next;
  if (d3 != i) { errors++; }

  i = m.next->next;
  if (d4 != i) { errors++; }

  i = m.next->next->next;
  if (NULL != i) { errors++; }

  event_lf_memory_free(&m, d2);
  if (4 != m.capacity) { errors++; }
  if (1 != m.used) { errors++; }
  
  i = m.next;
  if (d2 != i) { errors++; }

  i = m.next->next;
  if (d3 != i) { errors++; }

  i = m.next->next->next;
  if (d4 != i) { errors++; }

  i = m.next->next->next->next;
  if (NULL != i) { errors++; }

  event_lf_memory_free(&m, d1);
  if (4 != m.capacity) { errors++; }
  if (0 != m.used) { errors++; }
  
  i = m.next;
  if (d1 != i) { errors++; }

  i = m.next->next;
  if (d2 != i) { errors++; }

  i = m.next->next->next;
  if (d3 != i) { errors++; }

  i = m.next->next->next->next;
  if (d4 != i) { errors++; }

  i = m.next->next->next->next->next;
  if (NULL != i) { errors++; }

  // prevent unused warning
  UNUSED(d5);
  UNUSED(d6);
  UNUSED(d7);

  return errors;
}

uint16_t event_lf_init_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data4[4];
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();
  event_lf_memory_add(&m, data4, countof(data4));


  uint8_t data[sizeof(event_lf_t)];
  memset(data, 0xff, sizeof(event_lf_t));

  event_lf_t * p = (event_lf_t *)data;

  event_lf_init(NULL, &m);
  event_lf_init(p, NULL);

  event_lf_init(p, &m);

  if (NULL != atomic_load(&p->list)) { errors++; }
  if (0 != atomic_load(&p->threads)) { errors++; }
  if (0 != atomic_load(&p->remove_count)) { errors++; }
  if (NULL != p->remove) { errors++; }
  if (&m != p->memory_object) { errors++; }
  if ((event_lf_item_t *(*)(void *))event_lf_memory_allocate != p->allocate) { errors++; }
  if ((void(*)(void *, event_lf_item_t *))event_lf_memory_free != p->free) { errors++; }



  event_lf_t o = { 0 };
  memset(&o, 0xff, sizeof(event_lf_t));

  // compound literal (C99 and later)
  o = (event_lf_t)EVENT_LF_INIT(&m);

  if (NULL != atomic_load(&o.list)) { errors++; }
  if (0 != atomic_load(&o.threads)) { errors++; }
  if (0 != atomic_load(&o.remove_count)) { errors++; }
  if (NULL != o.remove) { errors++; }
  if (&m != o.memory_object) { errors++; }
  if ((event_lf_item_t *(*)(void *))event_lf_memory_allocate != o.allocate) { errors++; }
  if ((void(*)(void *, event_lf_item_t *))event_lf_memory_free != o.free) { errors++; }


  return 0;
}

static event_lf_item_t * event_lf_init_custom_memory_allocate_test(event_lf_memory_t * object)
{

  UNUSED(object);
  return NULL;
}

static void event_lf_init_custom_memory_free_test(event_lf_memory_t * object, event_lf_item_t * item)
{
  UNUSED(object);
  UNUSED(item);
  ;
}

uint16_t event_lf_init_custom_memory_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data4[4];
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();
  event_lf_memory_add(&m, data4, countof(data4));


  uint8_t data[sizeof(event_lf_t)];
  memset(data, 0xff, sizeof(event_lf_t));

  event_lf_t * p = (event_lf_t *)data;

  event_lf_init_custom_memory(NULL, &m, (event_lf_item_t *(*)(void *))event_lf_init_custom_memory_allocate_test, (void(*)(void *, event_lf_item_t *))event_lf_init_custom_memory_free_test);
  event_lf_init_custom_memory(p, NULL, (event_lf_item_t *(*)(void *))event_lf_init_custom_memory_allocate_test, (void(*)(void *, event_lf_item_t *))event_lf_init_custom_memory_free_test);
  event_lf_init_custom_memory(p, &m, NULL, (void(*)(void *, event_lf_item_t *))event_lf_init_custom_memory_free_test);
  event_lf_init_custom_memory(p, &m, (event_lf_item_t *(*)(void *))event_lf_init_custom_memory_allocate_test, NULL);

  event_lf_init_custom_memory(p, &m, (event_lf_item_t *(*)(void *))event_lf_init_custom_memory_allocate_test, (void(*)(void *, event_lf_item_t *))event_lf_init_custom_memory_free_test);

  if (NULL != atomic_load(&p->list)) { errors++; }
  if (0 != atomic_load(&p->threads)) { errors++; }
  if (0 != atomic_load(&p->remove_count)) { errors++; }
  if (NULL != p->remove) { errors++; }
  if (&m != p->memory_object) { errors++; }
  if ((event_lf_item_t *(*)(void *))event_lf_init_custom_memory_allocate_test != p->allocate) { errors++; }
  if ((void(*)(void *, event_lf_item_t *))event_lf_init_custom_memory_free_test != p->free) { errors++; }



  event_lf_t o = { 0 };
  memset(&o, 0xff, sizeof(event_lf_t));

  // compound literal (C99 and later)
  o = (event_lf_t)EVENT_LF_INIT_CUSTOM_MEMORY(
    &m,
    (event_lf_item_t *(*)(void *))event_lf_init_custom_memory_allocate_test,
    (void(*)(void *, event_lf_item_t *))event_lf_init_custom_memory_free_test);
  

  if (NULL != atomic_load(&o.list)) { errors++; }
  if (0 != atomic_load(&o.threads)) { errors++; }
  if (0 != atomic_load(&o.remove_count)) { errors++; }
  if (NULL != o.remove) { errors++; }
  if (&m != o.memory_object) { errors++; }
  if ((event_lf_item_t *(*)(void *))event_lf_init_custom_memory_allocate_test != o.allocate) { errors++; }
  if ((void(*)(void *, event_lf_item_t *))event_lf_init_custom_memory_free_test != o.free) { errors++; }

  
  return 0;
}

static void handler1(void * sender, void * e) { out_index += sprintf(out+out_index, "1"); printf("1\n"); UNUSED(sender); UNUSED(e); }
static void handler2(void * sender, void * e) { out_index += sprintf(out+out_index, "2"); printf("2\n"); UNUSED(sender); UNUSED(e); }
static void handler3(void * sender, void * e) { out_index += sprintf(out+out_index, "3"); printf("3\n"); UNUSED(sender); UNUSED(e); }

static inline bool IS_FLAG_SET(uint16_t value, uint16_t flags)
{
  return (flags & value) == flags;
}

static inline bool IS_MASKED_BITS_EQUAL(uint16_t value, uint16_t mask, uint16_t flags)
{
  return (mask & value) == flags;
}

uint16_t event_lf_add_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data2[2];
  event_lf_item_t * i;
  bool result;
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();
  event_lf_memory_add(&m, data2, countof(data2));

  event_lf_t e = EVENT_LF_INIT(&m);

  result = event_lf_add(NULL, handler1);
  if (false != result) { errors++; }
  if (NULL != atomic_load(&e.list)) { errors++; }
  if (0 != m.used) { errors++; }

  result = event_lf_add(&e, NULL);
  if (false != result) { errors++; }
  i = atomic_load(&e.list);
  if (false == IS_MASKED_BITS_EQUAL(atomic_load(&i->state), EVENT_LF_STATE_FLAG_IS_VALID | EVENT_LF_STATE_FLAG_IS_ALIVE, EVENT_LF_STATE_FLAG_IS_ALIVE)) { errors++; }
  if (data2 + 0 != i) { errors++; }
  if (NULL != atomic_load(&i->next)) { errors++; }
  if (1 != m.used) { errors++; }
  

  result = event_lf_add(&e, NULL);
  if (false != result) { errors++; }
  i = atomic_load(&e.list);
  if (false == IS_MASKED_BITS_EQUAL(atomic_load(&i->state), EVENT_LF_STATE_FLAG_IS_VALID | EVENT_LF_STATE_FLAG_IS_ALIVE, EVENT_LF_STATE_FLAG_IS_ALIVE)) { errors++; }
  if (data2 + 0 != i) { errors++; }
  if (NULL != atomic_load(&i->next)) { errors++; }
  if (1 != m.used) { errors++; }
  
  result = event_lf_add(&e, handler1);
  if (true != result) { errors++; }
  i = atomic_load(&e.list);
  if (false == IS_FLAG_SET(atomic_load(&i->state), EVENT_LF_STATE_FLAG_IS_VALID | EVENT_LF_STATE_FLAG_IS_ALIVE)) { errors++; }
  if (data2 + 0 != i) { errors++; }
  if (handler1 != i->function) { errors++; }
  if (2 != m.used) { errors++; }

  i = atomic_load(&i->next);
  if (false == IS_MASKED_BITS_EQUAL(atomic_load(&i->state), EVENT_LF_STATE_FLAG_IS_VALID | EVENT_LF_STATE_FLAG_IS_ALIVE, EVENT_LF_STATE_FLAG_IS_ALIVE)) { errors++; }
  if (data2 + 1 != i) { errors++; }
  if (NULL != atomic_load(&i->next)) { errors++; }


  return errors;
}

uint16_t event_lf_count_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data5[5];
  uint16_t count;
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();
  event_lf_memory_add(&m, data5, countof(data5));

  event_lf_t e = EVENT_LF_INIT(&m);

  if (0 != (count = event_lf_count(NULL))) { errors++; }
  if (0 != (count = event_lf_count(&e))) { errors++; }

  event_lf_add(&e, NULL);
  if (0 != (count = event_lf_count(&e))) { errors++; }

  event_lf_add(&e, handler1);
  if (1 != (count = event_lf_count(&e))) { errors++; }

  event_lf_add(&e, handler1);
  if (2 != (count = event_lf_count(&e))) { errors++; }
  
  event_lf_add(&e, handler2);
  if (3 != (count = event_lf_count(&e))) { errors++; }
  
  event_lf_add(&e, handler3);
  if (4 != (count = event_lf_count(&e))) { errors++; }
  
  event_lf_add(&e, handler3);
  if (4 != (count = event_lf_count(&e))) { errors++; }


  return errors;
}

uint16_t event_lf_dispose_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data5[5];
  bool result;
  uint16_t count;
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();
  event_lf_memory_add(&m, data5, countof(data5));

  event_lf_t e = EVENT_LF_INIT(&m);

  if (0 != (count = event_lf_count(NULL))) { errors++; }
  if (0 != (count = event_lf_count(&e))) { errors++; }

  event_lf_add(&e, NULL);
  event_lf_add(&e, handler1);
  event_lf_add(&e, handler1);
  event_lf_add(&e, handler2);
  event_lf_add(&e, handler3);
  
  if (5 != m.used) { errors++; }

  result = event_lf_dispose(NULL);
  if (false != result) { errors++; }

  result = event_lf_dispose(&e);
  if (true != result) { errors++; }
  if (0 != m.used) { errors++; }
  
  event_lf_add(&e, NULL);
  event_lf_add(&e, handler1);
  event_lf_add(&e, handler1);
  event_lf_add(&e, handler2);
  event_lf_add(&e, handler3);
  
  if (5 != m.used) { errors++; }
  result = event_lf_dispose(&e);
  if (true != result) { errors++; }
  if (0 != m.used) { errors++; }

  return errors;
}

static uint16_t event_lf_foreach_counter_test = 0;
static uint16_t event_lf_foreach_errors_test = 0;
static void event_lf_foreach_function_test(void *sender, event_lf_item_t *item)
{
  event_lf_item_t * data5 = sender;

  if (data5 + event_lf_foreach_counter_test != item) 
  {
    event_lf_foreach_errors_test++;
  }

  event_lf_foreach_counter_test++;
}

uint16_t event_lf_foreach_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data5[5];
  uint16_t count;
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();
  event_lf_memory_add(&m, data5, countof(data5));

  event_lf_t e = EVENT_LF_INIT(&m);

  if (0 != (count = event_lf_count(NULL))) { errors++; }
  if (0 != (count = event_lf_count(&e))) { errors++; }

  event_lf_add(&e, NULL);
  event_lf_add(&e, handler1);
  event_lf_add(&e, handler1);
  event_lf_add(&e, handler2);
  event_lf_add(&e, handler3);

  event_lf_foreach(&e, &e, NULL);

  event_lf_foreach_counter_test = 0;
  event_lf_foreach_errors_test = 0;
  event_lf_foreach(&e, data5, event_lf_foreach_function_test);

  errors += event_lf_foreach_errors_test;

  return errors;
}

static uint16_t check_out(const char * const result)
{
  uint16_t errors = 0;

  size_t size = strlen(result);

  if (size != out_index)
  {
    errors++;
  }

  if (0 != (memcmp(out, result, size)))
  {
    errors++;
  }

  // Reset buffer
  memset(out, 0, sizeof(out));
  out_index = 0;

  return errors;
}

uint16_t event_lf_invoke_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data5[5];
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();
  event_lf_memory_add(&m, data5, countof(data5));

  event_lf_t e = EVENT_LF_INIT(&m);

  event_lf_invoke(NULL, NULL, NULL);

  event_lf_add(&e, NULL);
  event_lf_add(&e, handler1);
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("1");

  event_lf_add(&e, handler1);
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("11");

  event_lf_add(&e, handler2);
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("112");

  event_lf_add(&e, handler3);
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("1123");

  return errors;
}

uint16_t event_lf_sub_test(void)
{
  uint16_t errors = 0;

  event_lf_item_t data11[11];
  event_lf_memory_t m = EVENT_LF_MEMORY_INIT();
  event_lf_memory_add(&m, data11, countof(data11));

  event_lf_t e = EVENT_LF_INIT(&m);

  event_lf_sub(&e, NULL);
  event_lf_sub(NULL, handler2);

  event_lf_add(&e, NULL);
  event_lf_add(&e, handler1);
  event_lf_add(&e, handler2);
  event_lf_add(&e, handler3);
  if ((3+1) != m.used) { errors++; }
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("123");

  event_lf_add(&e, handler1);
  event_lf_add(&e, handler2);
  event_lf_add(&e, handler3);
  if ((6+1) != m.used) { errors++; }
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("123123");

  event_lf_sub(&e, handler2);
  if ((5+1) != m.used) { errors++; }
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("12313");

  event_lf_sub(&e, handler2);
  if ((4+1) != m.used) { errors++; }
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("1313");
  
  // simulate another thread start
  atomic_fetch_add(&e.threads, 1);

  event_lf_sub(&e, handler1);
  if ((4+1) != m.used) { errors++; }
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("133");
  
  // simulate another thread end
  atomic_fetch_sub(&e.threads, 1);

  event_lf_sub(&e, handler3);
  if ((2+1) != m.used) { errors++; }
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("13");

  event_lf_sub(&e, handler3);
  if ((1+1) != m.used) { errors++; }
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("1");

  event_lf_sub(&e, handler3);
  if ((1+1) != m.used) { errors++; }
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("1");
  
  event_lf_sub(&e, handler1);
  if ((0+1) != m.used) { errors++; }
  event_lf_invoke(&e, NULL, NULL);
  errors += check_out("");

  return errors;
}

static void h1(void * sender, void * e) { UNUSED(sender); printf("%i", *((int * )e)*4 + 1); }
static void h2(void * sender, void * e) { UNUSED(sender); printf("%i", *((int * )e)*4 + 2); }
static void h3(void * sender, void * e) { UNUSED(sender); printf("%i", *((int * )e)*4 + 3); }
static void h4(void * sender, void * e) { UNUSED(sender); printf("%i", *((int * )e)*4 + 4); }

event_lf_item_t data10[4*(4+1)];
event_lf_memory_t memory10 = EVENT_LF_MEMORY_INIT();

int event_lf_thread_test_function(int thread_number)
{
  uint16_t errors = 0;

  bool result;
  uint16_t count;

  event_lf_t e = EVENT_LF_INIT(&memory10);

  if (0 != (count = event_lf_count(NULL))) { errors++; }
  if (0 != (count = event_lf_count(&e))) { errors++; }

  result = event_lf_add(&e, NULL);
  if (false != result) { errors++; }
  result = event_lf_add(&e, h1);
  if (true != result) { errors++; }
  result = event_lf_add(&e, h1);
  if (true != result) { errors++; }
  result = event_lf_add(&e, h2);
  if (true != result) { errors++; }
  result = event_lf_add(&e, h3);
  if (true != result) { errors++; }
  event_lf_invoke(&e, NULL, &thread_number);

  result = event_lf_dispose(NULL);
  if (false != result) { errors++; }

  result = event_lf_dispose(&e);
  if (true != result) { errors++; }

  result = event_lf_add(&e, NULL);
  if (false != result) { errors++; }
  result = event_lf_add(&e, h1);
  if (true != result) { errors++; }
  result = event_lf_add(&e, h2);
  if (true != result) { errors++; }
  result = event_lf_add(&e, h3);
  if (true != result) { errors++; }
  result = event_lf_add(&e, h4);
  if (true != result) { errors++; }
  event_lf_invoke(&e, NULL, &thread_number);

  result = event_lf_dispose(&e);
  if (true != result) { errors++; }

  return errors;
}

// Function that both threads will run
void* event_lf_thread_test_worker_function(void * arg)
{
  int thread_number = *((int *)arg);
  int * errors = arg;
  *errors = 0;


  for (int i = 0; i < 50000; i++)
  {
      *errors += event_lf_thread_test_function(thread_number);
  }

    return NULL;
}

int event_lf_thread_test() {
    pthread_t thread0;
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;

    int errors0 = 0;
    int errors1 = 1;
    int errors2 = 2;
    int errors3 = 3;

    event_lf_memory_add(&memory10, data10, countof(data10));

    // Create two threads running the same function
    pthread_create(&thread0, NULL, event_lf_thread_test_worker_function, &errors0);
    pthread_create(&thread1, NULL, event_lf_thread_test_worker_function, &errors1);
    pthread_create(&thread2, NULL, event_lf_thread_test_worker_function, &errors2);
    pthread_create(&thread3, NULL, event_lf_thread_test_worker_function, &errors3);

    // Wait for both threads to finish
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    printf("Both threads finished.\n");

    return errors0 + errors1 + errors2 + errors3;
}



#ifdef EVENT_LF_ALLOW_STANDARD_MALLOC_FREE

int event_lf_standard_malloc_free_test(void)
{
  uint16_t errors = 0;

  event_lf_t e = EVENT_LF_INIT_STANDARD_MALLOC_FREE();

  event_lf_add(&e, handler1);
  event_lf_add(&e, handler2);
  event_lf_add(&e, handler3);
  event_lf_invoke(&e, NULL, NULL);

  errors += check_out("123");

  return errors;
}

#endif

#endif


void event_lf_example1(void)
{
  event_lf_item_t data10[10];

  event_lf_memory_t memory = EVENT_LF_MEMORY_INIT();
  event_lf_memory.Add(&memory, data10, countof(data10));

  event_lf_t e1 = EVENT_LF_INIT(&memory);
  event_lf.Init(&e1, &memory);
  event_lf.Add(&e1, NULL);

  event_lf.Add(&e1, handler1);
  event_lf.Add(&e1, handler2);
  event_lf.Add(&e1, handler3);
  event_lf.Invoke(&e1, NULL, NULL); // 1, 2, 3

  event_lf.Sub(&e1, handler2);
  event_lf.Invoke(&e1, NULL, NULL); // 1, 3

  event_lf.Dispose(&e1);
}

void event_lf_example2(void)
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

int event_lf_test(void)
{
  int errors = 0;

  errors += event_lf_memory_init_test();
  errors += event_lf_memory_add_test();
  errors += event_lf_memory_allocate_test();
  errors += event_lf_memory_free_test();
  errors += event_lf_init_test();
  errors += event_lf_init_custom_memory_test();
  errors += event_lf_add_test();
  errors += event_lf_count_test();
  errors += event_lf_dispose_test();
  errors += event_lf_foreach_test();
  errors += event_lf_invoke_test();
  errors += event_lf_sub_test();
  errors += event_lf_thread_test();

#ifdef EVENT_LF_ALLOW_STANDARD_MALLOC_FREE
  errors += event_lf_standard_malloc_free_test();
#endif

  errors += check_lock_free_runtime() ? 0 : 1;

  event_lf_example1();
  event_lf_example2();

  return errors;
}


/*---------------------------------------------------------------------*
 *  eof                                                                *
 *---------------------------------------------------------------------*/
