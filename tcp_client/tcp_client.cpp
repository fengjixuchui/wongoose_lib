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
	case WG_EV_RECV:
		if (c->recv.len >= strlen("hello\n"))
		{
			printf("recv data:%s", c->recv.buf);
			wg_iobuf_del(&c->recv, 0, strlen("hello\n")); 		//only for tcp proto
			wg_send(c, "hello\n", strlen("hello\n"));
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
	wg_connect(wg_mgr, "127.0.0.1:8000", fn, NULL);
	wg_mgr_runloop(wg_mgr);
	return 0;
}

