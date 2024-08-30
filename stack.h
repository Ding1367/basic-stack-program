#ifndef _DING_STACK_H_
#define _DING_STACK_H_

#include <stdlib.h>
#include <math.h>

struct Stack {
    size_t top;
    size_t sz;
    int64_t *stk;
};

#define ding_stack_t struct Stack
#define STKP_NOT_FOUND -pow(2, 31)
#define STKP_CODE_SUCCESS 1000
#define STK_CODE_FAILED 500

extern int tref;
extern int sref;

#ifdef __cplusplus
extern "C" {
#endif
#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>
void print_stack(ding_stack_t *);
ding_stack_t *create_stack(size_t);
int push_to(ding_stack_t *, int64_t);
int peek_at(ding_stack_t *, int64_t *);
int pop_from(ding_stack_t *, int64_t *);
void stkp_push_stack_to_lua(lua_State *, ding_stack_t *);
int stkp_call_operation(lua_State *, const char *, ding_stack_t *stk);
void stkp_graceful_exit(int);
#ifdef __cplusplus
}
#endif

#endif
