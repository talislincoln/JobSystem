
#include "JobPool.h"

void emptyJob(Job*, const void*)
{

}

int main()
{
	JobPool jb;
	Job *root = jb.CreateJob(&emptyJob);

	#pragma omp parallel for
	for (size_t i = 0; i < 1000; ++i)
	{
		Job* job = jb.CreateJobAsChild(root, &emptyJob);
		jb.Run(job);
		
	}
	jb.Run(root);
	jb.Wait(root);
}