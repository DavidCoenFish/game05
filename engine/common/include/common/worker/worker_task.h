#pragma once
/*
   wanted a helper to manage getting a lambda into a thread, 
then waiting for it if you wanted to destroy it,
and handle telling it that there is work to be done. SignalWorkToDo()

not using multiple signals for the two states as was loosing info on [signal work, signal dtor, wait for multiple, returned dtor (missing work signal)]

*/
class WorkerTask
{
private:
   //disabled
   WorkerTask(const WorkerTask&);

public:
   static std::shared_ptr<WorkerTask> Factory(const std::function<void(void)>& in_task);
   WorkerTask(const std::function<void(void)>& in_task);
   ~WorkerTask();
   void SignalWorkToDo();

private:
   void DoWork();

private:
   HANDLE _event;
   std::future<void> _future;
   std::function<void(void)> _task;

   std::atomic_int _flag_work;
   std::atomic_bool _flag_dtor;
};
