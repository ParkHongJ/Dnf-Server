// Client.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Client.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "../Default/ServerPacketHandler.h"
#include <thread>
#include <iostream>
#include <mutex>
#include "ServerManager.h"
#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE g_hInst;                                // ���� �ν��Ͻ��Դϴ�.
HWND g_hWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
#define PACKET_SIZE 512

using namespace std;
extern bool g_ThreadLoop;

extern 
// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.
    CMainApp* pMainApp = nullptr;

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;

    pMainApp = CMainApp::Create();
    if (nullptr == pMainApp)
        return FALSE;


    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Default"))))
        return FALSE;
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_60"))))
        return FALSE;

    _float		fTimeAcc = 0.f;


    WSADATA wsaData;
    SOCKET clientSocket;
    SOCKADDR_IN serverAddress;
    int serverPort = 7778;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // Winsock���ʱ�ȭ�մϴ�.
    {
        // ShowErrorMessage("WSAStartup()");
        int a = 10;
    }
    clientSocket = socket(PF_INET, SOCK_STREAM, 0); // TCP �����������մϴ�.
    if (clientSocket == INVALID_SOCKET)
    {
        //ShowErrorMessage("socket()");
        int a = 10;
    }
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // ���ڿ�IP����Ʈ��ũ����Ʈ��������
    serverAddress.sin_port = htons(serverPort); // 2����Ʈ������Ʈ��ũ����Ʈ��������

    if (connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
		//ShowErrorMessage("connect()");
		cout << "Error" << endl;
	}
	{
		BYTE packet[PACKET_SIZE] = "";

		PacketHeader header;

		header.protocol = PKT_C_LOGIN;
		header.size = PACKET_SIZE;
		(*(PacketHeader*)packet) = header;

		send(clientSocket, (const char*)packet, PACKET_SIZE, 0);
	}
	bool g_ThreadLoop = true;

	ServerPacketHandler::Init();

	ServerManager* pServerManager = ServerManager::GetInstance();
	pServerManager->Initialize(clientSocket);

	thread WorkerThread = thread([=, &g_ThreadLoop]()
		{
			while (true)
			{
                if (!g_ThreadLoop)
                    break;

                BYTE packet[PACKET_SIZE] = "";
                int length = recv(clientSocket, (char*)packet, sizeof(packet), 0);

                pServerManager->AddPacket(packet);
			}
		});


    // �⺻ �޽��� �����Դϴ�.
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		pGameInstance->Update_Timer(TEXT("Timer_Default"));

		fTimeAcc += pGameInstance->Get_TimeDelta(TEXT("Timer_Default"));

		if (fTimeAcc >= /*1.0f / 60.0f*/0.0f)
		{
            pServerManager->ProcessPacket();
			pGameInstance->Update_Timer(TEXT("Timer_60"));
			pMainApp->Tick(pGameInstance->Get_TimeDelta(TEXT("Timer_60")));
			pMainApp->Render();

			fTimeAcc = 0.f;
		}		
	}

	RELEASE_INSTANCE(CGameInstance);

	Safe_Release(pMainApp);    

    BYTE packet[PACKET_SIZE] = "";

    PacketHeader header;

    header.protocol = PKT_C_EXIT;
    header.size = PACKET_SIZE;
    (*(PacketHeader*)packet) = header;

    send(clientSocket, (const char*)packet, PACKET_SIZE, 0);

    g_ThreadLoop = false;

    if (WorkerThread.joinable())
    {
        WorkerThread.join();
    }    
    closesocket(clientSocket);
    WSACleanup();

    pServerManager->Destroy();

    return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   RECT		rcWindow = { 0, 0, g_iWinSizeX, g_iWinSizeY };

   AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, TRUE);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   g_hWnd = hWnd;

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
