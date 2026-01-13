#include "libcdg.h"

// Dynamic arrays

internal inline void dg_dynamic_array_make_buffer_unchecked(_DG_Any_Dynamic_Array *arr, u32 capacity, u8 *data)
{
  _DG_Any_Dynamic_Array replica = {0};
  replica.cap = capacity;
  replica.data = data;
  DG_MEMCPY(arr, &replica, sizeof(replica));
}

// TODO: colocar em libcdg.h
DG_SYMBOL void dg_dynamic_array_make_buffer_impl(u8 *data, usize data_size, _DG_Any_Dynamic_Array *arr, u32 capacity, u32 item_size)
{
  DG_ASSERT((usize)(capacity * item_size) <= data_size);
  dg_dynamic_array_make_buffer_unchecked(arr, capacity, data);
}

DG_SYMBOL void dg_dynamic_array_make_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, u32 capacity, u32 item_size)
{
  u8 *data = dg_arena_alloc_size(a, item_size * capacity);
  dg_dynamic_array_make_buffer_unchecked(arr, capacity, data);
}


internal void dg_dynamic_array_grow(DG_Arena *a, _DG_Any_Dynamic_Array *arr, u32 item_size)
{
  _DG_Any_Dynamic_Array replica = {0};
  DG_MEMCPY(&replica, arr, sizeof(replica));

  if (!replica.data) {
    dg_dynamic_array_make_impl(a, &replica, 8, item_size);
  } else {
    dg_dynamic_array_resize_impl(a, &replica, replica.cap * 2, item_size);
  }
  DG_MEMCPY(arr, &replica, sizeof(replica));
}

DG_SYMBOL void dg_dynamic_array_resize_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, i32 new_cap, u32 item_size)
{
  usize maybe_arena_pos = a->base_pos + (uptr)(arr->cap * item_size + sizeof(DG_Arena));
  if (maybe_arena_pos == a->pos) {
    // TODO: if the last allocation was this dynamic array, does not need to copy the data nor change the pointer
    if (new_cap > arr->cap) {
      dg_arena_alloc_size(a, item_size * (new_cap - arr->cap));
    } else if (new_cap < arr->cap) {
      usize new_pos = a->pos - ((arr->cap - new_cap) * item_size);
      dg_arena_pop_to(a, new_pos);
    }
  } else if (new_cap > arr->cap) {
    arr->data = dg_arena_alloc_size(a, item_size * new_cap);
    DG_MEMCPY(arr->data, arr->data, arr->len * item_size);
  }
  arr->cap = new_cap;
}

DG_SYMBOL void dg_dynamic_array_pop_impl(_DG_Any_Dynamic_Array *arr, void *dst, u32 item_size)
{
  DG_ASSERT(arr->len > 0);
  if (dst) {
    void *last_item_start = (void *)((uptr)arr->data + ((arr->len - 1) * item_size));
    DG_MEMCPY(dst, last_item_start, item_size);
  }
  arr->len -= 1;
}

DG_SYMBOL void dg_dynamic_array_push_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size)
{
  void *dst = (void *)(((uptr)arr->data) + (arr->len * item_size));
  arr->len++;
  // NOTE: remover?
  DG_MEMZERO_SIZE(dst, item_size);
  DG_MEMCPY(dst, src, item_size);
}

DG_SYMBOL bool dg_dynamic_array_try_push_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size)
{
  if (arr->len < arr->cap) {
    dg_dynamic_array_push_impl(arr, src, item_size);
    return true;
  }

  return false;
}

DG_SYMBOL void dg_dynamic_array_push_or_error_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size)
{
  DG_ASSERT(arr->len < arr->cap);
  dg_dynamic_array_push_impl(arr, src, item_size);
}

DG_SYMBOL void dg_dynamic_array_push_or_grow_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, void *src, u32 item_size)
{
  if (arr->len >= arr->cap) {
    dg_dynamic_array_grow(a, arr, item_size);
  }
  dg_dynamic_array_push_impl(arr, src, item_size);
}

DG_SYMBOL void dg_dynamic_array_clear_impl(_DG_Any_Dynamic_Array *arr)
{
  arr->len = 0;
}

DG_SYMBOL void dg_dynamic_array_as_slice_impl(_DG_Any_Dynamic_Array *arr, _DG_Any_Slice *out_slice)
{
  out_slice->len = arr->len;
  out_slice->data = arr->data;
}

DG_SYMBOL void dg_slice_make_from_dynamic_array_impl (
  DG_Arena *arena,
  _DG_Any_Slice *dest,
  _DG_Any_Dynamic_Array *source,
  u32 _slice_item_size
) {
  _DG_Any_Slice result_to_copy = {0};
  dg_dynamic_array_as_slice(source, &result_to_copy);
  dg_slice_copy_impl(arena, &result_to_copy, dest, _slice_item_size);
}


// Slices

DG_SYMBOL void dg_slice_make_impl(DG_Arena *a, _DG_Any_Slice *slice, u32 len, u32 item_size)
{
  DG_ASSERT(most_significant_bit(len) <= 31);
  _DG_Any_Slice res = {0};

  void *data = dg_arena_alloc_size(a, len * item_size);

  if (data) {
    res.len = len;
    res.data = data;
  }

  *slice = res;
}

DG_SYMBOL void dg_slice_set(_DG_Any_Slice *arr, i32 index, void *value, u32 item_size)
{
  DG_ASSERT(index >= 0);
  DG_ASSERT(index < arr->len);

  void *item_start = (void *)((uptr)arr->data + (index * item_size));
  if (value) {
    DG_MEMCPY(item_start, value, item_size);
  }

}

DG_SYMBOL void *dg_slice_access_impl(_DG_Any_Slice *slice, i32 index, u32 item_size)
{
  DG_ASSERT(index >= 0);
  DG_ASSERT(index < slice->len);

  void *item_start = (void *)((uptr)slice->data + (index * item_size));
  return item_start;
}

DG_SYMBOL void dg_slice_get(_DG_Any_Slice *arr, i32 index, u32 item_size, void *opt_dst)
{
  DG_ASSERT(opt_dst);
  void *item_start = dg_slice_access_impl(arr, index, item_size);
  if (opt_dst) {
    DG_MEMCPY(opt_dst, item_start, item_size);
  }
}

internal void _dg_slice_mergesort_merge(
  Ordering_Kind (*compare_fn)(void *, void *),
  _DG_Any_Slice *input,
  _DG_Any_Slice *output,
  i32 start, i32 middle, i32 end, usize item_size)
{

  i32 first_half_idx = start, second_half_idx = middle + 1, cursor = start;

  while (first_half_idx <= middle && second_half_idx <= end) {

    if (
      compare_fn(
        dg_slice_access_impl(input, first_half_idx, item_size),
        dg_slice_access_impl(input, second_half_idx, item_size)) <= ORDERING_EQ)
    {
      dg_slice_set(output, cursor, dg_slice_access_impl(input, first_half_idx, item_size), item_size);
      cursor++;
      first_half_idx++;
    } else {
      dg_slice_set(output, cursor, dg_slice_access_impl(input, second_half_idx, item_size), item_size);
      cursor++;
      second_half_idx++;
    }
  }

  if (first_half_idx <= middle) {
    DG_MEMCPY(
      dg_slice_access_impl(output, cursor, item_size),
      dg_slice_access_impl(input, first_half_idx, item_size),
      item_size * (middle - first_half_idx + 1)
    );
  }

  if (second_half_idx <= end) {
    DG_MEMCPY(
      dg_slice_access_impl(output, cursor, item_size),
      dg_slice_access_impl(input, second_half_idx, item_size),
      item_size * (end - second_half_idx + 1)
    );
  }

  DG_MEMCPY(
    dg_slice_access_impl(input, start, item_size),
    dg_slice_access_impl(output, start, item_size),
    item_size * (end - start + 1)
  );

}

internal void _dg_slice_mergesort(
  Ordering_Kind (*compare_fn)(void *, void *),
  _DG_Any_Slice *input,
  _DG_Any_Slice *output,
  i32 start, i32 end, usize item_size)
{
  if (start < end) {
    i32 middle = start + (end - start) / 2;
    _dg_slice_mergesort(compare_fn, input, output, start, middle, item_size);
    _dg_slice_mergesort(compare_fn, input, output, middle + 1, end, item_size);
    _dg_slice_mergesort_merge(compare_fn, input, output, start, middle, end, item_size);
  }
}


DG_SYMBOL void dg_slice_mergesort_impl(
  _DG_Any_Slice *slice,
  Ordering_Kind (*compare_fn)(void *, void *),
  usize item_size)
{
  DG_ASSERT(compare_fn);
  DG_Temp_Arena scratch = dg_scratch_get(0);
  {
    _DG_Any_Slice aux;
    dg_slice_make_impl(scratch.arena, &aux, slice->len, item_size);
    _dg_slice_mergesort(compare_fn, slice, &aux, 0, slice->len - 1, item_size);
  }
  dg_scratch_release(scratch);
}

DG_SYMBOL void dg_dynamic_array_mergesort_impl(
  _DG_Any_Dynamic_Array *dynarr,
  Ordering_Kind (*compare_fn)(void *, void *),
  usize item_size)
{
  DG_ASSERT(dynarr);
  _DG_Any_Slice slice = {0};
  dg_dynamic_array_as_slice(dynarr, &slice);
  dg_slice_mergesort_impl(&slice, compare_fn, item_size);
}

// NOTE: inclusive on start, exclusive on finish
DG_SYMBOL void dg_slice_subslice_impl(_DG_Any_Slice *slice, i32 start, i32 finish, _DG_Any_Slice *out_subslice, usize item_size)
{
  DG_ASSERT(out_subslice);
  DG_ASSERT(start <= finish);
  DG_ASSERT(start >= 0);
  DG_ASSERT(finish >= 0);

  i32 length = finish - start;

  _DG_Any_Slice result = {0};
  result.len = length;
  result.data = (void*)((uptr)slice->data + (start * item_size));

  *out_subslice = result;
}

DG_SYMBOL void dg_slice_copy_impl(DG_Arena *a, _DG_Any_Slice *source, _DG_Any_Slice *dest, usize item_size)
{
  _DG_Any_Slice result = *source;
  result.data = dg_arena_alloc_size(a, source->len * item_size);
  DG_MEMCPY(result.data, source->data, source->len * item_size);
  *dest = result;
}

