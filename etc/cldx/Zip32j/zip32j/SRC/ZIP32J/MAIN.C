/*
	main.c

		by Yoshioka Tsuneo(QWF00133@niftyserve.or.jp)
		welcome any e-mail!!
		You can use this file as Public Domain Software.
		Copy,Edit,Re-distibute and for any purpose,you can use this file.

*/
#include "zip32j.h"
#include <stdio.h>
#include <direct.h>
int main(int argc,char *argv[])
{
	int v;
	int ret;
	char output[10001];
	char input[10001]="";
	int i;

	for(i=1;i<argc;i++){
		strcat(input,argv[i]);
		strcat(input," ");
	}
	v=ZipGetVersion();
	chdir("c:\\tmp\\zip");
	//printf("ZipGetVersion()=%d\n",v);
	//ret=Zip(NULL,"-r c:\\tmp\\zip\\z1.zip c:\\tmp\\zip\\data1.txt c:\\tmp\\zip\\data2.txt",output,10000);
	//ret=Zip(NULL,"c:\\tmp\\zip\\z1.zip c:\\tmp\\zip\\data1.txt c:\\tmp\\zip\\data2.txt",output,10000);
	//ret=Zip(NULL,"c:\\tmp\\zip\\z1.zip c:\\tmp\\zip\\ @c:\\tmp\\zip\\res tmp",output,10000);
	//ret=Zip(NULL,"c:\\tmp\\zip\\z1.zip C:\\tmp\\ZIP\\‚ ‚¢‚¤‚¦‚¨‚©‚«‚­‚¯‚±‚³‚µ‚·‚¹‚»‚½‚¿‚Â‚Ä‚Æ‚È‚É‚Ê‚Ë‚Ì‚Í‚Ð‚Ó‚Ö‚Ù‚Ü‚Ý‚Þ‚ß‚à‚â‚ä‚æ‚ç‚è‚é‚ê‚ë‚í‚ð‚ñ",output,10000);
	//ret=Zip(NULL,"-R z tmp",output,10000);
	ret=Zip(NULL,input,output,10000);
	return 0;
}
