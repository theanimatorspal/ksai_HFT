#include "hftsim.hpp"
#include <sol/sol.hpp>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT int luaopen_hftsim(lua_State *L) {
  // sol::state_view s(L);
  // std::cout << "HELLO EVYERHING\n";
  // auto hft = s["hft"].get_or_create<sol::table>();
  // hft.set_function("hello", []() { printf("Hello from hftsim\n"); });
  return 1;
}
