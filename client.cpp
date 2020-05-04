#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include<sys/time.h>

#define MAXLINE 512

void client()
{
	struct info{
		int framcount;
		//char framcount[4];
		double time_sec;
		double time_usec;

		char other[488];

	};
	//printf("%ld\n",sizeof(info));512
	const char *servInetAddr = "127.0.0.1";
	int socketfd;
	struct sockaddr_in sockaddr;
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(6000);
	inet_pton(AF_INET, servInetAddr, &sockaddr.sin_addr);
	if ((connect(socketfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))) < 0)
	{
		printf("connect error %s errno: %d\n", strerror(errno), errno);
		exit(0);
	}

	printf("send message to server\n");

	for(int n=0;n<100;n++){
		//
		info a;
		struct timeval tpstart; 
		gettimeofday(&tpstart,NULL);
		a.framcount=n;
		a.time_sec=tpstart.tv_sec;
		a.time_usec=tpstart.tv_usec;
		send(socketfd, &a, MAXLINE, 0);
		cv::Mat src = cv::imread("8UC3.png", -1);
		if (src.empty()){
			std::cout << "read img error" << std::endl;
		}
		cv::Mat dst;
		cv::resize(src, dst, cv::Size(512, 640));
		cv::Mat img;
		cv::cvtColor(dst, img, cv::COLOR_BGR2GRAY);
		int height = img.rows;  
		int width = img.cols;
		//cv::imwrite("out.jpg", img);
		int widthstep = img.cols*img.channels();
		//printf("widthstep : %d\n", widthstep);
		unsigned char sendData[MAXLINE];
		for (int i = 0; i < height; i++)
		{
			memset(sendData, 0, sizeof(sendData));
			unsigned char* data = img.ptr<unsigned char>(i);
			for (int j = 0; j < widthstep; j++)
			{
				sendData[j] = data[j];
			}
			send(socketfd, sendData, MAXLINE, 0);
		}
	}
	
		
	close(socketfd);
}

//client
int main(int argc, char **argv)
{
	client();
}
