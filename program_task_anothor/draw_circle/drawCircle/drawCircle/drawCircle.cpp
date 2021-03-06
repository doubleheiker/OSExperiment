// drawCircle.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <opencv2/opencv.hpp>
#include <iostream>
#include <Windows.h>
#include <process.h>

using namespace std;
using namespace cv;

#define PI 3.14159265358979
#define P(S) WaitForSingleObject(S, INFINITE)
#define V(S) ReleaseSemaphore(S, 1, NULL)
HANDLE mutex;
Mat board(800, 1000, CV_8UC3);

void drawPoint(Point pt, bool f)
{
	Scalar color;
	if (f == true)
		color = Scalar(0, 0, 255);
	else
		color = Scalar(255, 0, 0);

	circle(board, pt, 3, color, -1);
}

DWORD WINAPI drawSquare()
{
	int w, h, l;
	P(mutex);
	w = board.cols;
	h = board.rows;
	V(mutex);
	l = min(w, h) * 0.8;
	int x = w / 2 - l / 2;
	int y = h / 2 - l / 2;
	double i = 0;
	while (i < l * 4)
	{
		int dx, dy;
		if (i < l)
		{
			dx = i;
			dy = 0;
		}
		else if (i < 2 * l)
		{
			dx = l;
			dy = i - l;
		}
		else if (i < 3 * l)
		{
			dx = 3 * l - i;
			dy = l;
		}
		else
		{
			dx = 0;
			dy = 4 * l - i;
		}
		Point p(x + dx, y + dy);
		P(mutex);
		drawPoint(p, 0);
		V(mutex);
		i += 4;
	}
	return 1;
}

DWORD WINAPI drawCircle()
{
	double angle = 0;
	int x, y, r;
	P(mutex);
	x = board.cols / 2;
	y = board.rows / 2;
	V(mutex);
	r = min(x, y) * 0.8;

	while (angle < 2 * PI)
	{
		int dx, dy;
		dx = r * cos(angle);
		dy = r * sin(angle);
		Point p(x + dx, y + dy);
		P(mutex);
		drawPoint(p, 1);
		V(mutex);
		angle += 0.01;
	}

	return 1;
}

int main()
{
	board = Scalar::all(255);

	mutex = CreateSemaphore(NULL, 1, 1, NULL);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)drawSquare, NULL, NULL, NULL);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)drawCircle, NULL, NULL, NULL);

	while (true)
	{
		P(mutex);
		imshow("Draw", board);
		char c = waitKey(10);
		if (c == 27)
			return 0;
		V(mutex);
	}

	return 0;
}