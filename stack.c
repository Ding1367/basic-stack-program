#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define stack_t ding_stack_t

void print_stack(stack_t *stk) {
    printf("[%zu]: ", stk->top);
    for (int i=0; i < stk->top; i++) {
        printf("%lld->", stk->stk[i]);
    }
    putchar('\n');
}

stack_t *create_stack(size_t sz) {
    stack_t *stk = malloc(sizeof(stack_t));
    int64_t *arr = malloc(sizeof(int64_t) * sz);
    if (stk) {
        memset(stk, 0, sizeof(stack_t));
        if (arr) {
            memset(arr, 0, sizeof(int64_t) * sz);
            stk->stk = arr;
            stk->sz = sz;
            stk->top = 0;
        } else {
            free(stk);
            stk = NULL;
        }
    } else {
        if (arr) {
            free(arr);
        }
    }
    return stk;
}

int push_to(stack_t *stk, int64_t val) {
    if (stk->top >= stk->sz) {
        return 0;
    }
    stk->stk[stk->top++] = val;
    return 1;
}

int peek_at(stack_t *stk, int64_t *valp){
    if (stk->top == 0){
        return 0;
    }
    *valp = stk->stk[stk->top - 1];
    return 1;
}

int pop_from(stack_t *stk, int64_t *valp) {
    if (stk->top == 0) {
        // underflow condition
        return 0;
    }
    *valp = stk->stk[--stk->top];
    stk->stk[stk->top] = 0;
    return 1;
}

void destroy_stack(stack_t *stk) {
    free(stk->stk);
    free(stk);
}

static int skip = 0;

static void print_options(void){
    if (skip) return;
    puts("Choose an option:");
    puts("1.  Print the stack");
    puts("2.  Push");
    puts("3.  Pop");
    puts("4.  Exit");
    puts("5.  Load Lua file");
    puts("6.  Set top");
    puts("7.  Call operation from Lua");
    puts("8.  Peek");
    puts("9.  Get top");
    puts("10. List operations available");
    puts("11. List signed program names");
}

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int tref = LUA_REFNIL;
int sref = LUA_REFNIL;

static int _register_operation(lua_State *L){
    size_t l;
    const char *s = luaL_checklstring(L, 1, &l);
    if (l >= 64){
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, tref);
    lua_pushstring(L, s);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    lua_pushvalue(L, 2);
    lua_settable(L, -3);
    lua_pushboolean(L, 1);
    return 1;
}

static int _get_operation(lua_State *L){
    lua_rawgeti(L, LUA_REGISTRYINDEX, tref);
    lua_pushstring(L, luaL_checkstring(L, 1));
    lua_gettable(L, -2);
    return 1;
}

static int _remove_operation(lua_State *L){
    lua_rawgeti(L, LUA_REGISTRYINDEX, tref);
    lua_pushstring(L, luaL_checkstring(L, 1));
    lua_pushnil(L);
    lua_settable(L, -3);
    return 0;
}

static int _sign(lua_State *L){
    const char *str = luaL_checkstring(L, 1);
    lua_rawgeti(L, LUA_REGISTRYINDEX, sref);
    lua_pushstring(L, str);
    lua_gettable(L, -2);
    if (lua_toboolean(L, -1)){
        lua_pop(L, 2);
        lua_pushboolean(L, 0);
    } else {
        lua_pop(L, 1);
        lua_pushstring(L, str);
        lua_pushboolean(L, 1);
        lua_settable(L, -3);
    }
    return 1;
}

static int _getsigns(lua_State *L){
    lua_rawgeti(L, LUA_REGISTRYINDEX, sref);
    return 1;
}

static const luaL_Reg _Gfuncs[] = {
    {"regop", _register_operation},
    {"remop", _remove_operation},
    {"getop", _get_operation},
    {"sign", _sign},
    {"getsigns", _getsigns},
    {NULL, NULL}
};

#define gstk(L) *(stack_t**)luaL_checkudata(L, 1, "stack")

static int _stk_top(lua_State *L){
    stack_t *stk = gstk(L);
    if (lua_gettop(L) == 1){
        lua_pushinteger(L, stk->top);
        return 1;
    }
    size_t newtop = luaL_checkinteger(L, 2);
    if (newtop > stk->sz){
        lua_pushboolean(L, 0);
    } else {
        stk->top = newtop;
        lua_pushboolean(L, 1);
    }
    return 1;
}

static int _stk_push(lua_State *L){
    stack_t *stk = gstk(L);
    lua_pushboolean(L, push_to(stk, luaL_checkinteger(L, 2)));
    return 1;
}

static int _stk_pop(lua_State *L){
    stack_t *stk = gstk(L);
    int64_t popped;
    int s;
    lua_pushboolean(L, s = pop_from(stk, &popped));
    if (s)
        lua_pushinteger(L, popped);
    return s + 1;
}

static int _stk_peek(lua_State *L){
    stack_t *stk = gstk(L);
    int64_t topval;
    int s;
    lua_pushboolean(L, s = peek_at(stk, &topval));
    if (s)
        lua_pushinteger(L, topval);
    return s + 1;
}

static int _stk_size(lua_State *L){
    stack_t *stk = gstk(L);
    lua_pushinteger(L, stk->sz);
    return 1;
}

const luaL_Reg stkmeths[] = {
    {"push", _stk_push},
    {"pop", _stk_pop},
    {"size", _stk_size},
    {"top", _stk_top},
    {"peek", _stk_peek},
    {NULL, NULL}
};

static void __create_metatable(lua_State *L){
    luaL_newmetatable(L, "stack");
    luaL_newlibtable(L, stkmeths);
    luaL_setfuncs(L, stkmeths, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 2);
}

void stkp_push_stack_to_lua(lua_State *L, stack_t *stk){
    stack_t **stkp = lua_newuserdata(L, sizeof(stack_t *));
    *stkp = stk;
    luaL_setmetatable(L, "stack");
}

// proxies
#define NOT_FOUND STKP_NOT_FOUND
#define CODE_SUCCESS STKP_CODE_SUCCESS
#define CODE_FAILED STK_CODE_FAILED

int stkp_call_operation(lua_State *L, const char *name, stack_t *stk){
    lua_rawgeti(L, LUA_REGISTRYINDEX, tref);
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    // not checking if the type is a function but checking if the function exists
    if (lua_isfunction(L, -1)) {
        stkp_push_stack_to_lua(L, stk);
        int r = lua_pcall(L, 1, 1, 0);
        if (r != LUA_OK){
            return r;
        } else {
            return lua_toboolean(L, -1) ? CODE_SUCCESS : CODE_FAILED;
        }
    } else {
        return NOT_FOUND;
    }
    return 0;
}

#define PRINT_OPTIONS -1
#define PRINT_STACK 1
#define PUSH 2
#define POP 3
#define EXIT 4
#define LOAD_LUA 5
#define SET_TOP 6
#define CALL_OP 7
#define PEEK 8
#define GET_TOP 9
#define LIST_OPTS 10
#define LIST_NAMES 11

lua_State *L;
stack_t *stack; // main stack

void stkp_graceful_exit(int code){
    destroy_stack(stack);
    lua_close(L);
    exit(code);
}

static int _graceful_exit(lua_State *oL){
    int value = luaL_optinteger(oL, 1, 0);
    stkp_graceful_exit(value);
    return 0;
}

int main(int argc, char **argv) {
    if (argc > 1){
        for (int i = 1; i < argc; i++){
            if (strncmp(argv[1], "--nom", 5) == 0)
                skip = 1;
        }
    }
    L = luaL_newstate();
    if (!L){
        puts("failed to allocate lua state");
        return 1;
    }
    
    printf("Enter the size for the stack: ");
    size_t sz;
    scanf("%zu", &sz);
    if (sz == 0){
        puts("bad size");
        lua_close(L);
        return 1;
    }
    stack = create_stack(sz);
    if (!stack) {
        puts("failed to allocate stack");
        lua_close(L);
        return 1;
    }
    unsigned opt = 0;
    int64_t arg;
    // no problems, initialize lua state machine
    luaL_openlibs(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    luaL_setfuncs(L, _Gfuncs, 0);
    lua_register(L, "exit", _graceful_exit); // this should always exist
    __create_metatable(L);
    lua_newtable(L);
    tref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_newtable(L);
    sref = luaL_ref(L, LUA_REGISTRYINDEX);
    while (opt != EXIT){
        print_options();
        printf("> ");
        if (!scanf("%u", &opt)){
            char _[64];
            scanf("%63s", _);
            continue;
        }
        switch(opt){
        case PRINT_STACK:
            print_stack(stack);
            break;
        case PUSH:
            printf("Enter a value (integer) to push onto the stack: ");
            scanf("%lld", &arg);
            if (!push_to(stack, arg)){
                puts("Stack overflow");
                break;
            }
            break;
        case POP:{
            int64_t poppedval;
            int popped = pop_from(stack, &poppedval);
            if (!popped){
                puts("empty stack");
                break;
            }
            printf("Popped off %lld from the stack\n", poppedval);
            break;
        }
        case LOAD_LUA:{
            char fbuf[64] = {0};
            printf("Enter the file name to load: ");
            scanf("%63s", fbuf);
            if (luaL_dofile(L, fbuf) != LUA_OK){
                puts("failed to load file");
                puts(lua_tostring(L, -1));
                break;
            }
            break;
        }
        case SET_TOP:{
            size_t newtop;
            printf("Enter the new top of the stack: ");
            scanf("%zu", &newtop);
            if (newtop > stack->sz){
                puts("failed: new top exceeds size");
                break;
            }
            stack->top = newtop;
            break;
        }
        case CALL_OP:{
            char opbuf[64] = {0};
            printf("Enter the operation name to call to: ");
            scanf("%63s", opbuf);
            int r;
            if ((r = stkp_call_operation(L, opbuf, stack)) != CODE_SUCCESS){
                if (r == CODE_FAILED){
                    puts("execution failed"); // user defined error, not lua itself
                    break;
                }
                printf("failed: %d\n", r);
                printf("lua error message: %s\n", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
            break;
        }
        case PEEK:{
            int64_t topval;
            if (peek_at(stack, &topval) == 0){
                puts("empty stack");
                break;
            }
            printf("Top element is %lld\n", topval);
            break;
        }
        case GET_TOP:
            printf("%zu elements are on the stack\n", stack->top);
            break;
        case LIST_OPTS:
            printf("The following operations are available: ");
            lua_rawgeti(L, LUA_REGISTRYINDEX, tref);
            lua_pushnil(L);
            while (lua_next(L, -2) != 0){
                printf("%s\t", lua_tostring(L, -2));
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
            puts("");
            break;
        case LIST_NAMES:{
            printf("Signed: ");
            lua_rawgeti(L, LUA_REGISTRYINDEX, sref);
            lua_pushnil(L);
            while (lua_next(L, -2) != 0){
                printf("%s\t", lua_tostring(L, -2));
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
            puts("");
            break;
        }
        case PRINT_OPTIONS:{
            skip = 0;
            print_options();
            skip = 1;
            break;
        }
        default:
            break;
        }
    }
    
    destroy_stack(stack);
    lua_close(L);
    return 0;
}
