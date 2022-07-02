#include"ConnectionPool.h"
#include"public.h"
#include<string>
using namespace std;
//�̰߳�ȫ������ʽ���������ӿ�
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;
	return &pool;
 }

//�������ļ��м���������
bool  ConnectionPool::loadConfigFile()
{
	FILE* pf = fopen("mysql.ini","r");
	if (pf==nullptr)
	{
		LOG("mysql.ini file is not exist!");
		return false;
	}

	while (!feof(pf))
	{
		char line[1024] = { 0 };
		fgets(line,1024,pf);
		string str = line;
		int idx = str.find('=',0);
		if (idx == -1)
		{
			continue;
		}
		int endidx = str.find('\n',idx);
		string key = str.substr(0,idx);
		string value = str.substr(idx+1,endidx-idx-1);
		if (key == "ip") { _ip = value; }
		else if (key == "port") { _port = atoi(value.c_str()); }
		else if (key == "username") { _username = value; }
		else if (key == "password") {_password= value; }
		else if (key == "dbname") { _dbname =value; }
		else if (key == "initSize") {_initSize = atoi(value.c_str()); }
		else if (key == "maxSize") {_maxSize = atoi(value.c_str()); }
		else if (key == "maxIdleTime") { _maxIdleTime = atoi(value.c_str()); }
		else if (key == "connectionTimeOut") { _connectionTimeOut = atoi(value.c_str()); }
	}
	return true;
}

//���ӳصĹ��캯��
ConnectionPool::ConnectionPool()
{
    //����������
	if (!loadConfigFile())
	{
		return;
	}

	//����initSize����������
	for (int i = 0; i < _initSize; ++i)
	{
		Connection* p = new Connection();
		p->connect(_ip,_port,_username,_password,_dbname);
		p->refreshAliveTime();//ˢ��һ�¿���ʱ������
		_connectionQue.push(p);
		_connectionCnt++;
	}

	//����һ���µ��̣߳���Ϊ���ӵ�������
	thread produce(std::bind(&ConnectionPool::produceConnectionTask,this));
	produce.detach();

	//����һ���̣߳���ʱ����������ʱ�������
	thread scaner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scaner.detach();
	



}

//�����ڶ������߳��У�ר�Ÿ�������������
void ConnectionPool::produceConnectionTask()
{
	for (;;)
	{
		unique_lock<mutex> lock(_queMutex);
		while (!_connectionQue.empty())
		{
			_cv.wait(lock);//���в��գ������߽��̽���ȴ�״̬���ͷ���
		}
		//���пգ�������������û�дﵽmaxSize�����������µ�����
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();//ˢ��һ�¿���ʱ������
			_connectionQue.push(p);
			_connectionCnt++;
		}
		//֪ͨ�������̣߳���������������
		_cv.notify_all();

	}

	
}


//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queMutex);
	while (_connectionQue.empty())
	{
		if (cv_status::timeout == _cv.wait_for(lock, chrono::milliseconds(_maxIdleTime)))
		{
			if (_connectionQue.empty())
			{
				LOG("��ȡ�������ӳ�ʱ��...��ȡ����ʧ��");
				return nullptr;
			}
		}
	}
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection* pcon) {
			unique_lock<mutex> lock(_queMutex);//�ڷ�����Ӧ���߳��е��õģ�����Ҫ���Ƕ��е��̰߳�ȫ����
			pcon->refreshAliveTime();//ˢ��һ�¿���ʱ������
			_connectionQue.push(pcon);
		});
	_connectionQue.pop();
	_cv.notify_all();//�����������Ժ�֪ͨ�������̼߳��һ�£��������Ϊ�գ��Ͻ�����
	return sp;

}

void ConnectionPool::scannerConnectionTask()
{
	for (;;)
	{
		//sleep_forģ�ⶨʱЧ��
		std::this_thread::sleep_for(chrono::seconds(_maxIdleTime));

		//ɨ���������У��ͷŶ��������
		unique_lock<mutex> lock(_queMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveTime() >= (_maxIdleTime * 1000))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p;//����~Connection�����ͷ�����
			}
			else
			{
				break;//��ͷ����û�г���_maxIdleTime���������ӿ϶�û��
			}
		}

	}

}