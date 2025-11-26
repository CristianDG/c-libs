#include "libcdg.h"

// Dynamic arrays

internal inline void dg_dynamic_array_make_buffer_unchecked(_DG_Any_Dynamic_Array *arr, u32 capacity, u8 *data) {
  _DG_Any_Dynamic_Array replica = {0};
  replica.cap = capacity;
  replica.data = data;
  DG_MEMCPY(arr, &replica, sizeof(replica));
}

// TODO: colocar em libcdg.h
DG_SYMBOL void dg_dynamic_array_make_buffer_impl(u8 *data, usize data_size, _DG_Any_Dynamic_Array *arr, u32 capacity, u32 item_size) {
  DG_ASSERT((usize)(capacity * item_size) <= data_size);
  dg_dynamic_array_make_buffer_unchecked(arr, capacity, data);
}

DG_SYMBOL void dg_dynamic_array_make_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, u32 capacity, u32 item_size) {
  u8 *data = dg_arena_alloc(a, item_size * capacity);
  dg_dynamic_array_make_buffer_unchecked(arr, capacity, data);
}


// FIXME: utilizar DG_Allocator_Header no lugar de DG_Arena,
// pra isso preciso criar uma especie de função de realloc
internal void dg_dynamic_array_grow(DG_Arena *a, _DG_Any_Dynamic_Array *arr, u32 item_size) {
  _DG_Any_Dynamic_Array replica = {0};
  DG_MEMCPY(&replica, arr, sizeof(replica));

  if (!replica.data) {
    // TODO: default capacity
    replica.cap = 8;
    replica.data = dg_arena_alloc(a, 2 * item_size * replica.cap);
  } else {
    replica.data = dg_arena_realloc(a, replica.data, 2 * item_size * replica.cap);
  }

  replica.cap *= 2;
  DG_MEMCPY(arr, &replica, sizeof(replica));
}

DG_SYMBOL void dg_dynamic_array_pop_impl(_DG_Any_Dynamic_Array *arr, void *dst, u32 item_size) {
  DG_ASSERT(arr->len > 0);
  if (dst) {
    void *last_item_start = (void *)((uptr)arr->data + ((arr->len - 1) * item_size));
    DG_MEMCPY(dst, last_item_start, item_size);
  }
  arr->len -= 1;
}

DG_SYMBOL void dg_dynamic_array_push_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size) {
  void *dst = (void *)(((uptr)arr->data) + (arr->len * item_size));
  arr->len++;
  DG_MEMCPY(dst, src, item_size);
}

DG_SYMBOL bool dg_dynamic_array_try_push_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size) {
  if (arr->len < arr->cap) {
    dg_dynamic_array_push_impl(arr, src, item_size);
    return true;
  }

  return false;
}

DG_SYMBOL void dg_dynamic_array_push_or_error_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size) {
  DG_ASSERT(arr->len < arr->cap);
  dg_dynamic_array_push_impl(arr, src, item_size);
}

DG_SYMBOL void dg_dynamic_array_push_or_grow_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, void *src, u32 item_size) {
  if (arr->len >= arr->cap) {
    dg_dynamic_array_grow(a, arr, item_size);
  }
  dg_dynamic_array_push_impl(arr, src, item_size);
}

DG_SYMBOL void dg_dynamic_array_clear_impl(_DG_Any_Dynamic_Array *arr) {
  arr->len = 0;
}

DG_SYMBOL _DG_Any_Slice dg_dynamic_array_as_slice(_DG_Any_Dynamic_Array *arr) {
  _DG_Any_Slice result = {0};
  result.len = arr->len;
  result.data = arr->data;
  return result;
}


// Slices

DG_SYMBOL void dg_slice_make_impl(DG_Arena *a, _DG_Any_Slice *slice, u64 len, u64 item_size){
  _DG_Any_Slice res = {0};

  void *data = dg_arena_alloc(a, len * item_size);

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

DG_SYMBOL void *dg_slice_get_ptr(_DG_Any_Slice *arr, i32 index, u32 item_size) {
  DG_ASSERT(index >= 0);
  DG_ASSERT(index < arr->len);

  void *item_start = (void *)((uptr)arr->data + (index * item_size));
  return item_start;
}

DG_SYMBOL void dg_slice_get(_DG_Any_Slice *arr, i32 index, u32 item_size, void *opt_dst) {
  DG_ASSERT(opt_dst);
  void *item_start = dg_slice_get_ptr(arr, index, item_size);
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
        dg_slice_get_ptr(input, first_half_idx, item_size),
        dg_slice_get_ptr(input, second_half_idx, item_size)) <= ORDERING_EQ)
    {
      dg_slice_set(output, cursor, dg_slice_get_ptr(input, first_half_idx, item_size), item_size);
      cursor++;
      first_half_idx++;
    } else {
      dg_slice_set(output, cursor, dg_slice_get_ptr(input, second_half_idx, item_size), item_size);
      cursor++;
      second_half_idx++;
    }
  }

  if (first_half_idx <= middle) {
    DG_MEMCPY(
      dg_slice_get_ptr(output, cursor, item_size),
      dg_slice_get_ptr(input, first_half_idx, item_size),
      item_size * (middle - first_half_idx + 1)
    );
  }

  if (second_half_idx <= end) {
    DG_MEMCPY(
      dg_slice_get_ptr(output, cursor, item_size),
      dg_slice_get_ptr(input, second_half_idx, item_size),
      item_size * (end - second_half_idx + 1)
    );
  }

  DG_MEMCPY(
    dg_slice_get_ptr(input, start, item_size),
    dg_slice_get_ptr(output, start, item_size),
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
  usize item_size
) {
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
  usize item_size
) {
  DG_ASSERT(dynarr);
  _DG_Any_Slice slice = dg_dynamic_array_as_slice(dynarr);
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

DG_SYMBOL void dg_slice_copy_impl(DG_Arena *a, _DG_Any_Slice *slice, _DG_Any_Slice *out_slice, usize item_size)
{
  _DG_Any_Slice result = *slice;
  result.data = dg_arena_alloc(a, slice->len * item_size);
  DG_MEMCPY(result.data, slice->data, slice->len * item_size);
  *out_slice = result;
}

