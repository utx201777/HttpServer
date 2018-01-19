#include "sample.h"
/************************************************************************
   created: 2018/01/18
   class: 
   author: linyi
   usage: 主入口，引入Sample类，完成全部行为
************************************************************************/


int main()
{
	Sample * sam = new Sample();
	sam->sampleInit();
	sam->sampleRun();
	return 0;
}