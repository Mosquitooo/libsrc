
#ifndef __TASK_H__
#define __TASK_H__

typedef void (*Task_Func)(void);

struct Task
{
	Task_Func func;  //任务函数
	void* m_data;	//任务函数参数
};

class TaskPool
{
public:
	TaskPool();
	~TaskPool();

public:

};

#endif
