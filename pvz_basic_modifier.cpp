#include<windows.h>
#include<TlHelp32.h>
#include<stdio.h> 

DWORD dwPid;
HANDLE hTarget;

/*
	CreateToolhelp32Snapshot
	Process32First,Process32Next
	(OpenProcess)
*/
BOOL GetProcessPidByName(char *szProcessName, DWORD *dwPid){
	HANDLE l_hProcessSnapshot;
	l_hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 l_pe32;
	l_pe32.dwSize = sizeof(PROCESSENTRY32);
	if(l_hProcessSnapshot==INVALID_HANDLE_VALUE){
		return FALSE;
	}
	
	if(Process32First(l_hProcessSnapshot, &l_pe32)==0){
		return FALSE;
	}
	do{
		if(!strcmp(szProcessName, l_pe32.szExeFile)){
			*dwPid = l_pe32.th32ProcessID;
			break;
		}
//		printf("current read pn: %s | pid:%d\n", l_pe32.szExeFile, l_pe32.th32ProcessID);
	}while(Process32Next(l_hProcessSnapshot, &l_pe32));
	
	CloseHandle(l_hProcessSnapshot);
	return TRUE;
}

BOOL PatchGame(LPVOID lpAddr, BYTE* bBuff, DWORD dwSize){
	//LPVOID == void* 
	LPVOID lpBuff = (LPVOID)bBuff;
	DWORD dwOldProtect;
	VirtualProtectEx(hTarget, lpAddr, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	BOOL result  = WriteProcessMemory(
		hTarget
		, lpAddr, lpBuff 
		, dwSize, NULL
	);
	VirtualProtectEx(hTarget, lpAddr, dwSize, dwOldProtect, &dwOldProtect);
	
	if(result==0){
		printf("[-] ����ʧ�ܣ�������룺%d\n", GetLastError());
		return FALSE;
	}
	puts("[+] �����ɹ�����ȥ��Ϸ�￴���ɣ�");
	return TRUE;
}

void Menu(){
	puts("[+] ��ӭ����p0iL��pvz�޸���0.0.1�˵�");
	puts("- ��ʾ�˵���0");
	puts("- �����޸ģ���ֲ��������⣺1");
	puts("- �����޸ģ��������ⳬ���ӱ���2");
	puts("- ��Ƭ�޸ģ���Ƭ˲��ָ���3");
	puts("[+] �������Ӧ���ֻس�ִ�ж�Ӧ����");
}

int main(){
	if(GetProcessPidByName((char *)"popcapgame1.exe", &dwPid)==FALSE){
		puts("[-] can not found Pid");
		return 0;
	}
	
	hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if(hTarget==NULL){
		puts("[-] can not open this process"); 
		return 0;
	}
	
	Menu();
	while(TRUE){
		char readin;scanf("%c", &readin);
		switch(readin){
			case '0':
				Menu();
				break;
			case '1':{
				BYTE bBuff[] = {
					0x90, 0x90
				};
				PatchGame((LPVOID)0x0041BA74, bBuff, (DWORD)sizeof(bBuff));
				break;
			}
			case '2':{
				BYTE bBuff[] = {
					0x01, 0xA0, 0x60, 0x55, 0x00, 0x00
				};
				PatchGame((LPVOID)0x00430A11, bBuff, (DWORD)sizeof(bBuff));
				break;
			}
			case '3':{
				BYTE bBuff[] = {
					0x90, 0x90
				};
				PatchGame((LPVOID)0x00487296, bBuff, (DWORD)sizeof(bBuff));
				break;
			}
			default:
				puts("[-] ������󣬿�������0�鿴�˵�"); 
				break;
		}
	}
 	return 0;
}
