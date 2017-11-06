#ifndef UNICODE
	#define UNICODE
#endif
#include<windows.h>
#include<stdio.h>
#include"mydialog.h"

// WndProc Prototype
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

// DlgProc Prototype
BOOL CALLBACK DlgProc(HWND,UINT,WPARAM,LPARAM);

// This functions converts Wide Character string to ANSI character string
void WidetoAscii(TCHAR *wStr,char* str)
{
	char Str[100];
	int numBytes;
	numBytes = WideCharToMultiByte(CP_UTF8,0,wStr,-1,Str,sizeof(Str),NULL,NULL);
	strcpy(str,Str);
}

// Opening a File
int FileOpen(char *fileNameA,HFILE *hFile, OFSTRUCT *ofStruct)
{
	*hFile = OpenFile(fileNameA,&(*ofStruct),OF_READ);
	if(*hFile == HFILE_ERROR)
	{
		return FO_FAILED;
	}
	return FO_SUCCESS;
}

// Count Number of characters in file hFile
DWORD countChars(HWND hDlg, HFILE hFile)
{
	BOOL rfReturn;
	DWORD count=0, numBytesRead;
	char readData[100];
	do
	{
		rfReturn = ReadFile((HANDLE) hFile,readData,100,&numBytesRead,NULL);
		if(rfReturn == TRUE)
		{
			count+=numBytesRead;
		}
		else
		{
			MessageBox(hDlg,TEXT("Unable to read data"),TEXT("Error"),MB_OK);
		}
	}while(numBytesRead!=0);

	return count;
}

// Count number of words in file hFile
DWORD countWords(HWND hDlg, HFILE hFile)
{
	BOOL rfReturn;
	DWORD count=0;
	unsigned int i;
	BOOL flag=TRUE;
	char readData[100];
	DWORD numBytesRead;
	do
	{
		rfReturn = ReadFile((HANDLE) hFile,readData,100,&numBytesRead,NULL);
		if(rfReturn == TRUE)
		{
			if(numBytesRead == 0)
			{
				count++;
			}
			else
			{
				for(i=0; i<numBytesRead; i++)
				{
					if(flag==FALSE)
					{
						if(readData[i] != ' ' || readData[i] != '\t' || readData[i] != '\n')
						{
							flag=TRUE;
							continue;
						}
					}
					else if(flag==TRUE)
					{
						if(readData[i] == ' ' || readData[i] == '\t' || readData[i] == '\n')
						{
							count++;
							flag=FALSE;
						}
					}
				}
			}
		}
		else
		{
			MessageBox(hDlg,TEXT("Unable to read data"),TEXT("Error"),MB_OK);
		}
	}while(numBytesRead!=0);

	return count;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	WNDCLASSEX wndclass;
	MSG msg;
	HWND hwnd;
	TCHAR AppName[] = TEXT("Windows");

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = AppName;
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(AppName, TEXT("Window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if(NULL == hwnd)
	{
		MessageBox(hwnd, TEXT("Window not created"),TEXT("Error"),MB_OK);
		return 0;
	}

	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInst;
	int dResult;
	TCHAR str[30] = TEXT("DATAENTRY");
	switch(iMsg)
	{
	case WM_CREATE:
		hInst = (HINSTANCE) GetWindowLong(hwnd,GWL_HINSTANCE);
		// Create dialog box using demo.rc resource script
		dResult = DialogBox(hInst,str,hwnd,DlgProc);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hwnd,iMsg,wParam,lParam));
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR fileName[100],demo[100];
	char fileNameA[100];
	static HFILE hFile;
	static OFSTRUCT ofStruct;
	int result;
	DWORD countOfChars=0,countOfWords=0;
	HDC hdc;

	switch(iMsg)
	{
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hDlg,ID_FNAME));
		return (TRUE);

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_OPENFILE:
			GetDlgItemText(hDlg,ID_FNAME,fileName,100);
			WidetoAscii(fileName,fileNameA);

			result = FileOpen(fileNameA,&hFile,&ofStruct);
			if(result == FO_SUCCESS)
			{
					TCHAR demo[100];
					wsprintf(demo,TEXT("File Opened Successfully : %s"),fileName);
					MessageBox(hDlg,demo,TEXT("FO_SUCCESS"),MB_OK);
			}
			else if(result == FO_FAILED)
			{
				MessageBox(hDlg,TEXT("Cannot open file. Please specify correct File Name."),TEXT("Error"),MB_OK);
				SetDlgItemText(hDlg,ID_FNAME,TEXT(""));
				SetFocus(GetDlgItem(hDlg,ID_FNAME));
			}
			return (TRUE);

		case ID_COUNTCHARS:
			countOfChars = countChars(hDlg,hFile);
			
			wsprintf(demo,TEXT("Total characters in file are %d"),countOfChars);
			hdc = GetDC(hDlg);
			TextOut(hdc,200,300,demo,lstrlen(demo));
			ReleaseDC(hDlg,hdc);

			return (TRUE);

		case ID_COUNTWORDS:
			countOfWords = countWords(hDlg,hFile);
			
			wsprintf(demo,TEXT("Total words in file are %d"),countOfWords);
			hdc = GetDC(hDlg);
			TextOut(hdc,200,300,demo,lstrlen(demo));
			ReleaseDC(hDlg,hdc);

			return (TRUE);

		case ID_CLOSE:
			CloseHandle((HANDLE) hFile);
			EndDialog(hDlg,0);
			PostQuitMessage(0);
			return (TRUE);
		}
		return (TRUE);
	}
	return(FALSE);
}