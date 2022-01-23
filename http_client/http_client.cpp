#include"../wongoose_lib/wongoose.h"
#include<stdio.h>
#pragma comment(lib,"../wongoose_lib/wongoose.lib")


static void fn(wg_connection* c, int ev, void* ev_data, void* fn_data)
{
	switch (ev)
	{
	case WG_EV_CLOSE:
		printf("disconnect a client\n");
		break;
	case WG_EV_CONNECT:
	{
		char addr[65] = { 0 };
		wg_ntoa(&c->addr, addr, 65);
		printf("connect to %s : %d\n", addr, (int)wg_ntohs(c->addr.port));
		wg_http_request(c, "http://127.0.0.1:8000/index.html", "User-Agent: hahaahah\r\n", NULL, 0);
	}
		break;
	case WG_EV_HTTP_MSG:
	{
		wg_http_message* hm = (wg_http_message*)ev_data;
		HANDLE hFile = CreateFileA("index.html", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			break;
		DWORD dwWritten = 0;
		WriteFile(hFile, hm->body.ptr, hm->body.len, &dwWritten, NULL);
		CloseHandle(hFile);
		wg_close_conn(c);
	}
		break;
	case WG_EV_ERROR:
		printf("error:%s\n", (char*)ev_data);
		break;
	default:
		break;
	}
}


int main()
{
	HANDLE wg_mgr = wg_mgr_init();
	wg_http_connect(wg_mgr, "http://127.0.0.1:8000", fn, NULL);
	wg_mgr_runloop(wg_mgr);
	return 0;
}
