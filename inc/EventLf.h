//! @file
//! @brief The EventLf.h header file.
//!
//! @details The module can be used in C and C++.


#ifndef INC_EVENTLF_H
#define INC_EVENTLF_H


// ------------------------------------------------------------------------- //
//  public: atomic handling
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: include files
// ------------------------------------------------------------------------- //

#include <fstream>

#include "event_lf.h"


// ------------------------------------------------------------------------- //
//  public: define
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: typedefs
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: function prototypes
// ------------------------------------------------------------------------- //

//! @brief To access all member functions working with type ::event_lf_memory_s
//!
//! @details Allows easy access to all member functions working with type
//!          ::event_lf_memory_s. The auto-completion function helps to
//!          select all suitable functions.
//!
class EventLfMemory
{

private:
  //! @brief The underlying C structure of the memory object
  event_lf_memory_t objectData;

  //! @brief Initialize the struct `event_lf_memory_s`
  void Init ();

public:
  //! @brief A pointer to the `event_lf_memory_t` memory object, so it can be used with C functions
  event_lf_memory_t * c_object;

  //! @brief Initializes the class without adding allocated memory
  //! @details The `Add()` method must be used to append data
  EventLfMemory ();

  //! @brief Initializes the class and adds the allocated memory
  //! @param[in,out] data Pointer to the data store to be added
  //! @param countof_data Number of data array elements
  EventLfMemory (event_lf_item_t * const data, size_t countof_data);

  //! @brief Initializes the class and adds the allocated memory
  //! @param data Storage space to be added
  EventLfMemory (std::span<event_lf_item_t> data);

  //! @brief Adds memory to the memory object
  //! @param[in,out] data Pointer to the data store to be added
  //! @param countof_data Number of data array elements
  void Add (event_lf_item_t * const data, size_t countof_data);

  //! @brief Adds memory to the memory object
  //! @param data Storage space to be added
  void Add (std::span<event_lf_item_t> data);

  //! @brief Function to allocate an item
  //! @return A pointer to an available event item, or null if there are no more available objects
  event_lf_item_t * Allocate ();

  //! @brief Function to free an item
  //! @param item The element to be freed
  void Free (event_lf_item_t * item);

  //! @brief Returns the number of elements used
  //! @return Number of used elements
  uint16_t getUsed ();

  //! @brief Returns the maximum number of elements that can be used (capacity)
  //! @return Number of total capacity
  uint16_t getCapacity ();

};



//! @brief To access all member functions working with type ::event_lf_s
//!
//! @details Allows to easy access all member functions working with type
//!          ::event_lf_s. The auto-completion function helps you to
//!          select all suitable functions.
//!
class EventLf
{

public:
  //! @brief Checks at runtime whether selected atomic types are lock-free.
  //!
  //! @details This function verifies that all atomic types used in the part
  //!          (`event_lf_item_t *`, `event_lf_state_flag_t`, and `uint16_t`)
  //!          are lock-free on the current platform.
  //!
  //! @return If all tested atomic types are lock-free
  //! @retval true If all tested atomic types are lock-free
  //! @retval false If not all tested atomic types are lock-free
  static bool CheckLockFreeRuntime ();


private:
  //! @brief The underlying C structure of the memory object
  event_lf_t objectData;

  //! @brief Initialize the struct `event_lf_s` with a given memory object
  //! @param[in] memory_object The memory object for managing memory
  void Init (event_lf_memory_t * const memory_object);


public:
  //! @brief A pointer to the `event_lf_t` memory object, so it can be used with C functions
  event_lf_t * c_object;

  //! @brief Initializes the class with the given memory object
  //! @param[in] memory_object The memory object for managing memory
  EventLf (event_lf_memory_t * const memory_object);

  //! @brief Initializes the class with the given memory class
  //! @param memory_class The memory object for managing memory
  EventLf (EventLfMemory & memory_class);

  //! @brief Dispose all items of the event
  ~EventLf ();

  //! @brief This function allows you to register a new function for the event
  //! @param[in] function Function pointer to the function to be added
  //! @return Returns if the function could be added
  //! @retval true The function could be added
  //! @retval false The function could not be added
  bool Add (function_pointer const function);

  //! @brief This function returns the number of valid handlers that have been added
  //! @return Number of valid items
  uint16_t Count ();

  //! @brief Dispose all items of the event
  //! @return Returns if all is disposed
  //! @retval true All items are disposed
  //! @retval false Not all items are disposed
  bool Dispose ();

  //! @brief Calls the specified @p function with the specified @p sender for all valid elements
  //! @param[in,out] sender Usually a pointer provided by the sender, but it can be chosen freely
  //! @param function The function that is called
  void Foreach (void * sender, void (*function)(void * sender, event_lf_item_t * item) );

  //! @brief Initialize the struct `event_lf_s` with a given memory object and allocate and free functions
  //! @param[in] memory_object The memory object for managing memory, if not used it can be null
  //! @param[in] allocate The callback function to allocate memory
  //! @param[in] free The callback function to free memory
  void InitCustomMemory (
    void * const memory_object,
    event_lf_item_t * (* allocate)(void * memory_object),
    void (* free)(void * memory_object, event_lf_item_t * mem));

  //! @brief The function can be used to call all functions registered for the event
  //! @param[in,out] sender Usually a pointer provided by the sender, but it can be chosen freely
  //! @param[in,out] e Usually a pointer to the event arguments, but it can be chosen freely
  void Invoke (void * const sender, void * const e);

  //! @brief This function allows you to remove a new function for the event
  //! @param[in] function Function pointer to the function to be removed
  void Sub (function_pointer const function);
};


// ------------------------------------------------------------------------- //
//  public: extern variables
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: macros like functions
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: static inline functions
// ------------------------------------------------------------------------- //


#endif // INC_EVENTLF_H


// ------------------------------------------------------------------------- //
//  eof
// ------------------------------------------------------------------------- //
