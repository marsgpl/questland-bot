// thread

#include <pthread.h>

#include "lua_pd.h"
#include "lualib.h"

//

#define LUA_MT_THREAD "mt.thread"

#define LUA_THREAD_ID_METAFIELD "_thread_id"
#define LUA_THREAD_ARGS_METAFIELD "_thread_args"

#define LUA_THREAD_STATE_RUNNING 0
#define LUA_THREAD_STATE_DEAD 1

//

typedef struct lua_ud_thread {
    pthread_t thread;
    lua_State *L;
    uint64_t id; // thread unique id (unique in process scope)
    short state;
} lua_ud_thread;

//

LUAMOD_API int luaopen_thread( lua_State *L );

static int lua_thread_start( lua_State *L );
static int lua_thread_args( lua_State *L );
static int lua_thread_id( lua_State *L );

static int lua_thread_stop( lua_State *L );
static int lua_thread_join( lua_State *L );
static int lua_thread_gc( lua_State *L );

static uint64_t inc_id( void );
static void *lua_thread_create_worker( void *arg );
static void lua_thread_xcopy( lua_State *fromL, int fromIndex, lua_State *toL );
static int lua_thread_atpanic( lua_State *L );
static int lua_custom_traceback( lua_State *L );
static int lua_custom_pcall( lua_State *L, int narg, int nres );

//

static const luaL_Reg __index[] = {
    {"start", lua_thread_start},
    {"args", lua_thread_args},
    {"id", lua_thread_id},
    {NULL, NULL}
};

static const luaL_Reg __thread_index[] = {
    {"stop", lua_thread_stop},
    {"join", lua_thread_join},
    {NULL, NULL}
};
