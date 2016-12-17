// testini.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
int main(int argc, char* argv[])
{
	LPSTR strName = "jacky";
	int nAge = 13;
	CHAR strtemp[500];
	sprintf(strtemp,"%d",nAge);
	WritePrivateProfileString("StudentInfo","Name",strName,"c:\\student.ini");
	WritePrivateProfileString("StudentInfo","Age",strtemp,"c:\\student.ini");
	
	//GetPrivateProfileString
	int nStudAge=GetPrivateProfileInt("StudentInfo","Age",10,"c:\\student.ini");
	printf("Hello World, %d!\n",nStudAge);
	return 0;
}

