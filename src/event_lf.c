//! @file
//! @brief Source file event_lf.c


// ------------------------------------------------------------------------- //
//  private: include files
// ------------------------------------------------------------------------- //

#include "event_lf.h"


// ------------------------------------------------------------------------- //
//  private: definitions
// ------------------------------------------------------------------------- //

/// @def INLINE
/// @brief A way for INLINE to use other commands
/// @details Checks whether ::INLINE is already defined; if not, the
///          C/C++ `inline` keyword is used.
#ifndef INLINE
#define INLINE inline
#endif

/// @def UNUSED
/// @brief A way for UNUSED to use other commands
/// @details Marks a variable, parameter, or function as intentionally unused.
///          On GCC- and Clang-compatible compilers, this expands to the
///          `__attribute__((unused))` attribute to suppress compiler warnings
///          about unused entities.
///
#ifndef UNUSED
  #if defined(__GNUC__) || defined(__clang__)
    #define UNUSED __attribute__((unused))
  #else
    #define UNUSED
  #endif
#endif


// ------------------------------------------------------------------------- //
//  private: macros like functions
// ------------------------------------------------------------------------- //

/// @brief Estimate whether the type is lock-free, based on its size
///
/// @details This macro maps only the size of a given type `T` to the
///          corresponding `ATOMIC_*_LOCK_FREE` macro defined in
///          `<stdatomic.h>`. It provides a compile-time estimation of
///          whether atomic operations on objects of that size are
///          lock-free on the target platform.
///
///          For precise, runtime determination, use `atomic_is_lock_free()`.
///
/// @param T The type to check for lock-free atomic support
///
/// @return An integer constant indicating the lock-free property 
/// @retval 0 Never lock-free or no matching fundamental type exists
/// @retval 1 Sometimes lock-free
/// @retval 2 Always lock-free
///
#define ATOMIC_LOCK_FREE_BY_SIZE(T) \
(                                                           \
  sizeof(T) == sizeof(char)      ? ATOMIC_CHAR_LOCK_FREE  : \
  sizeof(T) == sizeof(short)     ? ATOMIC_SHORT_LOCK_FREE : \
  sizeof(T) == sizeof(int)       ? ATOMIC_INT_LOCK_FREE   : \
  sizeof(T) == sizeof(long)      ? ATOMIC_LONG_LOCK_FREE  : \
  sizeof(T) == sizeof(long long) ? ATOMIC_LLONG_LOCK_FREE : \
  0                                                         \
)                                                          // ;

/// @brief Estimate at compile-time that a type is always lock-free for atomic operations
///
/// @details If the type is not always lock-free, compilation will fail with
///          an error message indicating the offending type.
///
///          To perform a runtime test use `check_lock_free_runtime()`
///
/// @param T The type to check for lock-free atomic support.
///
#define CHECK_LOCK_FREE_COMPILE_TIME(T) \
  _Static_assert(                                  \
    ATOMIC_LOCK_FREE_BY_SIZE(T) == 2,              \
    #T " is not always lock-free on this platform" \
  )                                               // ;


/// @cond INTERNAL

CHECK_LOCK_FREE_COMPILE_TIME(event_lf_item_t *);

CHECK_LOCK_FREE_COMPILE_TIME(event_lf_state_flag_t);

CHECK_LOCK_FREE_COMPILE_TIME(uint16_t);

#if ATOMIC_POINTER_LOCK_FREE != 2
#error "Atomic pointer is not always lock-free; signal handler may deadlock."
#endif

/// @endcond


// ------------------------------------------------------------------------- //
//  private: typedefs
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: variables
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public:  variables
// ------------------------------------------------------------------------- //

const struct event_lf_memory_sc event_lf_memory = 
{
  /* .Add      = */ event_lf_memory_add,
  /* .Allocate = */ event_lf_memory_allocate,
  /* .Free     = */ event_lf_memory_free,
  /* .Init     = */ event_lf_memory_init,
};

const struct event_lf_sc event_lf =
{  
  /* .Add              = */ event_lf_add,
  /* .Count            = */ event_lf_count,
  /* .Dispose          = */ event_lf_dispose,
  /* .Foreach          = */ event_lf_foreach,
  /* .Init             = */ event_lf_init,
  /* .InitCustomMemory = */ event_lf_init_custom_memory,
  /* .Invoke           = */ event_lf_invoke,
  /* .Sub              = */ event_lf_sub,
};


// ------------------------------------------------------------------------- //
//  private: function prototypes
// ------------------------------------------------------------------------- //

/// @brief Try moving items from `list` to `remove`
///
/// The function must be called in such a way that only one instance
/// is active at a time
///
/// @param[in] object The event object itself, must not be null
static void event_lf_sort_remove(event_lf_t * object);

/// @brief Tries to remove items from the list `remove`
///
/// The function must be called in such a way that only one instance
/// is active at a time
///
/// @param[in] object The event object itself, must not be null
static void event_lf_sort_free(event_lf_t * object);


// ------------------------------------------------------------------------- //
//  private: inline functions
// ------------------------------------------------------------------------- //

/// @brief Check if specific flags are set in a value.
/// 
/// @details This function verifies whether all bits specified in the @p flags
///          parameter are set in the given @p value.
/// 
/// @param value The value to be checked.
/// @param flags The bitmask representing the flags to test.
/// @return Checks if the bits are set
/// @retval true If all bits in @p flags are set in @p value.
/// @retval false Otherwise.
static INLINE bool IS_FLAG_SET(uint16_t value, uint16_t flags)
{
  return (flags & value) == flags;
}

/// @brief Checks whether specific masked bits in a value match given flags.
/// 
/// @details This function applies a bitmask to the input value and compares
///          the result against the expected flags.
/// 
/// @param value The input value whose bits will be tested.
/// @param mask  The bitmask used to select relevant bits from @p value .
/// @param flags The expected bit pattern after masking.
/// 
/// @return If the masked bits are equal to the given flags
/// @retval true If ( @p value & @p mask ) equals @p flags
/// @retval false Otherwise.
static INLINE UNUSED bool IS_MASKED_BITS_EQUAL(uint16_t value, uint16_t mask, uint16_t flags)
{
  return (mask & value) == flags;
}

/// @brief Check if specific flags are clear in a value.
/// 
/// @details This function verifies whether all bits specified in the @p flags
///          parameter are cleared in the given @p value.
/// 
/// @param value The value to be checked.
/// @param flags The bitmask representing the flags to test.
/// @return Checks if the bits are cleared
/// @retval true  If none of the bits in @p flags are set in @p value.
/// @retval false If any bit in @p flags is set in @p value.
static INLINE bool IS_FLAG_CLEAR(uint16_t value, uint16_t flags)
{
  return 0 == (flags & value);
}

/// @brief Checks whether another thread was running at the time of the check
/// @param[in] object The event object itself, must not be null
/// @return Returns whether another thread is running
/// @retval true  if another thread has run or is still running
/// @retval false if another thread wasn't running or still isn't running
static INLINE bool EVENT_LF_IS_ANOTHER_THREAD_RUNNING(const event_lf_t * const object)
{
    return 1 < atomic_load(&object->threads);
}

/// @brief Marks the object as in use, for concurrent environments
/// @param[in,out] object The event object itself, must not be null
static INLINE void EVENT_LF_OBJECT_USE_BEGIN(event_lf_t * const object)
{
    atomic_fetch_add(&object->threads, 1);
}

/// @brief Ends the current use of the object, for concurrent environments
/// @param[in,out] object The event object itself, must not be null

static INLINE void EVENT_LF_OBJECT_USE_END(event_lf_t * const object)
{
  // - This ensures that the following functions are not called in
  //   parallel by another thread.
  // - Normally, it would have been enough to block that one function
  // - It's possible that, in the end, no one will have called the
  //   following functions
  if (false == EVENT_LF_IS_ANOTHER_THREAD_RUNNING(object))
  {
    // As long as another thread is running, there is no guarantee that it is
    // still accessing a soft deleted moved item. Once it has been determined
    // that no other thread is running, the moved items can be deleted.
    event_lf_sort_remove(object);
    

    if (false == EVENT_LF_IS_ANOTHER_THREAD_RUNNING(object))
    {
      event_lf_sort_free(object);
    }
  }
  
  atomic_fetch_sub(&object->threads, 1);
}


// ------------------------------------------------------------------------- //
//  private:  functions
// ------------------------------------------------------------------------- //

static void event_lf_sort_remove(event_lf_t * const object)
{
  uint16_t remove_count = atomic_load(&object->remove_count);
  if (0 == remove_count) { return; }

  event_lf_item_t * item;
  event_lf_item_t * next;
  event_lf_state_flag_t state;
  uint16_t count = remove_count;

  volatile _Atomic(event_lf_item_t *) * item_address = &object->list;

  while (NULL != (item = atomic_load(item_address)) && 0 < count)
  {
    state = atomic_load(&item->state);

    // If the flag is not set the item can be removed
    if (IS_FLAG_CLEAR(state, EVENT_LF_STATE_FLAG_IS_ALIVE))
    {
      next = atomic_load(&item->next);
      atomic_store(item_address, next);

      // Add item to remove
      next = object->remove;
      item->remove = next;
      object->remove = item;

      count--;

      // item_address remains the same
    }
    else
    {
      item_address = &item->next;
    }
  }

  atomic_fetch_sub(&object->remove_count, remove_count - count);

  return;
}

static void event_lf_sort_free(event_lf_t * const object)
{
  event_lf_item_t * item;

  while (NULL != (item = object->remove))
  {
    // Unhook the first item 
    object->remove = item->remove;

    object->free(object->memory_object, item);
  }
}


// ------------------------------------------------------------------------- //
//  public:  functions
// ------------------------------------------------------------------------- //

bool check_lock_free_runtime(void)
{
  _Atomic(event_lf_item_t *) var1;
  _Atomic(event_lf_state_flag_t) var2;
  _Atomic(uint16_t) var3;

  return atomic_is_lock_free(&var1)
    && atomic_is_lock_free(&var2)
    && atomic_is_lock_free(&var3);
}


void event_lf_memory_add(event_lf_memory_t * object, event_lf_item_t * const data, size_t countof_data)
{
  if (NULL == object) { return; }

  if (NULL == data) { return; }
  
  if (0 == countof_data) { return; }

  // The data can be concatenated without any issues,
  // as long as it has not yet been appended.
  event_lf_item_t * item = data;
  for (int i = countof_data-1; 0 < i; i--)
  {
    atomic_store(&item->next, item + 1);
    item++;
  }
  // Now `item` is the last element of `data`

  // The data is attached here
  event_lf_item_t * existing;
  existing = atomic_load(&object->next);
  
  while(true)
  {
    atomic_store(&item->next, existing);

    if(atomic_compare_exchange_strong(&object->next, &existing, data))
    {
      break;
    }
  }

  // The added data increases the capacity
  atomic_fetch_add(&object->capacity, countof_data);
}

event_lf_item_t * event_lf_memory_allocate(event_lf_memory_t * object)
{
  if (NULL == object) { return NULL; }

  event_lf_item_t * mem = atomic_load(&object->next);
  event_lf_item_t * next;
    
  while (NULL != mem)
  {
    next = mem->next;

    if (atomic_compare_exchange_strong(&object->next, &mem, next))
    {
      atomic_fetch_add(&object->used, 1);
      return mem;
    }

  } 
  
  return NULL;
}

void event_lf_memory_free(event_lf_memory_t * object, event_lf_item_t * item)
{
  if (NULL == object) { return; }

  if (NULL == item) { return; }

  event_lf_item_t * mem = atomic_load(&object->next);

  while (true)
  {
    item->next = mem;

    if (atomic_compare_exchange_strong(&object->next, &mem, item))
    {
      atomic_fetch_sub(&object->used, 1);
      return;
    }
  }
}

void event_lf_memory_init(event_lf_memory_t * object)
{
  if (NULL == object) { return; }

  atomic_store(&object->next, NULL);
  atomic_store(&object->used, 0);
  atomic_store(&object->capacity, 0);
}


bool event_lf_add(event_lf_t * const object, function_pointer const function)
{
  if (NULL == object) { return false; }

  EVENT_LF_OBJECT_USE_BEGIN(object);

  bool result;
  event_lf_item_t * new_item;
  event_lf_item_t * item;
  event_lf_item_t * next;
  event_lf_item_t * expected;

  new_item = NULL;
  result = false;
  item = NULL;

  do
  {
    volatile _Atomic(event_lf_item_t *) * item_address = &object->list;
    if (NULL == (next = atomic_load(item_address)))
    {
      // There is no last element, so add a new one 
      event_lf_item_t * new_item = object->allocate(object->memory_object);
      if (NULL == new_item) { break; }

      // Inits members of new struct
      atomic_init(&new_item->next, NULL);
      new_item->function = NULL;
      atomic_init(&new_item->state, EVENT_LF_STATE_FLAG_IS_ALIVE);
      new_item->remove = NULL;

      event_lf_item_t * expected = NULL;
      if (false == atomic_compare_exchange_strong(item_address, &expected, new_item))
      {
        // If the add function is called twice at the same time,
        // only one of them can initialize the first element
        object->free(object->memory_object, new_item);
      }

      next = atomic_load(item_address);
    }

    // call with null can be used to init the event
    if (NULL == function) { break; }

    // Go through all the items and finds last
    do
    {
      item = next;
      item_address = &item->next;
    } while (NULL != (next = atomic_load(item_address)));

    new_item = object->allocate(object->memory_object);
    if (NULL == new_item) { break; }

    // Inits members of new struct
    atomic_init(&new_item->next, NULL);
    new_item->function = NULL;
    atomic_init(&new_item->state, EVENT_LF_STATE_FLAG_IS_ALIVE);
    new_item->remove = NULL;

    while (true)
    {
      // If you can write into next of the item, you own it.
      expected = NULL;
      if (atomic_compare_exchange_strong(item_address, &expected, new_item))
      {
        item->function = function;
        atomic_fetch_or(&item->state, EVENT_LF_STATE_FLAG_IS_VALID);
        result = true;
        break;
      }

      item_address = &expected->next;
      while (NULL != (next = atomic_load(item_address)))
      {
        item = next;
        item_address = &item->next;
      }

    }

  } while (false);

  EVENT_LF_OBJECT_USE_END(object);

  return result;
}

uint16_t event_lf_count(event_lf_t * const object)
{
  if (NULL == object) { return 0; }

  EVENT_LF_OBJECT_USE_BEGIN(object);

  event_lf_item_t * item;
  event_lf_state_flag_t state;
  uint16_t count = 0;

  // Iterates through the `list` and calls each function
  volatile _Atomic(event_lf_item_t *) * item_address = &object->list;
  while (NULL != (item = atomic_load(item_address)))
  {
    state = atomic_load(&item->state);
  
    if (IS_FLAG_SET(state, EVENT_LF_STATE_FLAG_IS_VALID | EVENT_LF_STATE_FLAG_IS_ALIVE))
    {
      count++;
    }

    item_address = &item->next;
  }

  EVENT_LF_OBJECT_USE_END(object);

  return count;
}

bool event_lf_dispose(event_lf_t * const object)
{
  if (NULL == object) { return false; }

  EVENT_LF_OBJECT_USE_BEGIN(object);

  event_lf_item_t * item;
  volatile _Atomic(event_lf_item_t *) * item_address;

  uint16_t count = 0;
  // Iterates through the `list`
  item_address = &object->list;
  while (NULL != (item = atomic_load(item_address)))
  {
    // Clears the valid flag and performs a soft-delete
    atomic_fetch_and(&item->state, ~EVENT_LF_STATE_FLAG_IS_ALIVE);
    item_address = &item->next;

    // All items needs to be freed
    count++;
  }

  atomic_store(&object->remove_count, count);

  EVENT_LF_OBJECT_USE_END(object);

  return (false == EVENT_LF_IS_ANOTHER_THREAD_RUNNING(object) 
    && NULL == atomic_load(&object->list) 
    && NULL == object->remove);
}

void event_lf_foreach(event_lf_t * const object, void * sender, void (*function)(void * sender, event_lf_item_t * item) )
{
  if (NULL == object) { return; }

  if (NULL == function) { return; }

  EVENT_LF_OBJECT_USE_BEGIN(object);

  event_lf_item_t * item;
  event_lf_state_flag_t state;

  // Iterates through the `list` and calls each function
  volatile _Atomic(event_lf_item_t *) * item_address = &object->list;
  while (NULL != (item = atomic_load(item_address)))
  {
    state = atomic_load(&item->state);
  
    if (IS_FLAG_SET(state, EVENT_LF_STATE_FLAG_IS_VALID | EVENT_LF_STATE_FLAG_IS_ALIVE))
    {
      function(sender, item);
    }

    item_address = &item->next;
  }

  EVENT_LF_OBJECT_USE_END(object);
}

void event_lf_init(event_lf_t * const object, event_lf_memory_t * const memory_object)
{
  event_lf_init_custom_memory(
    object,
    memory_object,
    (event_lf_item_t *(*)(void *))event_lf_memory_allocate,
    (void(*)(void *, event_lf_item_t *))event_lf_memory_free);
}

void event_lf_init_custom_memory(
  event_lf_t * const object,
  void * const memory_object,
  event_lf_item_t * (*allocate)(void * memory_object),
  void (*free)(void * memory_object, event_lf_item_t * mem))
{
  if (NULL == object) { return; }

  atomic_store(&object->list, NULL);
  atomic_store(&object->threads, 0);
  atomic_store(&object->remove_count, 0);
  object->remove = NULL;

  object->memory_object = memory_object;
  object->allocate = allocate;
  object->free = free;
}

void event_lf_invoke(event_lf_t * const object, void * const sender, void * const e)
{
  if (NULL == object) { return; }

  EVENT_LF_OBJECT_USE_BEGIN(object);

  event_lf_item_t * item;
  event_lf_state_flag_t state;

  // Iterates through the `list` and calls each function
  volatile _Atomic(event_lf_item_t *) * item_address = &object->list;
  while (NULL != (item = atomic_load(item_address)))
  {
    state = atomic_load(&item->state);
  
    if (IS_FLAG_SET(state, EVENT_LF_STATE_FLAG_IS_VALID | EVENT_LF_STATE_FLAG_IS_ALIVE))
    {
      item->function(sender, e);
    }

    item_address = &item->next;
  }

  EVENT_LF_OBJECT_USE_END(object);
}

void event_lf_sub(event_lf_t * const object, function_pointer const function)
{
  if (NULL == object) { return; }

  EVENT_LF_OBJECT_USE_BEGIN(object);

  do
  {  
    if (NULL == function) { break; }
    
    event_lf_item_t * item;
    event_lf_item_t * found;
    event_lf_state_flag_t state;
    volatile _Atomic(event_lf_item_t *) * item_address;

    while (true)
    {
      // Iterates through the `list` and search for the function
      found = NULL;
      item_address = &object->list;
      while (NULL != (item = atomic_load(item_address)))
      {
        state = atomic_load(&item->state);

        if (function == item->function && IS_FLAG_SET(state, EVENT_LF_STATE_FLAG_IS_VALID | EVENT_LF_STATE_FLAG_IS_ALIVE ))
        {
          found = item;
        }

        item_address = &item->next;
      }

      if (NULL != found)
      {
        // If an item is found, the valid flag is cleared, and the item is
        // thus soft-deleted
        state = atomic_fetch_and(&found->state, ~EVENT_LF_STATE_FLAG_IS_ALIVE);
        
        if (IS_FLAG_CLEAR(state, EVENT_LF_STATE_FLAG_IS_ALIVE))
        {
          // Another thread cleared it previous so find another item.
          continue;
        }
        
        atomic_fetch_add(&object->remove_count, 1);
      }

      break;
    }

  } while (false);

  EVENT_LF_OBJECT_USE_END(object);
}


// ------------------------------------------------------------------------- //
//  eof
// ------------------------------------------------------------------------- //
