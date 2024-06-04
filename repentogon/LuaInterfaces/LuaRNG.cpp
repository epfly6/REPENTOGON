#include "IsaacRepentance.h"
#include "LuaCore.h"
#include "Log.h"
#include "HookSystem.h"

LUA_FUNCTION(Lua_RNGSetSeed) {
	RNG* rng = lua::GetUserdata<RNG*>(L, 1, lua::Metatables::RNG, lua::metatables::RngMT);

	unsigned int seed = (unsigned int)luaL_checkinteger(L, 2);
	unsigned int shiftidx = (unsigned int)luaL_optinteger(L, 3, 35);

	if (seed <= 0) {
		luaL_argerror(L, 2, "RNG seed must be an integer above 0");
	}

	if (shiftidx < 0 || shiftidx > 80) {
		luaL_argerror(L, 3, "RNG shift index must be between 0 and 80 (inclusive)");
	}

	rng->SetSeed(seed, shiftidx);

	return 0;
}

static void DoRandomInt(lua_State* L, RNG* rng, int& result, bool& negative) {
	int res = 0;
	int arg1 = (int)luaL_checkinteger(L, 2);

	if (lua_gettop(L) == 3) {
		int arg2 = (int)luaL_checkinteger(L, 3);

		if (arg1 > arg2) {
			luaL_argerror(L, 1, "interval is empty");
		}
		else
		{
			unsigned int interval = arg2 - arg1;
			res = rng->RandomInt(interval + 1); // +1 to make it inclusive like math.random
			res += arg1;
		}
	}
	else {
		res = rng->RandomInt(arg1);
		negative = false;
	}

	result = res;
}

LUA_FUNCTION(Lua_RNG_RandomInt) {
	RNG* rng = lua::GetUserdata<RNG*>(L, 1, lua::Metatables::RNG, lua::metatables::RngMT);
	bool negative = true;
	int result = 0;
	DoRandomInt(L, rng, result, negative);
	if (!negative) {
		lua_pushinteger(L, (unsigned int)result);
	}
	else {
		lua_pushinteger(L, result);
	}
	return 1;
}

LUA_FUNCTION(Lua_RNG_PhantomInt) {
	RNG* rng = lua::GetUserdata<RNG*>(L, 1, lua::Metatables::RNG, lua::metatables::RngMT);
	RNG copy;
	memcpy(&copy, rng, sizeof(RNG));
	bool negative = true;
	int result = 0;
	DoRandomInt(L, &copy, result, negative);
	if (!negative) {
		lua_pushinteger(L, (unsigned int)result);
	}
	else {
		lua_pushinteger(L, result);
	}
	return 1;
}

LUA_FUNCTION(Lua_RNG_PhantomFloat) {
	RNG* rng = lua::GetUserdata<RNG*>(L, 1, lua::Metatables::RNG, lua::metatables::RngMT);
	RNG copy;
	memcpy(&copy, rng, sizeof(RNG));

	float result = copy.RandomFloat();
	lua_pushnumber(L, result);
	return 1;
}

void DoRandomVector(lua_State* L, RNG* rng, bool phantom) {
	Vector result = Isaac::RandomUnitVector(rng->_seed);
	if (!phantom) {
		rng->Next(); // we do it after calling bc the func increments the seed before use
	}
	Vector* toLua = lua::luabridge::UserdataValue<Vector>::place(L, lua::GetMetatableKey(lua::Metatables::VECTOR));
	*toLua = result;
	return;
}

LUA_FUNCTION(Lua_RNG_RandomVector) {
	RNG* rng = lua::GetUserdata<RNG*>(L, 1, lua::Metatables::RNG, lua::metatables::RngMT);
	DoRandomVector(L, rng, false);
	return 1;
}

LUA_FUNCTION(Lua_RNG_PhantomVector) {
	RNG* rng = lua::GetUserdata<RNG*>(L, 1, lua::Metatables::RNG, lua::metatables::RngMT);
	DoRandomVector(L, rng, true);
	return 1;
}

LUA_FUNCTION(Lua_RNG_Constructor) {
	int seed = luaL_optinteger(L, 2, 0xAA17414F);
	int shiftIdx = luaL_optinteger(L, 3, 35);

	if (seed == 0) {
		return luaL_error(L, "Invalid seed 0 for RNG object\n");
	}

	if (shiftIdx < 0 || shiftIdx > 80) {
		return luaL_error(L, "Invalid shift index %d for RNG object\n", shiftIdx);
	}

	void* key = lua::GetMetatableKey(lua::Metatables::RNG);
	RNG* rng = lua::luabridge::UserdataValue<RNG>::place(L, key);
	rng->SetSeed(seed, shiftIdx);

	return 1;
}

static void OverrideRNGConstructor(lua_State* L) {
	lua::LuaStackProtector protector(L);

	int result = lua_getglobal(L, "RNG");
	if (result == LUA_TNIL) {
		lua_pop(L, 1);
		ZHL::Log("[ERR] No global \"RNG\" in Lua environment\n");
		return;
	} 
	else if (result != LUA_TTABLE) {
		lua_pop(L, 1);
		ZHL::Log("[ERR] Global \"RNG\" is not a table\n");
		return;
	}

	result = lua_getmetatable(L, -1);
	if (result == 0) {
		lua_pop(L, 1);
		ZHL::Log("[ERR] Global \"RNG\" has not metatable\n");
		return;
	}

	lua_pushcfunction(L, Lua_RNG_Constructor);
	lua_setfield(L, -2, "__call");
	lua_pop(L, 2);
	return;
}

HOOK_METHOD(LuaEngine, RegisterClasses, () -> void) {
	super();

	lua::LuaStackProtector protector(_state);

	luaL_Reg functions[] = {
		{ "SetSeed", Lua_RNGSetSeed },
		{ "RandomInt", Lua_RNG_RandomInt },
		{ "PhantomInt", Lua_RNG_PhantomInt },
		{ "PhantomFloat", Lua_RNG_PhantomFloat },
		{ "RandomVector", Lua_RNG_RandomVector },
		{ "PhantomVector", Lua_RNG_PhantomVector },
		{ NULL, NULL }
	};

	OverrideRNGConstructor(_state);
	lua::RegisterFunctions(_state, lua::Metatables::RNG, functions);
}