#include <stdio.h>

int main()
{
	char path[]="/home/sun/Desktop/Rotanimret/skeylogger/skl";    //Your path 
	char arrayName[]="path";                                      //Your array name

	for(int i=0;i<sizeof path;i++)
		printf("%s[%d]='%c';\n",arrayName,i,path[i]);

	return 0;
}
