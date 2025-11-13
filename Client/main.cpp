#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN			// Только при подключении <Windows.h>
#endif //!WIN32_LEAN_AND_MEAN

#include<Windows.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>
#include<iostream>
#include<string>
using namespace std;

#pragma comment(lib, "WS2_32.lib")

#define DEFAULT_PORT "27015"
#define BUFFER_LENGHT 1460

void WaitingServer(SOCKET connect_socket);

int main()
{
	setlocale(LC_ALL, "");
	cout << "Hi WinSock" << endl;

	INT iResult = 0;
	DWORD dwLastError = 0;

	//0)Инициализация WinSock:
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);		//MAKEWORD(2,2) - задает версию WinSock
	if (iResult != 0)
	{
		cout << "WSAStartup failed: " << iResult << endl;
		return iResult;
	}

	//1)Создаем переменные для хранения информации о сокете:
	addrinfo* result = NULL;
	addrinfo* ptr = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//2)Задаем информацию о Сервере, к которому будем подключаться:
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed: " << iResult << endl;
		WSACleanup();
		return iResult;
	}

	//3)Создаем сокет для подключения к Серверу:
	ptr = result;
	SOCKET connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (connect_socket == INVALID_SOCKET)
	{
		cout << "Socket error: " << dwLastError << endl;
		freeaddrinfo(result);
		WSACleanup();
		return dwLastError;
	}

	//4)Подключаемся к Серверу:
	iResult = connect(connect_socket, ptr->ai_addr, (INT)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		dwLastError = WSAGetLastError();
		cout << "Connection error: " << dwLastError << endl;
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return dwLastError;
	}

	////5)Отправляем данные на Сервер:
	//CHAR send_buffer[BUFFER_LENGHT] = "Hello Server, I am client";
	//iResult = send(connect_socket, send_buffer, strlen(send_buffer), 0);
	//if (iResult == SOCKET_ERROR)
	//{
	//	dwLastError = WSAGetLastError();
	//	cout << "Send failed with error: " << dwLastError << endl;
	//	closesocket(connect_socket);
	//	freeaddrinfo(result);
	//	WSACleanup();
	//	return dwLastError;
	//}
	//cout << iResult << " Bytes send" << endl;

	////6)Ожидаем ответ от Сервера:
	//CHAR recv_buffer[BUFFER_LENGHT] = {};
	//do
	//{
	//	iResult = recv(connect_socket, recv_buffer, BUFFER_LENGHT, 0);
	//	if (iResult > 0) cout << iResult << " Bytes received, message:\t" << recv_buffer << ".\n";
	//	else if (iResult == 0) cout << "Connection closed" << endl;
	//	else cout << "Receive failed with error: " << WSAGetLastError() << endl;
	//} while (iResult > 0);
	
	string str;
	while (true)
	{
		cout << ">: ";
		getline(cin, str);
		if (str == "exit" || str == "quit") break;

		iResult = send(connect_socket, str.c_str(), str.length(), 0);
		if (iResult == SOCKET_ERROR)
		{
			dwLastError = WSAGetLastError();
			cout << "Send failed with error: " << dwLastError << endl;
			closesocket(connect_socket);
			freeaddrinfo(result);
			WSACleanup();
			return dwLastError;
		}

		CHAR recv_buffer[BUFFER_LENGHT] = {};
		iResult = recv(connect_socket, recv_buffer, BUFFER_LENGHT, 0);
		if (iResult > 0) cout << iResult << " Bytes received, message:\t" << recv_buffer << ".\n";
		else if (iResult == 0) cout << "Connection closed" << endl;
		else cout << "Received failed with error: " << WSAGetLastError() << endl;
	}

	//7)Отключение от Сервера:
	iResult = shutdown(connect_socket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		dwLastError = WSAGetLastError();
		cout << "Shutdown failed with error: " << dwLastError << endl;
	}
	closesocket(connect_socket);
	freeaddrinfo(result);
	WSACleanup();
	return dwLastError;
}