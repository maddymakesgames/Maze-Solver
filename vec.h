#ifndef __VEC_HEADER__
#define __VEC_HEADER__

#ifndef __VEC_IMPL
typedef struct Vec* Vec;
#endif

Vec vec_new(int (*cmp)(void*, void*));
void vec_free(Vec vec);
void vec_insert(Vec vec, void* data);
bool vec_empty(Vec vec);
void* vec_remove(Vec vec);
int vec_len(Vec vec);

#endif
