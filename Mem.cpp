int Mem::m_BaseAddress = 0;
Process *Mem::m_Process;
void* Mem::m_MainWindowsHandle;

bool Mem::Initialize(const std::wstring &ProcessName, std::wstring &windowsTitle)
{
	if (Process::GetProcessesByName(ProcessName).size() > 0)
	{
		m_Process = Process::GetProcessesByName(ProcessName)[0];
		m_BaseAddress = Process::GetProcessesByName(ProcessName)[0]->MainModule->BaseAddress->ToInt32;
		m_MainWindowsHandle = m_Process->MainWindowHandle;

		m_pProcessHandle = OpenProcess(56, false, m_Process->Id);
		windowsTitle = m_Process->MainWindowTitle;
		return true;
	}
	return false;
}

bool <missing_class_definition>::Inject(const std::wstring &dllPath)
{
	if (m_pProcessHandle == 0)
	{
		return false;
	}
	std::vector<unsigned char> dllBytes = Encoding::ASCII->GetBytes(dllPath);
	int allocAddress = VirtualAllocEx(m_pProcessHandle, 0, dllBytes.size(), MEM_COMMIT, PAGE_READWRITE);
	if (allocAddress == 0)
	{
		return false;
	}

	int kernelMod = GetModuleHandle(L"kernel32.dll");
	auto loadLibAddr = GetProcAddress(kernelMod, L"LoadLibraryA");

	if (kernelMod == 0 || loadLibAddr == 0)
	{
		return false;
	}

	WriteProcessMemory(m_pProcessHandle, allocAddress, dllBytes, dllBytes.size(), 0);
	int libThread = CreateRemoteThread(m_pProcessHandle, 0, 0, loadLibAddr, allocAddress, 0, 0);

	if (libThread == 0)
	{
		return false;
	}
	else
	{
		WaitForSingleObject(libThread, 5000);
		CloseHandle(libThread);
	}

	VirtualFreeEx(m_pProcessHandle, allocAddress, dllBytes.size(), MEM_RELEASE);
	//CloseHandle(m_pProcessHandle)

	return true;
}

std::vector<float> <missing_class_definition>::ConvertToFloatArray(std::vector<unsigned char> &bytes)
{
	if ((bytes.size() % 4) > 0)
	{
		//VB TO C++ CONVERTER TODO TASK: This exception's constructor requires an argument:
		//ORIGINAL LINE: Throw New ArgumentException
		throw std::invalid_argument();
	}
	std::vector<float> numArray(static_cast<int>(bytes.size() / 4));
	for (int i = 0; i < numArray.size(); i++)
	{
		numArray[i] = BitConverter::ToSingle(bytes, (i * 4));
	}

	return numArray;
}

//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
//ORIGINAL LINE: template<typename T> where T: Structure
template<typename T>
std::vector<float> <missing_class_definition>::ReadMatrix(int Adress, int MatrixSize)
{
	std::vector<unsigned char> buffer((Marshal::SizeOf(T::typeid) * MatrixSize));
	ReadProcessMemory(static_cast<int>(m_pProcessHandle), Adress, buffer, buffer.size(), m_iNumberOfBytesRead);
	return Mem::ConvertToFloatArray(buffer);
}

//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
//ORIGINAL LINE: template<typename T> where T: Structure
template<typename T>
T <missing_class_definition>::ByteArrayToStructure(std::vector<unsigned char> &bytes)
{
	T local = nullptr;
	GCHandle *handle = GCHandle::Alloc(bytes, GCHandleType::Pinned);
	try
	{
		local = static_cast<T>(Marshal::PtrToStructure(handle->AddrOfPinnedObject, T::typeid));
	}
	//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to the exception 'finally' clause:
	finally
	{
		handle->Free();
	}
	return local;
}

void *<missing_class_definition>::StructureToByteArray(void *obj)
{
	int length = Marshal::SizeOf(obj);
	std::vector<unsigned char> dest(length + 1);
	void* num = Marshal::AllocHGlobal(length);
	Marshal::StructureToPtr(obj, num, true);
	Marshal::Copy(num, dest, 0, length);
	Marshal::FreeHGlobal(num);
	return dest;
}

//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
//ORIGINAL LINE: template<typename T> where T: Structure
template<typename T>
T <missing_class_definition>::GetEntry(int baseEntry, int entryId, int &entryAddr)
{
	entryAddr = ReadMemory<int>(baseEntry + (0xC * entryId) + 0x8);
	return ReadMemory<T>(entryAddr + 0x8);
}

//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
//ORIGINAL LINE: template<typename T> where T: Structure
template<typename T>
T <missing_class_definition>::GetEntryS(int baseEntry, int entryId, int &entryAddr)
{
	entryAddr = ReadMemory<int>(baseEntry + entryId);
	return ReadMemory<T>(entryAddr + 0x8);
}

int <missing_class_definition>::GetEntryAddr(int baseEntry, int entryId)
{
	return ReadMemory<int>(baseEntry + (0xC * entryId) + 0x8);
}

int <missing_class_definition>::m_iNumberOfBytesRead = 0;
void* <missing_class_definition>::m_pProcessHandle;

//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
//ORIGINAL LINE: template<typename T> where T: Structure
template<typename T>
T <missing_class_definition>::ReadMemory(int Adress)
{
	std::vector<unsigned char> buffer(Marshal::SizeOf(T::typeid));
	ReadProcessMemory(m_pProcessHandle.ToInt32(), Adress, buffer, buffer.size(), m_iNumberOfBytesRead);
	return ByteArrayToStructure<T>(buffer);
}

//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
//ORIGINAL LINE: template<typename T> where T: Structure
template<typename T>
T <missing_class_definition>::WriteMemory(int Adress, void *write)
{

	std::vector<unsigned char> buffer = StructureToByteArray(write);
	WriteProcessMemory(m_pProcessHandle.ToInt32(), Adress, buffer, buffer.size(), m_iNumberOfBytesRead);
	return ByteArrayToStructure<T>(buffer);
}

void *<missing_class_definition>::WriteFloatMem(int Adress, float write)
{
	int tempVar = 0;
	WriteFloatMemory(m_pProcessHandle.ToInt32(), Adress, write, 4, tempVar);
	//VB TO C++ CONVERTER NOTE: Inserted the following 'return' since all code paths must return a value in C++:
	return nullptr;
}

std::vector<unsigned char> <missing_class_definition>::buffer;

std::wstring <missing_class_definition>::ReadString(int address, int _Size)
{
	//Try
	if (_Size > 0 && _Size < 256)
	{
		buffer = std::vector<unsigned char>(_Size);
		ReadProcessMemory(Mem::m_pProcessHandle::ToInt32, address, buffer, _Size, m_iNumberOfBytesRead);
		return Encoding::ASCII->GetString(buffer);
	}
	return L"";

	//Catch ex As Exception
	//Console.WriteLine(ex.ToString)
	//End Try

}

std::vector<unsigned char> <missing_class_definition>::ReadBytes(int address, int _Size)
{
	if (_Size > 0 && _Size < 20)
	{
		buffer = std::vector<unsigned char>(_Size);
		ReadProcessMemory(Mem::m_pProcessHandle::ToInt32, address, buffer, _Size, m_iNumberOfBytesRead);
		return buffer;
	}
	return std::vector<unsigned char>();
}
