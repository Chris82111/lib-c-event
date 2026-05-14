# lib-c-event

This library enables the use of events in C/C++. The logic is implemented in a
lock-free manner, this allows the event to be used in a single thread, in a multithreaded environment, and during interrupts.
The functions add, sub, invoke, and all others can be called from any number
of threads and interrupts. 
The underlying memory is allocated in advance and then managed internally.
This memory can be used by any number of events. The `malloc` and `free`
functions are not required. However, the logic is implemented using function
pointers, allowing for custom memory management and thus enabling the use of
malloc and free. The included memory management is lock-free; other
implementations must have the same requirements so that the library maintains
this property overall.
The underlying logic is implemented in C. Usage in C++ is handled via wrapper
functions that call the C functions. This allows parts to be implemented and
switched seamlessly between C and C++.

Every event requires an additional component at the end. When allocating storage space, this number must be taken into account and provided for.

## Examples

The figure shows the general steps of the examples.  

<picture>
  <source
    media="(prefers-color-scheme: dark)"
    srcset="./docs/event_dark.svg" />
  <img
    alt=""
    src="./docs/event.svg"
    width="500" />
</picture>

The examples show how easy it is to add, call, and remove an item to the event. 

### Example C

```c
void event_lf_example1(void)
{
  event_lf_item_t data10[10];

  event_lf_memory_t memory = EVENT_LF_MEMORY_INIT();
  event_lf_memory.Add(&memory, data10, countof(data10));

  event_lf_t e1 = EVENT_LF_INIT(&memory);
  event_lf.Init(&e1, &memory);
  event_lf.Add(&e1, NULL); // Optional adds a trailing item

  event_lf.Add(&e1, handler1);
  event_lf.Add(&e1, handler2);
  event_lf.Add(&e1, handler3);
  event_lf.Invoke(&e1, NULL, NULL); // 1, 2, 3

  event_lf.Sub(&e1, handler2);
  event_lf.Invoke(&e1, NULL, NULL); // 1, 3

  event_lf.Dispose(&e1);
}
```

### Example C++

```cpp
void EventLf_example1(void)
{
  std::array<event_lf_item_t, 10> data10;

  auto memory = EventLfMemory(data10);
  
  auto e1 = EventLf(memory);

  e1 += handler1;
  e1 += handler2;
  e1 += handler3;
  e1.Invoke(NULL, NULL); // 1, 2, 3

  e1 -= handler2;
  e1.Invoke(NULL, NULL); // 1, 3
}
```

More examples can be found in the [more_examples.md](./docs/more_examples.md) file.
