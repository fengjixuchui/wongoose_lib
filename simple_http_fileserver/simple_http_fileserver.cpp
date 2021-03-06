#include"../wongoose_lib/wongoose.h"
#include<stdio.h>
#pragma comment(lib,"../wongoose_lib/wongoose.lib")


static void fn(wg_connection* c, int ev, void* ev_data, void* fn_data)
{
	switch (ev)
	{
	case WG_EV_CLOSE:
		if (c->is_client) printf("disconnect a client\n");
		else printf("close port %d\n", (int)wg_ntohs(c->addr.port));
		break;
	case WG_EV_LISTEN:
	{
		char addr[65] = { 0 };
		wg_ntoa(&c->addr, addr, 65);
		printf("listen on %s : %d\n", addr, (int)wg_ntohs(c->addr.port));
	}
	break;
	case WG_EV_ACCEPT:
	{
		char addr[65] = { 0 };
		wg_ntoa(&c->addr, addr, 65);
		printf("accept on %s : %d\n", addr, (int)wg_ntohs(c->addr.port));
	}
	break;
	case WG_EV_HTTP_MSG: 
	{
		wg_http_message* hm = (wg_http_message*)ev_data;
		wg_http_serve_dir(c, hm, "website");
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
	wg_http_listen(wg_mgr, "0.0.0.0:8000", fn, NULL);
	wg_mgr_runloop(wg_mgr);
	return 0;
}
