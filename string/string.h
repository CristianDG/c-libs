#ifndef STRING_H
#define STRING_H
#include "../libcdg/libcdg.h"
#include <stdarg.h>

typedef DG_Make_Slice_Type(u8) String8;

typedef DG_Make_Sll_Node_Type(string_node, String8) String_Node;

typedef struct {
  String_Node *first;
  String_Node *last;
  u32 node_count;
  u32 total_size;
} String_List;

typedef DG_Make_Slice_Type(String8) String_Array;


#define ensure_string_literal(str) ("" str "")

DG_SYMBOL String8 string8(char *str, usize len);
DG_SYMBOL String8 string8_vfmt(DG_Arena *a, char *fmt, va_list args);
DG_SYMBOL String8 string8_fmt(DG_Arena *a, char *fmt, ...);
DG_SYMBOL String8 string8_from_cstring_clamped(char *data, u32 max_len);
DG_SYMBOL String8 string8_from_cstring(char *data);
DG_SYMBOL String8 string8_from_ncstring(char *data, u32 len);
#define           string8_literal(str) string8_from_ncstring(ensure_string_literal(str), (sizeof (str)) - 1)

DG_SYMBOL String8 string8_substring_alias(String8 str, u32 low, u32 high);
DG_SYMBOL String8 string8_substring_copy(DG_Arena *a, String8 str, u32 low, u32 high);
DG_SYMBOL usize   string8_len(String8 str);
DG_SYMBOL usize   string8_size(String8 str);

DG_SYMBOL String8 string8_copy(DG_Arena *a, String8 str);
DG_SYMBOL char*   string8_copy_to_cstring(DG_Arena *a, String8 str);
DG_SYMBOL void    string8_copy_to_buffer(String8 str, char *buffer, usize buffer_size);

DG_SYMBOL String_List string8_split_lines(DG_Arena *a, String8 str);

enum string8_split_flags {
  STR_SPLIT_KEEP_EMPTIES = 1 << 0,
};

struct string8_split_opt {
  u8 flags;
};

// FIXME: olhar essas funções {
DG_SYMBOL String_List string8_split_on_ncstring_pro(DG_Arena *a, String8 str, char *split_chars, u32 split_chars_count, u8 flags);
DG_SYMBOL String_List string8_split_on_ncstring_impl(DG_Arena *a, String8 str, char *split_chars, u32 split_chars_count, struct string8_split_opt *params);
#define               string8_split_on_ncstring(arena, str, split_chars, split_chars_count, ...) string8_split_on_ncstring_impl((arena), (str), (split_chars), (split_chars_count), &(struct string8_split_opt) { __VA_ARGS__ })
#define               string8_split_on_string_literal(arena, str, str_lit, ...) string8_split_on_ncstring_impl((arena), (str), ensure_string_literal(str_lit), DG_STRLEN(str_lit), &(struct string8_split_opt) { __VA_ARGS__ })
DG_SYMBOL String_List string8_split_on_string8_impl(DG_Arena *a, String8 str, String8 split_str, struct string8_split_opt *params);
#define               string8_split_on_string8(arena, str, split_str, ...) string8_split_on_string8_impl((arena), (str), (split_str), &(struct string8_split_opt) { __VA_ARGS__ })
DG_SYMBOL String_List string8_split_on_char_impl(DG_Arena *arena, String8 str, char c, struct string8_split_opt *params);
#define               string8_split_on_char(arena, str, c, ...) string8_split_on_char_impl((arena), (str), (c), &(struct string8_split_opt) { __VA_ARGS__ })
// }

DG_SYMBOL String8 string_list_to_string8(DG_Arena *a, String_List *list);
DG_SYMBOL char*   string_list_to_cstring(DG_Arena *a, String_List *list);

DG_SYMBOL String_Node *string_list_push_node(String_List *list, String_Node *node);
DG_SYMBOL String_Node *string_list_push_fmt(DG_Arena *a, String_List *list, char *fmt, ...);
DG_SYMBOL String_Node *string_list_push_string8(DG_Arena *a, String_List *list, String8 str);
DG_SYMBOL String_Node *string_list_push_ncstring(DG_Arena *a, String_List *list, char *str, u32 length);
DG_SYMBOL String_Node *string_list_push_cstring(DG_Arena *a, String_List *list, char *str);
#define                string_list_push_string_literal(a, list, str) string_list_push_ncstring(a, list, ensure_string_literal(str), sizeof str)

DG_SYMBOL String_Array string_list_to_array(DG_Arena *a, String_List list);

struct string8_parse_opt {
  u8 base;
};

DG_SYMBOL u32 string8_parse_u32_impl(String8 str, struct string8_parse_opt *params);
#define       string8_parse_u32(str, ...) string8_parse_u32_impl(str, &(struct string8_parse_opt) { __VA_ARGS__ })

DG_SYMBOL i32 string8_parse_i32(String8 str);
DG_SYMBOL f32 string8_parse_f32(String8 str);

#endif
