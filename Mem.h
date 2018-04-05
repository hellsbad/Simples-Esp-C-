#include <string>
#include <vector>
#include <stdexcept>
#include <type_traits>
class Mem
{

public:
	static int m_BaseAddress;
	static Process *m_Process;
	static void* m_MainWindowsHandle;

private:
	static constexpr int PROCESS_ALL_ACCESS = 0x1F0FFF;
	static constexpr int MEM_COMMIT = 0x1000;
	static constexpr int MEM_RELEASE = 0x8000;
	static constexpr int PAGE_READWRITE = 0x4;

	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	//[DllImport("kernel32.dll")]
public:
	__declspec(dllimport) static void* OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	//[DllImport("kernel32.dll")]
	__declspec(dllimport) static bool ReadProcessMemory(int hProcess, int lpBaseAddress, std::vector<unsigned char> &buffer, int size, int &lpNumberOfBytesRead);

	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	//[DllImport("kernel32.dll")]
	__declspec(dllimport) static bool WriteProcessMemory(int hProcess, int lpBaseAddress, std::vector<unsigned char> &buffer, int size, int &lpNumberOfBytesWritten);

	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
private:
	__declspec(dllimport) static int VirtualAllocEx(void* hProcess, void* lpAddress, unsigned int dwSize, unsigned int flAllocationType, unsigned int flProtect);
	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	__declspec(dllimport) static bool VirtualFreeEx(void* hProcess, void* lpAddress, unsigned int dwSize, int dwFreeType);
	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	__declspec(dllimport) static void* GetProcAddress(void* hModule, const std::wstring &lpProcName);
	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	__declspec(dllimport) static void* GetModuleHandle(const std::wstring &lpModuleName);
	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	__declspec(dllimport) static void* CreateRemoteThread(void* hProcess, void* lpThreadAttributes, unsigned int dwStackSize, void* lpStartAddress, void* lpParameter, int dwCreationFlags, void* lpThreadId);
	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	__declspec(dllimport) static int WaitForSingleObject(void* hHandle, int dwMilliseconds);
	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
	__declspec(dllimport) static bool CloseHandle(void* hObject);

public:
	static bool Initialize(const std::wstring &ProcessName, std::wstring &windowsTitle)
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



public:
	static bool Inject(const std::wstring &dllPath)
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

	static std::vector<float> ConvertToFloatArray(std::vector<unsigned char> &bytes)
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
	static std::vector<float> ReadMatrix(int Adress, int MatrixSize)
	{
		std::vector<unsigned char> buffer((Marshal::SizeOf(T::typeid) * MatrixSize));
		ReadProcessMemory(static_cast<int>(m_pProcessHandle), Adress, buffer, buffer.size(), m_iNumberOfBytesRead);
		return Mem::ConvertToFloatArray(buffer);
	}

private:
	//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
	//ORIGINAL LINE: template<typename T> where T: Structure
	template<typename T>
	static T ByteArrayToStructure(std::vector<unsigned char> &bytes)
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

	static void *StructureToByteArray(void *obj)
	{
		int length = Marshal::SizeOf(obj);
		std::vector<unsigned char> dest(length + 1);
		void* num = Marshal::AllocHGlobal(length);
		Marshal::StructureToPtr(obj, num, true);
		Marshal::Copy(num, dest, 0, length);
		Marshal::FreeHGlobal(num);
		return dest;
	}

public:
	//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
	//ORIGINAL LINE: template<typename T> where T: Structure
	template<typename T>
	static T GetEntry(int baseEntry, int entryId, int &entryAddr = 0)
	{
		entryAddr = ReadMemory<int>(baseEntry + (0xC * entryId) + 0x8);
		return ReadMemory<T>(entryAddr + 0x8);
	}

	//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
	//ORIGINAL LINE: template<typename T> where T: Structure
	template<typename T>
	static T GetEntryS(int baseEntry, int entryId, int &entryAddr = 0)
	{
		entryAddr = ReadMemory<int>(baseEntry + entryId);
		return ReadMemory<T>(entryAddr + 0x8);
	}

	static int GetEntryAddr(int baseEntry, int entryId)
	{
		return ReadMemory<int>(baseEntry + (0xC * entryId) + 0x8);
	}


	//Public Shared Function GetEntryMeta(ByVal baseEntry As Integer, ByVal entryId As Integer, Optional ByRef entryAddr As Integer = 0) As Integer
	//    entryAddr = ReadMemory(Of Integer)(baseEntry + (&HC * entryId) + &H8)
	//    Return ReadMemory(Of Integer)(entryAddr + &H4)
	//End Function


public:
	//VB TO C++ CONVERTER TODO TASK: Native C++ does not allow initialization of static non-const/integral fields in their declarations - choose the conversion option for separate .h and .cpp files:
	static int m_iNumberOfBytesRead = 0;
	static void* m_pProcessHandle;
	//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
	//ORIGINAL LINE: template<typename T> where T: Structure
	template<typename T>
	static T ReadMemory(int Adress)
	{
		std::vector<unsigned char> buffer(Marshal::SizeOf(T::typeid));
		ReadProcessMemory(m_pProcessHandle.ToInt32(), Adress, buffer, buffer.size(), m_iNumberOfBytesRead);
		return ByteArrayToStructure<T>(buffer);
	}

	//VB TO C++ CONVERTER TODO TASK: There is no native C++ template equivalent to the generic 'Structure' constraint:
	//ORIGINAL LINE: template<typename T> where T: Structure
	template<typename T>
	static T WriteMemory(int Adress, void *write)
	{

		std::vector<unsigned char> buffer = StructureToByteArray(write);
		WriteProcessMemory(m_pProcessHandle.ToInt32(), Adress, buffer, buffer.size(), m_iNumberOfBytesRead);
		return ByteArrayToStructure<T>(buffer);
	}

	//VB TO C++ CONVERTER NOTE: The following .NET DllImport attribute was converted using the Microsoft-specific __declspec(dllimport):
private:
	__declspec(dllimport) static int WriteFloatMemory(int hProcess, int lpBaseAddress, float &lpBuffer, int nSize, int &lpNumberOfBytesWritten);

public:
	static void *WriteFloatMem(int Adress, float write)
	{
		int tempVar = 0;
		WriteFloatMemory(m_pProcessHandle.ToInt32(), Adress, write, 4, tempVar);
		//VB TO C++ CONVERTER NOTE: Inserted the following 'return' since all code paths must return a value in C++:
		return nullptr;
	}

	static std::vector<unsigned char> buffer;
	static std::wstring ReadString(int address, int _Size)
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

	static std::vector<unsigned char> ReadBytes(int address, int _Size)
	{
		if (_Size > 0 && _Size < 20)
		{
			buffer = std::vector<unsigned char>(_Size);
			ReadProcessMemory(Mem::m_pProcessHandle::ToInt32, address, buffer, _Size, m_iNumberOfBytesRead);
			return buffer;
		}
		return std::vector<unsigned char>();
	}

};
