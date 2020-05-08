
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <opencv2/core/core.hpp>  // NOLINT
#include <opencv2/highgui/highgui.hpp>  // NOLINT
#include <opencv2/imgproc/imgproc.hpp>  // NOLINT
#include<sys/time.h>
#include <iostream>
#include<thread>
using std::thread;
#define MAXLINE 512


void server(){
	struct info{
		int framcount;
		double time_sec;
		double time_usec;
		char other[488];

	};
	//printf("%ld\n",sizeof(info));512
	char sendline[MAXLINE];
	int listenfd, connfd;
	struct sockaddr_in addrServer;
	struct sockaddr_in addrClient;
	memset(&addrServer, 0, sizeof(addrServer));
	memset(&addrClient, 0, sizeof(addrClient));
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	addrServer.sin_port = htons(6000);
	listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listenfd, (struct sockaddr *) &addrServer, sizeof(addrServer));

	listen(listenfd, 5);
	socklen_t len = sizeof(sockaddr);
	std::cout<<"Please wait for the client information"<<std::endl;
	connfd = accept(listenfd, (struct sockaddr *) &addrClient, &len);
	for(int n=0;n<100;n++){
		info a;
		recv(connfd, &a, MAXLINE, 0);
		struct timeval tpstart,tpend; 
		gettimeofday(&tpstart,NULL);
		double exetime=(tpstart.tv_sec-a.time_sec)*1000+(tpstart.tv_usec-a.time_usec)/1000;
		std::cout<<a.framcount<<" send delay"<<exetime<<"ms"<<std::endl;
		cv::Mat image_src = cv::Mat(640, 512, CV_8UC1);
		int height = image_src.rows;  //图像高度
		int width = image_src.cols;
		int widthstep = image_src.cols*image_src.channels();
		unsigned char revData[MAXLINE] = "";
			//接收数据
		for (int i = 0; i < height; i++)
		{
			memset(revData, 0, sizeof(revData));
			unsigned char* data = image_src.ptr<unsigned char>(i);
			int ret = recv(connfd, revData, MAXLINE, 0);
			if (ret>0){
				// for (int j = 0; j <widthstep; j++){
				// 	data[j] = revData[j];
				// }
                memcpy(data,revData,MAXLINE);
			}
			ret = 0;
		}
         gettimeofday(&tpend,NULL);
        std::cout<<" recive time  :"<<(tpend.tv_sec-tpstart.tv_sec)*1000000+(tpend.tv_usec-tpstart.tv_usec)<<"us"<<std::endl;
		// std::string name="output/out_"+std::to_string(n)+".jpg";
	    //  imwrite(name.c_str(), image_src);
	}
	close(connfd);
	close(listenfd);
}
//server
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
		std::cout<<"connect error "<<strerror(errno)<<"errno: "<<errno<<std::endl;
		return ;
	}

	std::cout<<"send message to server"<<std::endl;
    	cv::Mat src = cv::imread("8UC3.png", -1);
		if (src.empty()){
			std::cout << "read img error" << std::endl;
		}
		cv::Mat dst;
		cv::resize(src, dst, cv::Size(512, 640));
		cv::Mat img;
		cv::cvtColor(dst, img, cv::COLOR_BGR2GRAY);
    unsigned char sendData[MAXLINE];
	for(int n=0;n<100;n++){
		//
		info a;
		struct timeval tpstart,tpend; 
		gettimeofday(&tpstart,NULL);
		a.framcount=n;
		a.time_sec=tpstart.tv_sec;
		a.time_usec=tpstart.tv_usec;
		send(socketfd, &a, MAXLINE, 0);

		// cv::Mat src = cv::imread("8UC3.png", -1);
		// if (src.empty()){
		// 	std::cout << "read img error" << std::endl;
		// }
		// cv::Mat dst;
		// cv::resize(src, dst, cv::Size(512, 640));
		// cv::Mat img;
		// cv::cvtColor(dst, img, cv::COLOR_BGR2GRAY);
		int height = img.rows;  
		int width = img.cols;
		//cv::imwrite("out.jpg", img);
		int widthstep = img.cols*img.channels();
		//printf("widthstep : %d\n", widthstep);
		
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
        //send(socketfd, sendData, 640*MAXLINE, 0);
        gettimeofday(&tpend,NULL);
        std::cout<<" send time  :"<<(tpend.tv_sec-tpstart.tv_sec)*1000000+(tpend.tv_usec-tpstart.tv_usec)<<"us"<<std::endl;
	}
	
		
	close(socketfd);
}

int main(int argc, char **argv)
{
    thread serverThread(server); 
    thread clientThread(client);

    serverThread.join();
    clientThread.join();
    return 0;
}
