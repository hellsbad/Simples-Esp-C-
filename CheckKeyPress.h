#include <unordered_map>


enum class KeyState : int
{
	None = 0,
	Press = 1,
	Release = 2
};
class CheckKeyPress
{

	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	//[DllImport("User32.dll")]
public:
	__declspec(dllimport) static short GetAsyncKeyState(Keys *vKey);

	//Private isPressed As Boolean

private:
	std::unordered_map<Keys*, bool> registeredKey;

public:
	KeyState getKeyPress(Keys *key)
	{
		if (registeredKey.find(key) == registeredKey.end())
		{
			registeredKey.emplace(key, false);
		}
		if (GetAsyncKeyState(key) != 0)
		{

			if (!registeredKey[key])
			{
				registeredKey[key] = true;
				//Console.WriteLine("Pressed")
				return KeyState::Press;
			}

		}
		else if (registeredKey[key])
		{
			//Console.WriteLine("Release")
			registeredKey[key] = false;
			return KeyState::Release;
		}
		return KeyState::None;
	}

};
