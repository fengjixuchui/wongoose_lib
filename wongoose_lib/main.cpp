#include"wongoose.h"
#include<stdio.h>
#pragma comment(lib,"wongoose.lib")

static void fn(wg_connection* c, int ev, void* ev_data, void* fn_data)
{
	switch (ev)
	{
	case WG_EV_CLOSE:
		if (c->is_client) printf("disconnect a client\n");
		else printf("close port %d\n", (int)wg_ntohs(c->addr.port));
		break;
	case WG_EV_ACCEPT:
		printf("accept a client\n");
		break;
	case WG_EV_LISTEN:
	{
		char addr[65] = { 0 };
		wg_ntoa(&c->addr, addr, 65);
		printf("listen on %s : %d\n", addr, (int)wg_ntohs(c->addr.port));
	}
		break;
	case WG_EV_CONNECT:
	{
		printf("connect a server\n");

	}
	break;
	case WG_EV_RECV:         
		//only for tcp proto
		//printf("recv data:%s", c->recv.buf);
		//wg_iobuf_del(&c->recv, 0, c->recv.len);
		//wg_send(e, "hello\n", strlen("hello\n"));
		break;
	case WG_EV_ERROR:
		printf("error:%s\n", (char*)ev_data);
		break;
	case WG_EV_HTTP_MSG:
	{
		//server
		//wg_http_message* hm = (wg_http_message*)ev_data;
		//if (wg_http_match_uri(hm, "/websocket"))
		//{
		//	wg_ws_upgrade(c, hm, NULL);
		//	char* buf = (char*)calloc(1,1024 * 1024);
		//	memset(buf, 'A', 1024 * 1024);
		//	wg_ws_send(c, buf, 1024 * 1024, WEBSOCKET_OP_BINARY);
		//	free(buf);
		//}
		//wg_http_reply(e, 200, "abc=123\r\n", "Hello\n");
		//wg_http_reply(e, 200, "abc=123\r\n", "\x0\x1\x2", 3);
		//wg_http_serve_dir(c, hm, ".");

		//client

	}
	break;
	case WG_EV_WS_MSG:
	{
		wg_ws_message* wm = (wg_ws_message*)ev_data;
		Sleep(0);
	}
	break;
	case WG_EV_WS_OPEN:
	{
		/*hFile = CreateFileA("1.exe", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		if (hFile==INVALID_HANDLE_VALUE)
			break;
		char* buf = (char*)calloc(1,1300);
		for (DWORD dwRead = 1300; dwRead == 1300;)
		{
			ReadFile(hFile, buf, 1300, &dwRead, NULL);
			wg_ws_send(e, buf, dwRead, WEBSOCKET_OP_BINARY);
		}
		free(buf);
		CloseHandle(hFile);*/
	}
	break;
	default:
		break;
	}
}

int main()
{
	HANDLE wg_mgr = wg_mgr_init();
	//wg_listen(wg_mgr, "0.0.0.0:8000", fn, NULL);
	//wg_http_listen(wg_mgr, "0.0.0.0:8000", fn, NULL);
	wg_ws_connect(wg_mgr, "http://127.0.0.1:8000/websocket", fn, NULL,"");
	wg_mgr_runloop(wg_mgr);
	return 0;
}

