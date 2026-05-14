//! @file
//! @brief Source file EventLf.cpp


// ------------------------------------------------------------------------- //
//  private: include files
// ------------------------------------------------------------------------- //

#include "EventLf.h"


// ------------------------------------------------------------------------- //
//  private: definitions
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: macros like functions
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: typedefs
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: variables
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public:  variables
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: function prototypes
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: inline functions
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private:  functions
// ------------------------------------------------------------------------- //

void EventLfMemory::Init ()
{
  event_lf_memory_init(c_object);
}

void EventLf::Init (event_lf_memory_t * const memory_object)
{
  event_lf_init(c_object, memory_object);
}

void EventLf::InitCustomMemory (
  void * const memory_object,
  event_lf_item_t * (* allocate)(void * memory_object),
  void (* free)(void * memory_object, event_lf_item_t * mem))
{
  event_lf_init_custom_memory(c_object, memory_object, allocate, free);
}


// ------------------------------------------------------------------------- //
//  public:  functions
// ------------------------------------------------------------------------- //

EventLfMemory::EventLfMemory ()
{
  c_object = &objectData;
  Init();
}

EventLfMemory::EventLfMemory (event_lf_item_t * const data, size_t countof_data)
{
  c_object = &objectData;
  Init();
  Add(data, countof_data);
}

EventLfMemory::EventLfMemory (std::span<event_lf_item_t> data)
{
  c_object = &objectData;
  Init();
  Add(data);
}

void EventLfMemory::Add (event_lf_item_t * const data, size_t countof_data)
{
  event_lf_memory_add(c_object, data, countof_data);
}

void EventLfMemory::Add (std::span<event_lf_item_t> data)
{
  event_lf_memory_add(c_object, data.data(), data.size());
}

event_lf_item_t * EventLfMemory::Allocate ()
{
  return event_lf_memory_allocate(c_object);
}

void EventLfMemory::Free (event_lf_item_t * item)
{
  event_lf_memory_free(c_object, item);
}

uint16_t EventLfMemory::getUsed()
{
  return c_object->used;
}

uint16_t EventLfMemory::getCapacity()
{
  return c_object->capacity;
}



bool EventLf::CheckLockFreeRuntime ()
{
  return check_lock_free_runtime();
}

EventLf::EventLf (event_lf_memory_t * const memory_object)
{
  c_object = &objectData;
  Init(memory_object);
  Add(NULL);
}

EventLf::EventLf (EventLfMemory & memory_class)
{
  c_object = &objectData;
  Init(memory_class.c_object);
  Add(NULL);
}

EventLf::EventLf (
  event_lf_memory_t * const memory_object,
  event_lf_item_t * (* allocate)(void * memory_object),
  void (* free)(void * memory_object, event_lf_item_t * mem))
{
  c_object = &objectData;
  InitCustomMemory(memory_object, allocate, free);
  Add(NULL);
}

#ifdef EVENT_LF_ALLOW_STANDARD_MALLOC_FREE

//! @brief Initialize the class and uses the standard malloc and free function
EventLf::EventLf ()
{
  c_object = &objectData;
  InitCustomMemory(NULL, event_lf_standard_malloc, event_lf_standard_free);
  Add(NULL);
}

#endif


EventLf::~EventLf ()
{
  Dispose();
}

bool EventLf::Add (function_pointer const function)
{
  return event_lf_add(c_object, function);
}

uint16_t EventLf::Count ()
{
  return event_lf_count(c_object);
}

bool EventLf::Dispose ()
{
  return event_lf_dispose(c_object);
}

void EventLf::Foreach (void * sender, void (*function)(void * sender, event_lf_item_t * item) )
{
  event_lf_foreach(c_object, sender, function);
}

void EventLf::Invoke (void * const sender, void * const e)
{
  event_lf_invoke(c_object, sender, e);
}

void EventLf::Sub (function_pointer const function)
{
  event_lf_sub(c_object, function);
}



// ------------------------------------------------------------------------- //
//  eof
// ------------------------------------------------------------------------- //
