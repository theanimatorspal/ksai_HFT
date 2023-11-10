#include "hftsim.hpp"
#include <sol/sol.hpp>

#ifdef _WIN32
    #define DLLEXPORT __declspec(dllexport)
#else
    #define DLLEXPORT
#endif

extern "C" DLLEXPORT int luaopen_hftsim(lua_State *L) {
        sol::state_view s(L);
        auto jkrguiApp = s["jkrguiApp"].get_or_create<sol::table>();
        jkrguiApp.set_function("hello", []() { printf("Hello from JkrGUI\n");});
        return 1;
}
