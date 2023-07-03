#include "common/common_pch.h"

#include "common/worker/worker_task.h"

std::shared_ptr<WorkerTask> WorkerTask::Factory(const std::function<void(void)>& in_task)
{
	return std::make_shared<WorkerTask>(in_task);
}

WorkerTask::WorkerTask(const std::function<void(void)>& in_task) //const std::future<_Ty>& future)
: _task(in_task)
, _event(0)
, _flag_work(0)
, _flag_dtor(false)
{
	_event = CreateEvent( 
		NULL,   // default security attributes
		FALSE,  // auto-reset event object
		FALSE,  // initial state is nonsignaled
		NULL);  // unnamed object

	_future = std::async(std::launch::async, [=](){
		DoWork();
	});
	return;
}

WorkerTask::~WorkerTask()
{
	_flag_dtor = true;
	if (nullptr != _event)
	{
		SetEvent(_event);
	}
	if (true == _future.valid())
	{
		_future.wait();
	}
	if (nullptr != _event)
	{
		CloseHandle(_event);
	}
	_event = nullptr;
	return;
}

void WorkerTask::SignalWorkToDo()
{
	_flag_work++;
	SetEvent(_event);
	return;
}

void WorkerTask::DoWork()
{
	int flagWorkCache = 0;
	while (true)
	{
		WaitForSingleObject(_event, INFINITE);

		const int flagWork = _flag_work.load();
		if (flagWorkCache != flagWork)
		{
			flagWorkCache = flagWork;
			//we expect task to keep running while it has work, rather than telling to run once for each time SignalWorkToDo() has been called
			_task();
		}

		if (true == _flag_dtor)
		{
			break;
		}
	}
	return;
}
