
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <opencv2/core/core.hpp>  // NOLINT
#include <opencv2/highgui/highgui.hpp>  // NOLINT
#include <opencv2/imgproc/imgproc.hpp>  // NOLINT
#include<sys/time.h>
#include <iostream>
#include <string>
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
		struct timeval tpend; 
		gettimeofday(&tpend,NULL);
		double exetime=(tpend.tv_sec-a.time_sec)*1000+(tpend.tv_usec-a.time_usec)/1000;
		std::cout<<a.framcount<<" "<<exetime<<"ms"<<std::endl;
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
				for (int j = 0; j <widthstep; j++){
					data[j] = revData[j];
				}
			}
			ret = 0;
		}
		// std::string name="output/out_"+std::to_string(n)+".jpg";
		// imwrite(name.c_str(), image_src);
	}
	close(connfd);
	close(listenfd);
}
//server
int main(int argc, char **argv)
{
	
	server();
	
}
