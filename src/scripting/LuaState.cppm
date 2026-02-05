module;

#include <sol/sol.hpp>

export module WFE.Scripting.LuaState;

export class LuaState
{
public:
    static sol::state& Get()
    {
        static sol::state lua;
        static bool init = false;
        if (!init)
        {
            lua.open_libraries(
                sol::lib::base,
                sol::lib::math,
                sol::lib::table,
                sol::lib::string
            );
            init = true;
        }

        return lua;
    }
};