#pragma once

//reference websites
/*
https://codereview.stackexchange.com/questions/36018/thread-pool-on-c11

https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/

https://github.com/Tastyep/TaskManager
*/

typedef void(*JobFunction)(Job*, const void*);

struct Job
{
	JobFunction function;
	Job* parent;
	unsigned __int32 unfinishedJobs;
	char padding[];
};

class JobPool
{
private:

	Job* AllocateJob()
	{
		return new Job();
	}


public:
	JobPool();
	~JobPool();

	void Execute(Job* job)
	{
		//(job->function)(job, job->data);
	}

	Job* CreateJob(JobFunction function)
	{
		Job* job = AllocateJob();
	}
};

