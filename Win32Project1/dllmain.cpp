// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <stdlib.h>
#include <vector>


using namespace std;

Checkmate* ck=NULL;
HINSTANCE dialogINST;
DWORD retVar;
DWORD addrOfSpeed=0;
DWORD addofMov=0;

vector<vector<int>> saveLocations;


int saveLoc1 = 0;
int saveLoc2 = 0;
int saveLoc3 = 0;

	void* Addr=NULL;
bool hasPatched=false;

void Run();
void __cdecl  main();
extern "C" __declspec( dllexport ) int  SpeedHack();
extern "C" __declspec( dllexport ) int  Teleport();

bool UnPatchTeleportHack();
bool PatchTeleport();

void* SearchMemory(char* pattern, char* fromAddress, char* toAddress, int size);
bool Patch();
bool UnPatch();

INT_PTR CALLBACK FormCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	DisableThreadLibraryCalls(hModule);

	dialogINST=hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

	case DLL_THREAD_ATTACH:
	{
			ck = new Checkmate();
			ck->dialogINST = hModule;
			//Patch();
			CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(main),NULL,NULL,NULL);
			break;
	}
	case DLL_THREAD_DETACH:
	{
		delete ck;
		ck=NULL;
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void Run()
{

		DialogBox(dialogINST,MAKEINTRESOURCE(IDD_DIALOG1),NULL,FormCallback);

		while (1)
		{

		}


}

INT_PTR CALLBACK FormCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lParam);



	switch (message)
	{
		case WM_INITDIALOG:
		{
			
			hasPatched=false;
			 
		
			break;
		}

		case WM_COMMAND:
		{
			if (LOWORD(wParam) ==IDC_BUTTON1)
			{
				char test[255];
				GetDlgItemTextA(hDlg,IDC_EDIT1,test,25);

				

				double cspeed = atof(test);


				if (hasPatched == false )
				{
					Patch();
					__asm
					{
						here: mov eax,addrOfSpeed
						cmp eax, 0x00
						je here
					}

		
					UnPatch();
					hasPatched = true;
					MessageBoxA(NULL,"Patching Done",test,MB_OK);
				} 
				if (hasPatched == true)
				{
					float fullSpeed = (float) cspeed;
					__asm {
						mov eax, addrOfSpeed
						mov ecx, [fullSpeed]
						mov [eax],ecx
					}
				}
				
			}
			//save location
			if (LOWORD(wParam) == IDC_BUTTON2)
			{
				//SendMessageA(GetDlgItem(hDlg, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)"hello wprld");
				if (addofMov != 0)
				{
					//save the first part 
					_asm
					{
						mov eax, addofMov
						add eax,0x124
						mov edx, [eax]
						mov [saveLoc1], edx
					}
					//save the second part 
					_asm
					{
						mov eax, addofMov
						add eax,0x11B
						mov edx, [eax]
						mov [saveLoc2], edx
					}
					//save the third part 
					_asm
					{
						mov eax, addofMov
						add eax,0x113
						mov edx, [eax]
						mov [saveLoc3], edx
					}

					
					vector<int> cords;
					cords.push_back(saveLoc1);
					cords.push_back(saveLoc2);
					cords.push_back(saveLoc3);

					saveLocations.push_back(cords);
					
					char display[32*3];
					sprintf_s(display,"%i,%i,%i",saveLoc1,saveLoc2,saveLoc3);
					SendMessageA(GetDlgItem(hDlg, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)display);


					

				}
			}
			//warp to
			if (LOWORD(wParam) == IDC_BUTTON3)
			{
				if (addofMov != 0)
				{
					int sel=-1;

					sel = SendDlgItemMessage( hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0 );

					if (sel == -1)
					{
						MessageBoxA(NULL, "You must select something from the listbox", "Error",MB_OK);
						return false;
					}

					vector<int> getSelection = saveLocations[sel];

					saveLoc1 = getSelection[0];
					saveLoc2 = getSelection[1];
					saveLoc3 = getSelection[2];

					_asm
					{
						mov edx, [saveLoc1]
						mov eax, addofMov
						add eax, 0x124
						mov [eax], edx
					}
					_asm
					{
						mov edx, [saveLoc2]
						mov eax, addofMov
						add eax, 0x11B
						mov [eax], edx
					}
					_asm
					{
						mov edx, [saveLoc3]
						mov eax, addofMov
						add eax, 0x113
						mov [eax], edx
					}
				}
			}
			break;
		}
	}
	return (INT_PTR)FALSE;
}

void __cdecl main()
{
	PatchTeleport();
	CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Run),NULL,NULL,NULL);
}

extern "C" __declspec( dllexport ) int  Teleport()
{
	_asm
	{
		//push ecx
		//mov ecx,ebx
		mov addofMov, ebx
		//pop ecx
		MOV DWORD PTR DS:[EBX+0x124],EDX
		FSUB QWORD PTR DS:[ESI+8]
	}
}

extern "C" __declspec( dllexport ) int  SpeedHack()
{
	_asm
	{
		push ebp
		mov ebp,esp
		push ecx
		MOV DWORD PTR SS:[EBP-4],ECX
		MOV EAX,DWORD PTR SS:[EBP-4]
		FLD DWORD PTR DS:[EAX+0x144]
		PUSH ecx
		mov ecx,eax
		add ecx, 0x144
		mov retVar,eax
		mov [addrOfSpeed], ecx
		pop ecx
		mov esp, ebp
		pop ebp

	}

	return retVar;
}

void* SearchMemory(char* pattern, char* fromAddress, char* toAddress, int size)
{
	bool didMatchAll=false;
	int k=0;
	int l=0;

	for (char* i= fromAddress; i < toAddress;i++)
	{
		k=0;
		l=0;
		
		for (char* j=pattern;j<pattern+size;j++)
		{
			
			if ((BYTE) *(i+k) == (BYTE) *j)
			{
				l++;
			} else {
				break;
			}
		
			k++;
		}
		
		if (size==l)
		{
			return i;
		} 
	}
	
	return NULL;


}

bool Patch()
{
			HANDLE result=NULL;
			HANDLE cProcess =GetCurrentProcess();
			MODULEINFO mi;
			DWORD dwProtect;
	

			

			result = GetModuleHandleA("Rag2.exe");
			GetModuleInformation(cProcess, (HMODULE) result, &mi, sizeof(mi));

			VirtualProtect(result,(SIZE_T) mi.SizeOfImage, PAGE_EXECUTE_READWRITE, &dwProtect);
			Addr = SearchMemory("\x55\x8B\xEC\x51\x89\x4D\xFC\x8B\x45\xFC\xD9\x80\x44\x01\x00\x00",(char*)result, (char*)result+mi.SizeOfImage,16);
			if (Addr != NULL)
			{
				
				__asm 
				{
					mov eax, Addr
					mov edx,0

					clear: mov [eax],0x90
					inc eax
					inc edx
					cmp edx,12
					jne clear
				}



				HMODULE ourDLL = GetModuleHandleA("CheckmateHS.dll");
				FARPROC speedHack = GetProcAddress( ourDLL, "SpeedHack");
				_asm
				{	
					mov eax,Addr
					mov [eax],0x52
				}

				_asm
				{	
					mov eax,Addr
					add eax,0x01
					mov [eax],0xBA
				}
				unsigned int breakDown = (unsigned int) speedHack & 0x000000FF;
				breakDown >>= 0;

				_asm
				{
					mov eax,Addr
					add eax,0x02
					mov ecx, [breakDown]
					mov [eax],ecx
				}

				breakDown = (unsigned int) speedHack & 0x0000FF00;
				breakDown >>= 8;
				_asm
				{
					mov eax,Addr
					add eax,0x03
					mov ecx, [breakDown]
					mov [eax],ecx
				}

				breakDown = (unsigned int) speedHack & 0x00FF0000;
				breakDown >>= 16;
				_asm
				{
					mov eax,Addr
					add eax,0x04
					mov ecx, [breakDown]
					mov [eax],ecx
				}
				breakDown = (unsigned int) speedHack & 0xFF000000;
				breakDown >>= 24;
				_asm
				{
					mov eax,Addr
					add eax,0x05
					mov ecx, [breakDown]
					mov [eax],ecx
				}

				_asm
				{
					mov eax,Addr
					add eax,0x06
					mov [eax],0xFF
					inc eax
					mov [eax], 0xD2
					inc eax
					mov [eax], 0x5A
					inc eax
					mov [eax],0x90
					inc eax
					mov [eax],0x90
					inc eax
					mov [eax],0x90
					inc eax
					mov [eax],0x90
					inc eax
					mov [eax],0x90
					inc eax
					mov [eax],0x90
					inc eax
					mov [eax],0x90
					inc eax
					mov [eax],0x90
					inc eax
					mov [eax],0x90
					inc eax
					mov [eax],0x90
				}

			} else {
				MessageBoxA(NULL,"Not found!", "Not found!", MB_OK);
				return false;
			}
		return true;
}			

bool UnPatch()
{
	__asm 
	{
		mov eax, Addr
		mov edx,0

		clear: mov [eax],0x90
		inc eax
		inc edx
		cmp edx,16
		jne clear
	}
	_asm
	{	
		mov eax,Addr
		mov [eax],0x55
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x01
			mov [eax],0x8B
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x02
			mov [eax],0xEC
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x03
			mov [eax],0x51
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x04
			mov [eax],0x89
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x05
			mov [eax],0x4D
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x06
			mov [eax],0xFC
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x07
			mov [eax],0x8B
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x08
			mov [eax],0x45
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x09
			mov [eax],0xFC
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x0A
			mov [eax],0xD9
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x0B
			mov [eax],0x80
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x0C
			mov [eax],0x44
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x0D
			mov [eax],0x01
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x0E
			mov [eax],0x00
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x0F
			mov [eax],0x00
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x10
			mov [eax],0x8B
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x11
			mov [eax],0xE5
	}
	_asm
	{	
			mov eax,Addr
			add eax,0x12
			mov [eax],0x5D
	}
}

bool PatchTeleport()
{
			HANDLE result=NULL;
			HANDLE cProcess =GetCurrentProcess();
			MODULEINFO mi;
			DWORD dwProtect;
			void* TeleportAddr=NULL;
	
			result = GetModuleHandleA("NxCharacter.dll");
			GetModuleInformation(cProcess, (HMODULE) result, &mi, sizeof(mi));

			VirtualProtect(result,(SIZE_T) mi.SizeOfImage, PAGE_EXECUTE_READWRITE, &dwProtect);
			
			TeleportAddr = SearchMemory("\x89\x93\x24\x01\x00\x00\xDC\x66\x08",(char*)result, (char*)result+mi.SizeOfImage,9);
			if (TeleportAddr != NULL)
			{
				__asm 
				{
					mov eax, TeleportAddr
					mov edx,0

					clear: mov [eax],0x90
					inc eax
					inc edx
					cmp edx,9
					jne clear
				}
			}

			HMODULE ourDLL = GetModuleHandleA("CheckmateHS.dll");
			FARPROC teleport = GetProcAddress( ourDLL, "Teleport");
			_asm
			{	
				mov eax,TeleportAddr
				mov [eax],0x50
			}

			_asm
			{	
				mov eax,TeleportAddr
				add eax,0x01
				mov [eax],0xB8
			}
			unsigned int breakDown = (unsigned int) teleport & 0x000000FF;
			breakDown >>= 0;

			_asm
			{
				mov eax,TeleportAddr
				add eax,0x02
				mov ecx, [breakDown]
				mov [eax],ecx
			}

			breakDown = (unsigned int) teleport & 0x0000FF00;
			breakDown >>= 8;
			_asm
			{
				mov eax,TeleportAddr
				add eax,0x03
				mov ecx, [breakDown]
				mov [eax],ecx
			}

			breakDown = (unsigned int) teleport & 0x00FF0000;
			breakDown >>= 16;
			_asm
			{
				mov eax,TeleportAddr
				add eax,0x04
				mov ecx, [breakDown]
				mov [eax],ecx
			}
			breakDown = (unsigned int) teleport & 0xFF000000;
			breakDown >>= 24;
			_asm
			{
				mov eax,TeleportAddr
				add eax,0x05
				mov ecx, [breakDown]
				mov [eax],ecx
			}
			_asm
			{
				mov eax,TeleportAddr
				add eax,0x06
				mov [eax],0xFF
				inc eax
				mov [eax], 0xD0
				inc eax
				mov [eax], 0x58
			}
			return true;
}

bool UnPatchTeleportHack()
{
}