#pragma once

#include <atomic>
#include <queue>

//reference websites
/*
https://codereview.stackexchange.com/questions/36018/thread-pool-on-c11

https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/

https://github.com/Tastyep/TaskManager
*/

#define MAX_JOB_COUNT 10000



struct Job
{
	void(*function)(Job*, const void*);
	Job* parent;
	std::atomic<unsigned __int32> unfinishedJobs;
	void* data;
	char padding[]; // should use memcp to copy data from another job to this variable
};

typedef void(*JobFunction)(Job*, const void*);

static Job g_jobAllocator[MAX_JOB_COUNT] = { 0 };
static std::atomic<uint32_t> g_allocatedJobs = 0u;

Job* g_jobsToDelete[MAX_JOB_COUNT];
std::atomic<uint32_t> g_jobToDeleteCount = 0u;

class JobPool
{
private:

	Job* AllocateJob(void)
	{
		//return new Job();

		const uint32_t index = ++g_allocatedJobs;
		return g_jobAllocator[(index - 1) % MAX_JOB_COUNT];
	}

	/// this is here beacuse this queue will most likely be in a differet class
	std::queue<Job*> GetWorkerThreadQueue()
	{
		return m_queue;
	}

	bool HasJobCompleted(Job* job)
	{
		return job->unfinishedJobs > 0;
	}

	Job* GetJob()
	{
		Job* job = m_queue.front();
		m_queue.pop();

		return job;
	}

	void Finish(Job* job)
	{
		const int32_t unfinishedJobs = --(job->unfinishedJobs);
		if (unfinishedJobs == 0)
		{
			const int32_t index = ++g_jobToDeleteCount;
			g_jobsToDelete[index - 1] = job;

			if (job->parent)
			{
				Finish(job->parent);
			}

			--(job->unfinishedJobs);
		}
	}

public:
	JobPool() {}
	~JobPool() {}

	void Execute(Job* job)
	{
		(job->function)(job, job->data);
	}

	Job* CreateJob(JobFunction function)
	{
		Job* job = AllocateJob();
		job->function = function;
		job->parent = nullptr;
		job->unfinishedJobs = 1;

		return job;
	}

	Job* CreateJobAsChild(Job* parent, JobFunction function)
	{
		parent->unfinishedJobs++;

		Job* job = AllocateJob();
		job->function = function;
		job->parent = parent;
		job->unfinishedJobs = 1;

		return job;
	}

	void Run(Job* job)
	{
		std::queue<Job*> queue = GetWorkerThreadQueue();

		queue.push(job);
	}

	void Wait(Job* job)
	{
		while (!HasJobCompleted(job))
		{
			Job* nextJob = GetJob();

			if (nextJob)
			{
				Execute(nextJob);
			}
		}
	}

private:
	/// change this queue to a multi threading queue
	std::queue<Job*> m_queue;
};

//std::queue<Job*> JobPool::m_queue;