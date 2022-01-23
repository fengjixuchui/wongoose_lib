#include"../wongoose_lib/wongoose.h"
#include<stdio.h>
#pragma comment(lib,"../wongoose_lib/wongoose.lib")


static void fn(wg_connection* c, int ev, void* ev_data, void* fn_data)
{
	switch (ev)
	{
	case WG_EV_CLOSE:
		printf("disconnect\n");
		break;
	case WG_EV_CONNECT:
	{
		char addr[65] = { 0 };
		wg_ntoa(&c->addr, addr, 65);
		printf("connect to %s : %d\n", addr, (int)wg_ntohs(c->addr.port));
	}
		break;
	case WG_EV_WS_MSG:
	{
		wg_ws_message* wm = (wg_ws_message*)ev_data;
		printf("recv data: %.*s\ntotallength:%llu\n", 10, wm->data.ptr,wm->data.len);
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
	wg_ws_connect(wg_mgr, "http://127.0.0.1:8000/websocket", fn, NULL,"");
	wg_mgr_runloop(wg_mgr);
	return 0;
}

