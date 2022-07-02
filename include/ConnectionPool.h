#pragma once
#include<string>
#include<queue>
#include"Connection.h"
#include<mutex>
#include<functional>
#include<condition_variable>
using namespace std;
class ConnectionPool
{
public:
	//获取连接池对象单例模式
	static ConnectionPool* getConnectionPool();
	//给外部提供接口，从连接池中获取一个可用的空闲连接
	shared_ptr<Connection> getConnection();
private:
	//#1单例第一步、构造函数私有化
	ConnectionPool();

	//从配置文件中加载配置项
	bool loadConfigFile();

	//运行在独立的线程中，专门负责生产新连接
	void produceConnectionTask();

	

	//定时清理线程，清理多余的空闲连接
	void scannerConnectionTask();
	string _ip;//mysql的ip地址
	unsigned short _port;//mysql的端口号
	string _username;//mysql登录用户名
	string _password;//mysql登录密码
	string _dbname;//连接的数据库名称
	
	int _initSize;//连接池的初始连接量
	int _maxSize;//连接池的最大连接量
	int _maxIdleTime;//连接最大空闲时间
	int _connectionTimeOut;//获取连接超时时间

	queue<Connection*> _connectionQue;//存储mysql连接的队列
	mutex _queMutex;//维护连接队列的线程安全互斥锁
	atomic_int _connectionCnt;//记录所创建连接connection的总数量
	condition_variable _cv;//设置条件变量，用于连接生产者线程和消费线程的通信
};