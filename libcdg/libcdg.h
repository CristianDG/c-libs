#ifndef LIBCDG_H
#define LIBCDG_H

// types.h {{{

// wip: context cracking {{{


#if defined(__clang__) // compiler switch

# define DG_COMPILER_CLANG 1

# if defined(_WIN32)
#  define DG_OS_WINDOWS 1
# elif defined(__gnu_linux__) || defined(__linux__)
#  define DG_OS_LINUX 1
# else
#  error "This Compiler/OS combination is not supported"
# endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define DG_ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define DG_ARCH_X86 1
# elif defined(__aarch64__)
#  define DG_ARCH_ARM64 1
# elif defined(__arm__)
#  define DG_ARCH_ARM32 1
# else
#  error "Architecture not supported."
# endif

#elif defined(__GNUC__) || defined(__GNUG__) // compiler switch

# define DG_COMPILER_GCC 0

# if defined(__gnu_linux__) || defined(__linux__)
#  define DG_OS_LINUX 1
# else
#  error "This Compiler/OS combination is not supported"
# endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define DG_ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define DG_ARCH_X86 1
# elif defined(__aarch64__)
#  define DG_ARCH_ARM64 1
# elif defined(__arm__)
#  define DG_ARCH_ARM32 1
# else
#  error "Architecture not supported."
# endif

#else // compiler switch
# error "Compiler not supported."
#endif // compiler switch

#if defined(DG_ARCH_X64)
# define DG_ARCH_64BIT 1
#elif defined(DG_ARCH_X86)
# define DG_ARCH_32BIT 1
#endif

#if DG_ARCH_ARM32 || DG_ARCH_ARM64 || DG_ARCH_X64 || DG_ARCH_X86
# define ARCH_LITTLE_ENDIAN 1
#else
# error "Endianness of this architecture not understood by context cracker."
#endif

#if defined(__cplusplus)
# define DG_LANG_CPP 1
#else
# define DG_LANG_C 1
#endif

// clear undefined variables
#if !defined(DG_ARCH_32BIT)
# define DG_ARCH_32BIT 0
#endif
#if !defined(DG_ARCH_64BIT)
# define DG_ARCH_64BIT 0
#endif
#if !defined(DG_ARCH_X64)
# define DG_ARCH_X64 0
#endif
#if !defined(DG_ARCH_X86)
# define DG_ARCH_X86 0
#endif
#if !defined(DG_ARCH_ARM64)
# define DG_ARCH_ARM64 0
#endif
#if !defined(DG_ARCH_ARM32)
# define DG_ARCH_ARM32 0
#endif
#if !defined(DG_COMPILER_MSVC)
# define DG_COMPILER_MSVC 0
#endif
#if !defined(DG_COMPILER_GCC)
# define DG_COMPILER_GCC 0
#endif
#if !defined(DG_COMPILER_CLANG)
# define DG_COMPILER_CLANG 0
#endif
#if !defined(DG_OS_WINDOWS)
# define DG_OS_WINDOWS 0
#endif
#if !defined(DG_OS_LINUX)
# define DG_OS_LINUX 0
#endif
#if !defined(DG_OS_MAC)
# define DG_OS_MAC 0
#endif
#if !defined(DG_LANG_CPP)
# define DG_LANG_CPP 0
#endif
#if !defined(DG_LANG_C)
# define DG_LANG_C 0
#endif

// }}}


#if __STDC_VERSION__ < 199901L /* older than c99 */
# ifndef inline
#  define inline
# endif
#endif

#if DG_COMPILER_MSVC
# define thread_static __declspec(thread)
#elif DG_COMPILER_CLANG || DG_COMPILER_GCC
# define thread_static __thread
#endif


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* TODO:
 * - context cracking
 * - defer macro
 */

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef uintptr_t uintptr;
typedef intptr_t intptr;

typedef size_t usize;
typedef ptrdiff_t isize;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float  f32;
typedef double f64;

typedef u32 b32;

#define global_variable static
#define local_persist static
#define internal static

#define DG_STATEMENT(x) do { x } while (0)

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define STR(x) #x
#define GLUE_IMPL(a,b) a##b
#define GLUE(a,b) GLUE_IMPL(a,b)
#define GLUE_2(a,b) GLUE(a,b)
#define GLUE_3(a,b,c) GLUE(GLUE(a,b), c)
#define GLUE_4(a,b,c,d) GLUE(GLUE_3(a,b,c), d)

#define ELEVENTH_ARGUMENT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define DG_NARGS(...) ELEVENTH_ARGUMENT(dummy, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define DG_BIT_MASK_REMOVE(mask, flag)  ((mask)  & ~(flag))
#define DG_BIT_MASK_ADD(mask, flag)     ((mask)  |  (flag))
#define DG_BIT_MASK_HAS(mask, flag)     (((mask) & (flag)) == (flag))

#if !defined(DG_STATIC_ASSERT) // {{{

#define DG_STATIC_ASSERT(...) DG_STATIC_ASSERT_IMPL(DG_NARGS(__VA_ARGS__), __VA_ARGS__)
#define DG_STATIC_ASSERT_IMPL(n, ...) GLUE(DG_STATIC_ASSERT_, n)(__VA_ARGS__)
#define DG_STATIC_ASSERT_1(expr) _Static_assert(expr, "")
#define DG_STATIC_ASSERT_2(expr, msg) _Static_assert(expr, msg)

#endif // }}} DG_STATIC_ASSERT

// NOTE: vou deixar sem as parenteses para ver se não tenho erros básicos de macros
#define ABS(a)   (a > 0 ? a : -a)
#define MAX(a,b) (a > b ? a :  b)
#define MIN(a,b) (a < b ? a :  b)
#define CLAMP(val, min, max) (val < min ? min : (val > max ? max : val))

#define CLAMP_TOP MIN
#define CLAMP_BOTTOM MAX

#define KILOBYTE 1024
#define MEGABYTE 1048576L

#define DG_REINTERPRET_CAST(type, val) \
  (*((type *)&(val)))

#define DG_OFFSET_OF(type, field) \
  ((uintptr)&(((type *) 0)->field))

#define DG_DYNAMIC_ACCESS(type, offset) \
  (((void *)(type))+offset)

#define DG_SWAP(type, a, b) ({ \
  type _tmp = *(a); \
  *(a) = *(b); \
  *(b) = _tmp; \
  (void)0; \
})

#define DG_BREAKPOINT_IMPL(line) \
  volatile usize GLUE(_dg_breakpoint_val_, line) = 0; GLUE(_dg_breakpoint_val_, line) += 0;

#define DG_BREAKPOINT DG_BREAKPOINT_IMPL(__LINE__)

#ifndef DG_LOG_ERROR
# include <stdio.h>
# define DG_LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)
#endif // DG_LOG_ERROR

#ifndef DG_LOG
# include <stdio.h>
# define DG_LOG(...) fprintf(stdout, __VA_ARGS__)
#endif // DG_LOG

#ifndef DG_CRASH // {{{
#if defined(DG_PLATFORM_WASM)
#define DG_CRASH() __builtin_trap()
#else
#define DG_CRASH() (*((volatile int *)0) = 69)
#endif
#endif // }}} DG_CRASH

#if !defined(DG_ASSERT_EXPR) // {{{
// NOTE: esse assert funciona como expressão: bool assert(bool)
#define DG_ASSERT_EXPR(exp) ( \
  (exp) \
    ? (true) \
    : (DG_LOG_ERROR("%s:%d : assertion '%s' failed\n", __FILE__, __LINE__, STR(exp)), DG_CRASH(), false) \
)
#endif // DG_ASSERT_EXPR }}}

#if defined(DG_NO_ASSERT)
# define DG_ASSERT_MSG_PASS_LOC(exp, msg, file, line)
# define DG_ASSERT_MSG(exp, msg)
# define DG_ASSERT(exp)
#else
# if !defined(DG_ASSERT_MSG_PASS_LOC) // {{{
#  define DG_ASSERT_MSG_PASS_LOC(exp, msg, file, line) \
   DG_STATEMENT({ \
     if ((exp) == false) { \
       DG_LOG_ERROR("%s:%d: assertion '%s' failed\n", file, line, msg); \
       DG_CRASH(); \
     } \
   })
#  define DG_ASSERT_MSG(exp, msg) DG_ASSERT_MSG_PASS_LOC(exp, msg, __FILE__, __LINE__)
#  define DG_ASSERT(exp) DG_ASSERT_MSG(exp, STR(exp))
# endif // }}}
#endif



#define DG_WIP \
  DG_ASSERT_MSG(false, "not implemented")

// }}}

// alloc.c {{{


#include <stddef.h>
// #include <string.h> // for memset

#define DG_DEFAULT_ALIGNMENT 16

// TODO: WIP
// typedef struct {
// } Growing_Arena;

#ifndef DG_MEMCPY
void *memcpy(void *dst, const void *src, usize size);
#include <string.h>
#define DG_MEMCPY memcpy
#endif

#ifndef DG_MEMSET
#include <string.h>
void *memset(void *dst, int val, usize size);
#define DG_MEMSET memset
#endif

#ifndef DG_REALLOC
#include <stdlib.h>
void *realloc(void *ptr, usize new_size);
#define DG_REALLOC realloc
#endif

#ifndef DG_MALLOC
#include <stdlib.h>
void *malloc(usize size);
#define DG_MALLOC malloc
#endif

#ifndef DG_FREE
#include <stdlib.h>
void free(void *ptr);
#define DG_FREE free
#endif

#ifndef DG_MEMZERO
#define DG_MEMZERO(ptr) DG_MEMSET(ptr, DG_MEMZERO_ZERO, sizeof *(ptr))
#endif

typedef struct {
  void *data;
  // TODO: add temp_count
  u32 size;
  u32 cursor;
  void *last_allocation;
} DG_Arena;

typedef struct {
  DG_Arena *arena;
  u32 cursor;
  void *last_allocation;
} DG_Temp_Arena;


DG_Arena *dg_arena_init(void);
DG_Arena *dg_arena_init_buffer(void *data, size_t size);

void *dg_arena_alloc_impl(DG_Arena *arena, size_t size, size_t alignment);
void *dg_tracking_arena_alloc_impl(DG_Arena *arena, size_t size, size_t alignment, char *file, i32 line);

DG_Temp_Arena dg_temp_arena_begin(DG_Arena *a);
void dg_temp_arena_end(DG_Temp_Arena tmp_mem);

void dg_arena_clear(DG_Arena *arena);

void *dg_arena_realloc_impl(DG_Arena *arena, void *ptr, size_t size, size_t alignment);

#if defined(DG_ARENA_DEBUG)
# define dg_arena_alloc(arena, size) dg_tracking_arena_alloc_impl(arena, size, DG_DEFAULT_ALIGNMENT, __FILE__, __LINE__)
# define dg_arena_alloc_pass_loc(arena, size, file, line) dg_tracking_arena_alloc_impl(arena, size, DG_DEFAULT_ALIGNMENT, file, line)
#else /* !defined(DG_ARENA_DEBUG) */
# define dg_arena_alloc(arena, size) dg_arena_alloc_impl(arena, size, DG_DEFAULT_ALIGNMENT)
# define dg_arena_alloc_pass_loc(arena, size, file, line) dg_arena_alloc_impl(arena, size, DG_DEFAULT_ALIGNMENT)
#endif /* !defined(DG_ARENA_DEBUG) */

#define DG_Temp_Guard(arena) \
  for (DG_Temp_Arena GLUE(_dg_tam_, __LINE__) = dg_temp_arena_begin(arena) \
  ; GLUE(_dg_tam_, __LINE__).arena \
  ; dg_temp_arena_end(GLUE(_dg_tam_, __LINE__)))

void dg_scratch_memory_init(void);
void dg_scratch_memory_init_buffer(u8 *data, usize size);

DG_Temp_Arena dg_scratch_get(DG_Arena *conflict);
#define dg_scratch_release(scratch_arena) dg_temp_arena_end(scratch_arena)


// }}}
// containers {{{


//
// dynamic arrays
//

// NOTE: implementation from https://nullprogram.com/blog/2023/10/05/

#define DG_Make_Dynamic_Array_Type(type) \
struct { \
  i32 cap; \
  i32 len; \
  type *data; \
}

// typedef void _DG_Any_Dynamic_Array;
typedef DG_Make_Dynamic_Array_Type(void) _DG_Any_Dynamic_Array;

void dg_dynamic_array_make_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, u32 capacity, u32 item_size);
#define dg_dynamic_array_make(arena, arr, capacity) dg_dynamic_array_make_impl(arena, (_DG_Any_Dynamic_Array *) arr, capacity, sizeof(*(arr)->data))

void dg_dynamic_array_pop_impl(_DG_Any_Dynamic_Array *arr, void *dst, u32 item_size);
#define dg_dynamic_array_pop_discard(arr) dg_dynamic_array_pop_impl((_DG_Any_Dynamic_Array *) arr, NULL, 0)
#define dg_dynamic_array_pop_size(arr, item, size) dg_dynamic_array_pop_impl((_DG_Any_Dynamic_Array *) arr, (void *) &(item), size)
#define dg_dynamic_array_pop(arr, item) dg_dynamic_array_pop_size(arr, item, sizeof(*(arr)->data))

bool dg_dynamic_array_try_push_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size);
#define dg_dynamic_array_try_push_size(arr, item, size) dg_dynamic_array_try_push_impl((_DG_Any_Dynamic_Array *) arr, (void *) item, size)
#define dg_dynamic_array_try_push(arr, item) dg_dynamic_array_try_push_size(arr, item, sizeof(*(arr)->data))

void dg_dynamic_array_push_or_error_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size);
#define dg_dynamic_array_push_size_or_error(arr, item, size) dg_dynamic_array_push_or_error_impl((_DG_Any_Dynamic_Array *)arr, (void *) &item, size)
#define dg_dynamic_array_push_or_error(arr, item) dg_dynamic_array_push_size_or_error(arr, item, sizeof(*(arr)->data))

void dg_dynamic_array_push_or_grow_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, void *data, u32 item_size);
#define dg_dynamic_array_push_size_or_grow(a, arr, item, size) dg_dynamic_array_push_or_grow_impl(a, (_DG_Any_Dynamic_Array *)arr, (void *) &item, size)
#define dg_dynamic_array_push_or_grow(a, arr, item) dg_dynamic_array_push_size_or_grow(a, arr, item, sizeof(*(arr)->data))

#define dg_dynamic_array_push dg_dynamic_array_push_or_grow

void dg_dynamic_array_clear_impl(_DG_Any_Dynamic_Array *arr);
#define dg_dynamic_array_clear(arr) dg_dynamic_array_clear_impl((_DG_Any_Dynamic_Array *) (arr))


//
// slices
//

#define DG_Make_Slice_Type(type) \
struct { \
  i32 len; \
  type *data; \
}


typedef DG_Make_Slice_Type(void) _DG_Any_Slice;

void dg_slice_make_impl(DG_Arena *a, _DG_Any_Slice *slice, u64 len, u64 item_size);
#define dg_slice_make(arena, slice, len) dg_slice_make_impl(arena, (_DG_Any_Slice *)slice, len, sizeof(*(slice)->data))

//
// Singly Linked Lists (Sll)
//

#define DG_Make_Sll_Node_Type(struct_name, type) \
struct struct_name { \
  struct struct_name *next; \
  type data; \
}

typedef DG_Make_Sll_Node_Type(_dg_any_node, void *) _DG_Any_Sll_Node;

internal void dg_sll_push_impl(_DG_Any_Sll_Node **first, _DG_Any_Sll_Node **last, _DG_Any_Sll_Node *node)
{
  if (!*first) {
    *first = node;
  } else if (*last) {
    (*last)->next = node;
  }
  *last = node;
  node->next = 0;
}

#define dg_sll_push(first, last, node) dg_sll_push_impl( \
    (_DG_Any_Sll_Node **) first, \
    (_DG_Any_Sll_Node **) last, \
    (_DG_Any_Sll_Node *) node)

internal void dg_sll_pop_front_impl(_DG_Any_Sll_Node **first, _DG_Any_Sll_Node **last)
{
  if (*first == *last) {
    *first = 0;
    *last = 0;
  } else {
    *first = (*first)->next;
  }
}

#define dg_sll_pop_front(first, last) dg_sll_pop_impl( \
    (_DG_Any_Sll_Node **) first, \
    (_DG_Any_Sll_Node **) last)


// }}}

#endif /* LIBCDG_H */
