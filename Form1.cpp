//VB TO C++ CONVERTER TODO TASK: There is no direct equivalent to 'Handles' in native C++:
//ORIGINAL LINE: Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
void Form1::Form1_Load(void *sender, EventArgs *e)
{
	if (Mem->Initialize(L"ros", windowsTitle))
	{
		GetWindowRect(Mem::m_MainWindowsHandle, windowsRect);

		this->Top = windowsRect.top;
		this->Left = windowsRect.left;
		this->Width = windowsRect.right - windowsRect.left;
		this->Height = windowsRect.bottom - windowsRect.top;
		SetWindowLong(this->Handle, static_cast<int>(GWL::GWL_EXSTYLE), static_cast<int>(WS::WS_EX_LAYERED | WS::WS_EX_TRANSPARENT));
		ThreadStart tempVar(GameInfo);
		auto gameInfoThread = new Thread(&tempVar);
		gameInfoThread->IsBackground = true;
		gameInfoThread->Start();

	}
	else
	{
		MsgBox(L"Please start game first!");
		Application::Exit();
	}

}

//VB TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in native C++:
//ORIGINAL LINE: Public Shared Function WorldToScreen(ByVal _viewMatrix As Single(), ByVal pos As Vect3, <Out> ByRef screen As Vect2, ByVal windowWidth As Integer, ByVal windowHeight As Integer) As Boolean
bool Form1::WorldToScreen(std::vector<float> &_viewMatrix, Vect3 *pos, Vect2 *&screen, int windowWidth, int windowHeight)
{
	screen = new Vect2();

	Vect4 *clipCoords = new Vect4 With{ .X = static_cast<float>(static_cast<double>(pos->X) * static_cast<double>(_viewMatrix[0]) + (static_cast<double>(pos->Y) * static_cast<double>(_viewMatrix[4])) + (static_cast<double>(pos->Z) * static_cast<double>(_viewMatrix[8]))) + static_cast<double>(_viewMatrix[12]),.Y = static_cast<float>(static_cast<double>(pos->X) * static_cast<double>(_viewMatrix[1]) + (static_cast<double>(pos->Y) * static_cast<double>(_viewMatrix[5])) + (static_cast<double>(pos->Z) * static_cast<double>(_viewMatrix[9]))) + static_cast<double>(_viewMatrix[13]),.Z = static_cast<float>(static_cast<double>(pos->X) * static_cast<double>(_viewMatrix[2]) + (static_cast<double>(pos->Y) * static_cast<double>(_viewMatrix[6])) + (static_cast<double>(pos->Z) * static_cast<double>(_viewMatrix[10]))) + static_cast<double>(_viewMatrix[14]),.W = static_cast<float>(static_cast<double>(pos->X) * static_cast<double>(_viewMatrix[3]) + (static_cast<double>(pos->Y) * static_cast<double>(_viewMatrix[7])) + (static_cast<double>(pos->Z) * static_cast<double>(_viewMatrix[11]))) + static_cast<double>(_viewMatrix[15]) };
	if (static_cast<double>(clipCoords->W) < 0.100000001490116)
	{
		return false;
	}

	Vect3 *NDC = nullptr;
	NDC->X = (clipCoords->X / clipCoords->W);
	NDC->Y = (clipCoords->Y / clipCoords->W);
	NDC->Z = (clipCoords->Z / clipCoords->W);

	screen->X = static_cast<float>((static_cast<double>(windowWidth / 2.0) * static_cast<double>(NDC->X)) + (static_cast<double>(NDC->X) + static_cast<double>(windowWidth / 2.0)));
	screen->Y = static_cast<float>(-(static_cast<double>(windowHeight / 2.0) * static_cast<double>(NDC->Y)) + (static_cast<double>(NDC->Y) + static_cast<double>(windowHeight / 2.0)));
	return true;
}
void <missing_class_definition>::GameInfo()
{
	auto overlay = new OverlayWindow(windowsRect::left + 7, windowsRect::top, this->Width, this->Height);
	auto rendererOptions = new Direct2DRendererOptions() With{ .AntiAliasing = true,.Hwnd = overlay->WindowHandle,.MeasureFps = true,.VSync = false };
	auto d2d = new Direct2DRenderer(rendererOptions);
	auto whiteSmoke = d2d->CreateBrush(0, 0, 0, 0);
	auto blackBrush = d2d->CreateBrush(0, 0, 0, 255);
	auto redBrush = d2d->CreateBrush(255, 0, 0, 255);
	auto greenBrush = d2d->CreateBrush(0, 255, 0, 255);
	auto blueBrush = d2d->CreateBrush(0, 0, 255, 255);
	auto font = d2d->CreateFont(L"Consolas", 10);

	auto m_pWorld = Mem::ReadMemory<int>(Mem::m_BaseAddress + PyGame + 0x410);
	auto m_pSceneContext = Mem::ReadMemory<int>(m_pWorld + 0x8);
	auto cameraBase = Mem::ReadMemory<int>(m_pSceneContext + 0x4);

	//VB TO C++ CONVERTER NOTE: There is no C++ equivalent to VB's implicit 'once only' variable initialization within loops, so the following variable declaration has been placed prior to the loop:
	Vect2 *screen = nullptr;
	//VB TO C++ CONVERTER NOTE: There is no C++ equivalent to VB's implicit 'once only' variable initialization within loops, so the following variable declaration has been placed prior to the loop:
	Vect3 *position = nullptr;
	while (true)
	{
		d2d->BeginScene();
		d2d->ClearScene();
		auto viewMatrix = Mem::ReadMatrix<float>(cameraBase + 0xC4, 16);
		int visibleCount = Mem::ReadMemory<int>(m_pWorld + 0x278); //As CModelFactory->CModelSkeletal 0x28 '0x27c
		int pLocalModel = Mem::ReadMemory<int>(m_pWorld + 0x27C);
		//d2d.DrawText(visibleCount.ToString, 50, 300, font, greenBrush)
		//Dim pLocalModel_m_pAnimator As Integer = Mem.ReadMemory(Of Integer)(pLocalModel + &H328) 'CModelSkeletal->WorldAnimator
		//Dim pLocalModel_m_Transform As Single() = Mem.ReadMatrix(Of Single)(pLocalModel + &H3B0, 16) 'CModelSkeletal->D3DXMATRIX
		//Dim pLocalModel_SpaceNode As Integer = Mem.ReadMemory(Of Integer)(pLocalModel + &H1C)
		int pSkeletonList = Mem::ReadMemory<int>(m_pWorld + 0x290); //As CModelFactory->CModelSkeletal 0x3C 'pSkeletonList 0x28c
		static bool printed = false;
		if (!printed)
		{
			printed = true;
			std::wcout << L"World: 0x" << std::hex << std::setw(0) << std::uppercase << m_pWorld << std::dec << std::nouppercase << std::endl;
		}
		//			Dim screen As Vect2

		for (int i = 0; i <= visibleCount; i += 4)
		{
			int r_pModel = Mem::ReadMemory<int>(pSkeletonList + i);

			int SpaceNode = Mem::ReadMemory<int>(r_pModel + 0x1C);
			int m_pAnimator = Mem::ReadMemory<int>(r_pModel + 0x328); //CModelSkeletal->WorldAnimator
			std::vector<float> m_Position = Mem::ReadMatrix<float>(r_pModel + 0x3B0, 16);

			std::wstring m_pModelName = Mem::ReadString(Mem::ReadMemory<int>(m_pAnimator + 0x528), 30);
			std::wstring typeName = L"Unknow";
			bool isPlayer = false;
			if (m_pModelName.find(L"dataosha_male") != std::wstring::npos) //dataosha_male, dataosha_femal
			{
				typeName = L"Player";
				isPlayer = true;
			}
			else if (m_pModelName.find(L"dataosha_female") != std::wstring::npos) //vehicle
			{
				typeName = L"Player";
				isPlayer = true;
				//Continue For
			}
			else if (m_pModelName.find(L"roadster") != std::wstring::npos) //vehicle
			{
				//Continue For
				typeName = L"Car";
			}
			else if (m_pModelName.find(L"jeep") != std::wstring::npos) //item\miz
			{
				typeName = L"Jeep";
				//Continue For
			}
			else if (m_pModelName.find(L"door") != std::wstring::npos) //item\dts
			{
				continue;
			}
			else if (m_pModelName.find(L"mizang") != std::wstring::npos) //Heritage
			{
				typeName = L"Her";
				//Continue For
			}
			else if (m_pModelName.find(L"halei") != std::wstring::npos) //Motor
			{
				typeName = L"Motor";
				//Continue For
			}
			else if (m_pModelName.find(L"buggy") != std::wstring::npos) //Buggy
			{
				typeName = L"Car";
				//Continue For
			}
			else if (m_pModelName.find(L"express") != std::wstring::npos)
			{
				//typeName = m_pModelName & ", " & Hex(pSkeletonList + i)
				typeName = L"Express";
				//Continue For
			}
			else if (m_pModelName.find(L"crusher") != std::wstring::npos)
			{
				typeName = L"Car";
				//Continue For
			}
			else if (m_pModelName.find(L"bike") != std::wstring::npos)
			{
				typeName = L"Bike";
				//Continue For
			}
			else if (m_pModelName.find(L"T$$") != std::wstring::npos)
			{
				continue;
			}
			else if (m_pModelName.find(L"plane") != std::wstring::npos)
			{
				typeName = L"Plane";
			}
			else if (m_pModelName.find(L"parachute") != std::wstring::npos)
			{
				typeName = L"Parachute";
				continue;
			}
			else
			{

			}

			//				Dim position As Vect3
			position->X = m_Position[12];
			position->Y = m_Position[13];
			position->Z = m_Position[14];

			//Dim vMax As Vect3 = Mem.ReadMemory(Of Vect3)(SpaceNode + &H1E0)
			//Dim vMin As Vect3 = Mem.ReadMemory(Of Vect3)(SpaceNode + &H1EC)

			if (WorldToScreen(viewMatrix, position, screen, this->Width, this->Height))
			{
				d2d->DrawText(typeName, screen->X, screen->Y, font, redBrush);
				if (isPlayer)
				{
					d2d->DrawLine(static_cast<float>(this->Width / 2.0), this->Height, screen->X, screen->Y, 1, greenBrush);
				}
			}
		}
		d2d->EndScene();


	}

}
