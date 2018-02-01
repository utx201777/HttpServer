#include "sample.h"
#include "Segment.h"
#include "segmentSample.h"
/************************************************************************
   created: 2018/01/18
   class: 
   author: linyi
   usage: 主入口，引入Sample类，完成全部行为
************************************************************************/

int main()
{
	/*Sample * sam = new Sample();
	sam->sampleInit();
	sam->sampleRun();
	*/
	/*Segment::SegmentProcess * m = new Segment::SegmentProcess("model/分割模型/model_seg_1.obj", 2);
	m->printModel(std::cout);
	m->clusterModel("model/分割模型/model_seg_1_cluster.txt");
	m->clusterTriangle("model/分割模型/model_seg_1_cluster.txt", "model/分割模型/model_seg_1_triangle.txt");
	*/
	SegmentSample *sam = new SegmentSample();
	sam->sampleInit();
	sam->sampleRun();
	return 0;
}