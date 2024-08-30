#include <stack.h>
#include <stdlib.h>
#include <string.h>

static int __resize(lua_State *L){
    ding_stack_t **stkp = luaL_checkudata(L, 1, "stack");
    ding_stack_t *stack = *stkp;
    size_t newsz = luaL_checkinteger(L, 2);
    size_t actualsz = stack->sz;
    if (newsz == actualsz){
        lua_pushboolean(L, 1);
        return 1;
    }
    int64_t *newstkarr = realloc(stack->stk, newsz * sizeof(int64_t));
    if (!newstkarr){
        lua_pushboolean(L, 0);
        return 1;
    }
    // success
    stack->sz = newsz;
    if (newsz > actualsz){
        int64_t *excess = stack->stk + actualsz;
        memset(excess, 0, (newsz - actualsz) * sizeof(int64_t));
    }
    stack->stk = newstkarr;
    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg cstackopfuncs[] = {
    {"_resize", __resize},
    {NULL, NULL}
};

int luaopen_cstackops(lua_State *L){
    luaL_newlib(L, cstackopfuncs);
    return 1;
}
