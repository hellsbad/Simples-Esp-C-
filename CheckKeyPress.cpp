KeyState CheckKeyPress::getKeyPress(Keys *key)
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