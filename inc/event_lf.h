//! @file
//! @brief The event_lf header file.
//!
//! @details The module can be used in C and C++.


#ifndef INC_EVENT_LF_H
#define INC_EVENT_LF_H


// ------------------------------------------------------------------------- //
//  public: atomic handling
// ------------------------------------------------------------------------- //

//! @defgroup stdatomic_support_in_c_and_cpp To use stdatomic, you must use at
//!           least the following versions
//!
//! @details To use the library in C, all you need is c11. In C++, you can also
//!          use the library with c++11. In this case, however, interface
//!          functions are required due to the different types, and this header
//!          cannot be used. This header is intended for use in both C and C++.
//!          Therefore, the header must work in both languages. stdatomic can
//!          be used, but only starting with c++23.
//!
//! @{

//! @def HAS_CPP23
//! @brief Checks whether C++23 or a newer version is being used
//! @return Returns the result of the version check
//! @retval 1 if C++23 or a newer version is being used
//! @retval 0 if C++23 or a later version was not used

//! @def HAS_C11
//! @brief Checks whether C11 or a newer version is being used
//! @return Returns the result of the version check
//! @retval 1 if C11 or a newer version is being used
//! @retval 0 if C11 or a later version was not used

#ifdef __cplusplus
  #define HAS_CPP23 ( 202302L <= __cplusplus )
  #define HAS_C11   0
#else
  #define HAS_CPP23 0
  #ifdef __STDC_VERSION__
    #define HAS_C11 ( 201112L <= __STDC_VERSION__ )
  #else
    #define HAS_C11 0
  #endif
#endif

#if !(HAS_CPP23 || HAS_C11)
  #error "Requires C11 or C++23"
#endif

//! @}


//! @defgroup stdatomic_support_in_cpp The stdatomic header is
//!           supported in C and C++
//!
//! @details With C++23, the header is compatible between C and C++.
//!
//! @{

#include <stdatomic.h>

//! @}


//! @defgroup cdt_parser_workaround Workaround for CDT indexer
//!
//! @details The CDT indexer has problems with the atomic functions.
//!          For this reason `_Atomic(T)` is redefined. Without changing the
//!          reserved keyword, the CDT indexer or auto-completion will not find
//!          the variable. A disadvantage is that no information is displayed
//!          about the fact that it is an atomic type.
//!
//! @{

#ifdef __CDT_PARSER__

  #ifndef __cplusplus

    #define _Atomic(...) __VA_ARGS__

    #define atomic_init(PTR, VAL) (*(PTR) = (VAL)) //;

    #define atomic_store(PTR, VAL) (*(PTR) = (VAL)) //;

    #define atomic_load(PTR) (*(PTR)) //;

    #define atomic_compare_exchange_strong(PTR, VAL, DES) \
      ((*(VAL) == *(PTR)) ? (*(PTR) = (DES), 1) : 0) //;

    #define _Static_assert(CONDITION, TEXT) //;

  #endif

#endif

//! @}


//! @defgroup atomic_var_init_check Support for use in C and C++ after deprecation
//!
//! @details In C and in C++ `ATOMIC_VAR_INIT(value)` was deprecated.
//!          It must be defined so that it is supported in newer versions.
//!
//! @{

//! @brief Fallback definition for atomic variable initialization.
//!
//! @details Defines ATOMIC_VAR_INIT as a direct value assignment if not
//!          already provided by the standard library or platform.
//!          This is a compatibility fallback and does not provide atomic
//!          semantics by itself.
//!
//! @return The value X unchanged.
#ifndef ATOMIC_VAR_INIT
#  define ATOMIC_VAR_INIT(VALUE) (VALUE)
#endif

//! @}


#ifdef __cplusplus
extern "C" {
#endif


// ------------------------------------------------------------------------- //
//  public: include files
// ------------------------------------------------------------------------- //

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


// ------------------------------------------------------------------------- //
//  public: define
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: typedefs
// ------------------------------------------------------------------------- //

//! @brief Status of the individual item
//! @details Each item has a status. This status is used to handle the item differently in specific methods.
typedef enum event_lf_state_flag_e
{
  EVENT_LF_STATE_FLAG_UNKNOWN = 0x00, //!< State of the item is unknown
  EVENT_LF_STATE_FLAG_IS_ALIVE = 0x01, //!< If the item is alive, it is not soft deleted. A new item and the last item are only alive.
  EVENT_LF_STATE_FLAG_IS_VALID = 0x02, //!< If the item is valid, it can be processed; invoke and sub needs alive and valid
} event_lf_state_flag_t;

//! @brief This is the type of handler called by the entire `event_lf` module
//! @param[in,out] sender Usually a pointer provided by the sender, but it can be chosen freely
//! @param[in,out] e Usually a pointer to the event arguments, but it can be chosen freely
typedef void (*function_pointer)(void * sender, void * e);


// Forward declaration
struct event_lf_item_s;

// Forward typedef, for information see ::event_lf_item_s
typedef struct event_lf_item_s event_lf_item_t;

//! @brief A structure for managing memory blocks
typedef struct event_lf_memory_s
{
  _Atomic(event_lf_item_t *) next; //!< Pointer to the next item or null
  _Atomic(uint16_t) used; //!< Number of used items
  _Atomic(uint16_t) capacity; //!< Maximum number of available items
} event_lf_memory_t;


//! @brief A structure for managing an event item
typedef struct event_lf_item_s
{
  _Atomic(event_lf_item_t *) next; //!< Pointer to the next item or null
  function_pointer function; //!< Function pointer, must not be null
  _Atomic(event_lf_state_flag_t) state; //!< State of the item

  event_lf_item_t * remove; //!< Pointer to the next removed item or null
} event_lf_item_t;

//! @brief A structure for managing events
typedef struct event_lf_s
{
  volatile _Atomic(event_lf_item_t *) list; //!< Pointer to the first item
  volatile _Atomic(uint16_t) threads; //!< Number of running threads

  volatile _Atomic(uint16_t) remove_count; //!< Number of items to be added to the "removed" chain
  event_lf_item_t * remove; //!< A sequence of elements that need to be removed

  void * memory_object; //!< Function pointer to manage memory, can be null
  event_lf_item_t * (*allocate)(void * memory_object); //!< Function pointer to allocate memory, must not be null
  void (*free)(void * memory_object, event_lf_item_t * mem); //!< Function pointer to free memory, must not be null
} event_lf_t;


//! @brief Represents a simplified form of a class
//!
//! @details The global variable ::event_lf_memory can be used to easily
//!          access all matching functions with auto-completion.
struct event_lf_memory_sc
{
  //! @brief Adds memory to the memory object
  //! @param[in,out] object The memory object itself
  //! @param[in,out] data Pointer to the data store to be added
  //! @param countof_data Number of data array elements
  void (* Add) (event_lf_memory_t * object, event_lf_item_t * const data, size_t countof_data);

  //! @brief Function to allocate an item
  //! @param[in,out] object The memory object itself
  //! @return A pointer to an available event item, or null if there are no more available objects
  event_lf_item_t * (* Allocate) (event_lf_memory_t * object);

  //! @brief Function to free an item
  //! @param[in,out] object The memory object itself
  //! @param item The element to be freed
  void (* Free) (event_lf_memory_t * object, event_lf_item_t * item);

  //! @brief Initialize the struct `event_lf_memory_s`
  //! @param[in,out] object The memory object itself
  void (* Init) (event_lf_memory_t * object);
};

//! @brief Represents a simplified form of a class
//!
//! @details The global variable ::event_lf can be used to easily access
//!          all matching functions with auto-completion.
struct event_lf_sc
{
  //! @brief This function allows you to register a new function for the event
  //! @param[in,out] object The event object itself
  //! @param[in] function Function pointer to the function to be added
  //! @return Returns if the function could be added
  //! @retval true The function could be added
  //! @retval false The function could not be added
  bool (* Add) (event_lf_t * const object, function_pointer const function);

  //! @brief This function returns the number of valid handlers that have been added
  //! @param[in,out] object The event object itself
  //! @return Number of valid items
  uint16_t (* Count) (event_lf_t * const object);

  //! @brief Dispose all items of the event
  //! @param[in,out] object The event object itself
  //! @return Returns if all is disposed
  //! @retval true All items are disposed
  //! @retval false Not all items are disposed
  bool (* Dispose) (event_lf_t * const object);

  //! @brief Calls the specified @p function with the specified @p sender for all valid elements
  //! @param[in,out] object The event object itself
  //! @param[in,out] sender Usually a pointer provided by the sender, but it can be chosen freely
  //! @param function The function that is called
  void (* Foreach) (event_lf_t * const object, void * sender, void (*function)(void * sender, event_lf_item_t * item) );

  //! @brief Initialize the struct `event_lf_s` with a given memory object
  //! @param[in,out] object The event object itself
  //! @param[in] memory_object The memory object for managing memory
  void (* Init) (event_lf_t * const object, event_lf_memory_t * const memory_object);

  //! @brief Initialize the struct `event_lf_s` with a given memory object and allocate and free functions
  //! @param[in,out] object The event object itself
  //! @param[in] memory_object The memory object for managing memory, if not used it can be null
  //! @param[in] allocate The callback function to allocate memory
  //! @param[in] free The callback function to free memory
  void (* InitCustomMemory) (
    event_lf_t * const object,
    void * const memory_object,
    event_lf_item_t * (* allocate)(void * memory_object),
    void (* free)(void * memory_object, event_lf_item_t * mem));

  //! @brief The function can be used to call all functions registered for the event
  //! @param[in,out] object The event object itself
  //! @param[in,out] sender Usually a pointer provided by the sender, but it can be chosen freely
  //! @param[in,out] e Usually a pointer to the event arguments, but it can be chosen freely
  void (* Invoke) (event_lf_t * const object, void * const sender, void * const e);

  //! @brief This function allows you to remove a new function for the event
  //! @param[in,out] object The event object itself
  //! @param[in] function Function pointer to the function to be removed
  void (* Sub) (event_lf_t * const object, function_pointer const function);
};


// ------------------------------------------------------------------------- //
//  public: function prototypes
// ------------------------------------------------------------------------- //

//! @brief Checks at runtime whether selected atomic types are lock-free.
//!
//! @details This function verifies that all atomic types used in the part
//!          (`event_lf_item_t *`, `event_lf_state_flag_t`, and `uint16_t`)
//!          are lock-free on the current platform.
//!
//! @return If all tested atomic types are lock-free
//! @retval true If all tested atomic types are lock-free
//! @retval false If not all tested atomic types are lock-free
bool check_lock_free_runtime(void);


//! @brief Adds memory to the memory object
//! @param[in,out] object The memory object itself
//! @param[in,out] data Pointer to the data store to be added
//! @param countof_data Number of data array elements
void event_lf_memory_add(event_lf_memory_t * object, event_lf_item_t * const data, size_t countof_data);

//! @brief Function to allocate an item
//! @param[in,out] object The memory object itself
//! @return A pointer to an available event item, or null if there are no more available objects
event_lf_item_t * event_lf_memory_allocate(event_lf_memory_t * object);

//! @brief Function to free an item
//! @param[in,out] object The memory object itself
//! @param item The element to be freed
void event_lf_memory_free(event_lf_memory_t * object, event_lf_item_t * item);

//! @brief Initialize the struct `event_lf_memory_s`
//! @param[in,out] object The memory object itself
void event_lf_memory_init(event_lf_memory_t * object);



#ifdef EVENT_LF_ALLOW_STANDARD_MALLOC_FREE

//! @brief Function to allocate an item
//! @param[in,out] object Not used
//! @return A pointer to an available event item, or null if there are no more available objects
event_lf_item_t * event_lf_standard_malloc(void * object);

//! @brief Function to free an item allocated with `malloc`.
//! @param[in,out] object Not used
//! @param item The element to be freed
void event_lf_standard_free(void * object, event_lf_item_t * item);


#endif



//! @brief This function allows you to register a new function for the event
//! @param[in,out] object The event object itself
//! @param[in] function Function pointer to the function to be added
//! @return Returns if the function could be added
//! @retval true The function could be added
//! @retval false The function could not be added
bool event_lf_add(event_lf_t * const object, function_pointer const function);

//! @brief This function returns the number of valid handlers that have been added
//! @param[in,out] object The event object itself
//! @return Number of valid items
uint16_t event_lf_count(event_lf_t * const object);

//! @brief Dispose all items of the event
//! @param[in,out] object The event object itself
//! @return Returns if all is disposed
//! @retval true All items are disposed
//! @retval false Not all items are disposed
bool event_lf_dispose(event_lf_t * const object);

//! @brief Calls the specified @p function with the specified @p sender for all valid elements
//! @param[in,out] object The event object itself
//! @param[in,out] sender Usually a pointer provided by the sender, but it can be chosen freely
//! @param function The function that is called
void event_lf_foreach(event_lf_t * const object, void * sender, void (*function)(void * sender, event_lf_item_t * item) );

//! @brief Initialize the struct `event_lf_s` with a given memory object
//! @param[in,out] object The event object itself
//! @param[in] memory_object The memory object for managing memory
void event_lf_init(event_lf_t * const object, event_lf_memory_t * const memory_object);

//! @brief Initialize the struct `event_lf_s` with a given memory object and allocate and free functions
//! @param[in,out] object The event object itself
//! @param[in] memory_object The memory object for managing memory, if not used it can be null
//! @param[in] allocate The callback function to allocate memory
//! @param[in] free The callback function to free memory
void event_lf_init_custom_memory(
  event_lf_t * const object,
  void * const memory_object,
  event_lf_item_t * (* allocate)(void * memory_object),
  void (* free)(void * memory_object, event_lf_item_t * mem));

//! @brief The function can be used to call all functions registered for the event
//! @param[in,out] object The event object itself
//! @param[in,out] sender Usually a pointer provided by the sender, but it can be chosen freely
//! @param[in,out] e Usually a pointer to the event arguments, but it can be chosen freely
void event_lf_invoke(event_lf_t * const object, void * const sender, void * const e);

//! @brief This function allows you to remove a new function for the event
//! @param[in,out] object The event object itself
//! @param[in] function Function pointer to the function to be removed
void event_lf_sub(event_lf_t * const object, function_pointer const function);


// ------------------------------------------------------------------------- //
//  public: extern variables
// ------------------------------------------------------------------------- //

//! @brief To access all member functions working with type ::event_lf_memory_s
//!
//! @details Allows a simplified class to easily access all member functions
//!          working with type ::event_lf_memory_s. The auto-completion
//!          function helps you to select all suitable functions via
//!          ::event_lf_memory_sc struct.
//!
extern const struct event_lf_memory_sc event_lf_memory;

//! @brief To access all member functions working with type ::event_lf_s
//!
//! @details Allows a simplified class to easily access all member functions
//!          working with type ::event_lf_s. The auto-completion function
//!          helps you to select all suitable functions via ::event_lf_sc
//!          struct.
//!
extern const struct event_lf_sc event_lf;


// ------------------------------------------------------------------------- //
//  public: macros like functions
// ------------------------------------------------------------------------- //

//! @brief Get the number of elements in a static array.
//!
//! @details Calculates the total number of elements in an array
//!
//! @note Only works with actual arrays, not pointers.
//!
//! @param ARRAY The array whose element count is to be determined.
//! @return Number of elements in the array.
#ifndef countof
#define countof(ARRAY) (sizeof(ARRAY)/sizeof(*(ARRAY)))
#endif

//! @brief Get the size of a single element in an array.
//!
//! @details Expands to the size (in bytes) of one element of the given array.
//!
//! @note Only works with actual arrays, not pointers.
//!
//! @param ARRAY The array whose element size is to be determined.
//! @return Size of one array element in bytes.
#ifndef itemsizeof
#define itemsizeof(ARRAY) (sizeof(*(ARRAY)))
#endif


//! @brief Initialize the struct `event_lf_memory_s`
#define EVENT_LF_MEMORY_INIT() \
{                                          \
  /* .next     = */ ATOMIC_VAR_INIT(NULL), \
  /* .used     = */ ATOMIC_VAR_INIT(0),    \
  /* .capacity = */ ATOMIC_VAR_INIT(0)     \
}                                         // ;

//! @brief Initialize the struct `event_lf_s` with a given memory object.
//!
//! @details This macro provides a static initializer for an event instance.
//!          It sets up atomic members, internal pointers, and memory
//!          management callbacks required for event handling.
//!
//! @param MEMORY_OBJECT_PTR Pointer to a user-provided memory/context object
//!                          used by the allocation and free callbacks.
//!
#define EVENT_LF_INIT(MEMORY_OBJECT_PTR) \
((event_lf_t){                                                                    \
  /* .list          = */ ATOMIC_VAR_INIT(NULL),                                   \
  /* .threads       = */ ATOMIC_VAR_INIT(0),                                      \
  /* .remove_count  = */ ATOMIC_VAR_INIT(0),                                      \
  /* .remove        = */ NULL,                                                    \
  /* .memory_object = */ (MEMORY_OBJECT_PTR),                                     \
  /* .allocate      = */ (event_lf_item_t *(*)(void *))event_lf_memory_allocate,  \
  /* .free          = */ (void(*)(void *, event_lf_item_t *))event_lf_memory_free \
})                                                                               // ;

//! @brief Initialize an event struct `event_lf_s` with a given memory object.
//!
//! @details This macro provides a static initializer for an event instance.
//!          It sets up atomic members, internal pointers, and memory
//!          management callbacks required for event handling.
//!
//! @param MEMORY_OBJECT_PTR Pointer to a user-provided memory/context object
//!                          used by the allocation and free callbacks.
//! @param ALLOCATE Function pointer to the allocation function.
//! @param FREE Function pointer to the free function.
//!
#define EVENT_LF_INIT_CUSTOM_MEMORY(MEMORY_OBJECT_PTR, ALLOCATE, FREE) \
((event_lf_t){                                  \
  /* .list          = */ ATOMIC_VAR_INIT(NULL), \
  /* .threads       = */ ATOMIC_VAR_INIT(0),    \
  /* .remove_count  = */ ATOMIC_VAR_INIT(0),    \
  /* .remove        = */ NULL,                  \
  /* .memory_object = */ (MEMORY_OBJECT_PTR),   \
  /* .allocate      = */ (ALLOCATE),            \
  /* .free          = */ (FREE)                 \
})                                             // ;


#ifdef EVENT_LF_ALLOW_STANDARD_MALLOC_FREE

//! @brief Initialize an event struct `event_lf_s` with standard malloc and free function
//!
//! @details This macro provides a static initializer for an event instance.
//!          It sets up atomic members, internal pointers, and memory
//!          management callbacks required for event handling.
//!
#define EVENT_LF_INIT_STANDARD_MALLOC_FREE() \
((event_lf_t){                                     \
  /* .list          = */ ATOMIC_VAR_INIT(NULL),    \
  /* .threads       = */ ATOMIC_VAR_INIT(0),       \
  /* .remove_count  = */ ATOMIC_VAR_INIT(0),       \
  /* .remove        = */ NULL,                     \
  /* .memory_object = */ NULL,                     \
  /* .allocate      = */ event_lf_standard_malloc, \
  /* .free          = */ event_lf_standard_free    \
})                                                // ;

#endif


// ------------------------------------------------------------------------- //
//  public: static inline functions
// ------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif // INC_EVENT_LF_H


// ------------------------------------------------------------------------- //
//  eof
// ------------------------------------------------------------------------- //
