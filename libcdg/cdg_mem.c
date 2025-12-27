#include "libcdg.h"

#ifndef DG_SCRATCH_ARENA_COUNT
#define DG_SCRATCH_ARENA_COUNT 2
#endif

global_variable thread_static DG_Arena *global_scratch[DG_SCRATCH_ARENA_COUNT] = {0};

DG_SYMBOL u8 most_significant_bit(u64 value) {
  u64 mask = 1;
  mask <<= 63;
  for (i64 i = 63; i >= 0; --i) {
    if (value & mask) {
      return i + 1;
    }
    mask >>= 1;
  }

  return 0;
}

DG_SYMBOL void dg_scratch_memory_init(void)
{
  // TODO: reservar no lugar de alocar
  u64 memory_size = 4 * MEGABYTE;
  u8 *memory = malloc(memory_size);

  dg_scratch_memory_init_buffer(memory, memory_size);
}

DG_SYMBOL DG_Temp_Arena dg_scratch_get(DG_Arena *conflict)
{
  if (!global_scratch[0]) {
    dg_scratch_memory_init();
  }

  DG_Arena *result = 0;

  DG_Arena **scratch_arena = global_scratch;

  for (u32 i = 0; i < DG_SCRATCH_ARENA_COUNT; ++i, ++scratch_arena) {
    bool is_non_conflicting = true;

    if (conflict == *scratch_arena) {
      is_non_conflicting = false;
    }

    if (is_non_conflicting) {
      result = *scratch_arena;
      break;
    }
  }

  return dg_temp_arena_begin(result);
}

DG_SYMBOL void dg_scratch_memory_init_buffer(u8 *data, usize size)
{
  DG_ASSERT(data != 0);
  DG_ASSERT(size > sizeof(global_scratch));

  u32 non_reserved_size = size - sizeof(global_scratch);
  u32 individual_arena_size = non_reserved_size / DG_SCRATCH_ARENA_COUNT;

  DG_ENSURE(individual_arena_size > sizeof(DG_Arena));

  DG_ASSERT(
      (individual_arena_size * DG_SCRATCH_ARENA_COUNT)
      <= size
  );

  uptr curr_ptr = (uptr)data;
  for (u32 i = 0; i < DG_SCRATCH_ARENA_COUNT; ++i) {
    global_scratch[i] = dg_arena_init_buffer((u8 *)curr_ptr, individual_arena_size);
    curr_ptr += individual_arena_size;
  }
}



DG_SYMBOL DG_Arena *dg_arena_init_buffer(void *data, size_t size)
{
  DG_Arena *arena = data;
  DG_MEMSET(arena, 0, sizeof *arena);

  arena->size = size;

  arena->current = arena;
  arena->pos = sizeof(*arena);

  return arena;
}


internal b32 dg_is_power_of_two(size_t x) {
  return ((x != 0) && ((x & (x - 1)) == 0));
}

// implementation from https://dylanfalconer.com/articles/the-arena-custom-memory-allocators
internal uintptr_t dg_align_forward(uintptr_t ptr, size_t alignment)
{
  uintptr_t p, a, modulo;
  if (!dg_is_power_of_two(alignment)) {
    DG_CRASH();
    return 0;
  }

  p = ptr;
  a = (uintptr_t)alignment;
  modulo = p & (a - 1);

  if (modulo) {
    p += a - modulo;
  }

  return p;
}

// TODO: olhar https://youtu.be/443UNeGrFoM?si=DBJXmKB_z8W8Yrrf&t=3074
DG_SYMBOL void *dg_arena_alloc_impl(DG_Arena *arena, size_t size, size_t alignment, char *file, int line)
{
  DG_Arena *current = arena->current;

  uptr pos_pre = dg_align_forward(current->pos, alignment);
  uptr pos_post = pos_pre + size;

  if (pos_post > current->size) {
    DG_Arena *next = 0;

    // TODO: get page size
    usize arena_size_aligned = dg_align_forward(sizeof(DG_Arena), alignment);
    usize commit_size = dg_align_forward(size + arena_size_aligned, alignment);
    commit_size = MAX(commit_size, 4 * KILOBYTE);

    { // NOTE: round to nearest kilobyte
      u32 kb_ammount = 0;
      kb_ammount = (commit_size / KILOBYTE);
      if (commit_size % KILOBYTE) {
        kb_ammount += 1;
      }
      commit_size = kb_ammount * KILOBYTE;
    }

    u8 *data = DG_MALLOC(commit_size);
    next = dg_arena_init_buffer(data, commit_size);

    next->base_pos = current->base_pos + current->size;

    next->prev = arena->current;
    arena->current = next;

    current = next;
    pos_pre = dg_align_forward(current->pos, alignment);
    pos_post = pos_pre + size;
  }

  void *result = 0;
  result = (u8 *)current + pos_pre;
  current->pos = pos_post;
  DG_MEMZERO_SIZE(result, size);

  return result;
}

DG_SYMBOL void dg_arena_deinit(DG_Arena *a)
{
  free(a);
}

DG_SYMBOL void dg_arena_pop_to(DG_Arena *a, usize pos)
{

  internal u32 count;
  count++;
  if (count == 3) {
    DG_BREAKPOINT;
  }

  usize actual_pos = CLAMP_BOTTOM(sizeof(DG_Arena), pos);
  DG_Arena *current = a->current;

  for (DG_Arena *tmp = current; tmp->base_pos >= actual_pos;) {
    tmp = tmp->prev;
    dg_arena_deinit(current);
    current = tmp;
  }

  a->current = current;
  usize new_pos = actual_pos - current->base_pos;
  DG_ENSURE(new_pos <= current->pos);
  current->pos = new_pos;
}

DG_SYMBOL DG_Temp_Arena dg_temp_arena_begin(DG_Arena *a)
{
  DG_Arena *current = a->current;
  usize cursor = current->base_pos + current->pos;
  return (DG_Temp_Arena) {
    .arena = a,
    .cursor = cursor,
  };
}

DG_SYMBOL void dg_temp_arena_end(DG_Temp_Arena tmp_mem)
{
  dg_arena_pop_to(tmp_mem.arena, tmp_mem.cursor);
}

// TODO:
DG_SYMBOL void dg_arena_clear(DG_Arena *arena)
{
  dg_arena_pop_to(arena, 0);
}
