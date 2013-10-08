// Test that the userdata is destroyed when neccessary

#include <boost/tr1/memory.hpp>
#include <boost/tr1/functional.hpp>
#include <luacppinterface.h>

int x = 0;

class SomeClass
{
	int number;
public:
	SomeClass(int number)
	{
		this->number = number;
	}
	
	~SomeClass()
	{
		x = 100;
	}
	
	int GetNumber() const
	{
		return number;
	}
};

int main()
{
	Lua lua;
	lua.LoadStandardLibraries();
	
	auto global = lua.GetGlobalEnvironment();
	
	auto newSomeClass = lua.CreateFunction< LuaUserdata<SomeClass>(int) >(
	[&](int num) -> LuaUserdata<SomeClass>
	{
		auto sc = new SomeClass(num);
		auto userData = lua.CreateUserdata<SomeClass>(sc);
		
		using namespace std::tr1::placeholders;
		userData.Set("GetNumber", lua.CreateFunction< int() >( std::tr1::bind(&SomeClass::GetNumber, sc) ));
	
		return userData;
	});

	auto someClassStatic = lua.CreateTable();
	someClassStatic.Set("new", newSomeClass);

	global.Set("SomeClass", someClassStatic);
	
	lua.RunScript(
	"someclass = SomeClass:new(2)\n"
	"someclass = nil\n"
	"collectgarbage()\n"
	);
	
	return 100 - x;
}
