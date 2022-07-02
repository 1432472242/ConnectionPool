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
	//��ȡ���ӳض�����ģʽ
	static ConnectionPool* getConnectionPool();
	//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
	shared_ptr<Connection> getConnection();
private:
	//#1������һ�������캯��˽�л�
	ConnectionPool();

	//�������ļ��м���������
	bool loadConfigFile();

	//�����ڶ������߳��У�ר�Ÿ�������������
	void produceConnectionTask();

	

	//��ʱ�����̣߳��������Ŀ�������
	void scannerConnectionTask();
	string _ip;//mysql��ip��ַ
	unsigned short _port;//mysql�Ķ˿ں�
	string _username;//mysql��¼�û���
	string _password;//mysql��¼����
	string _dbname;//���ӵ����ݿ�����
	
	int _initSize;//���ӳصĳ�ʼ������
	int _maxSize;//���ӳص����������
	int _maxIdleTime;//����������ʱ��
	int _connectionTimeOut;//��ȡ���ӳ�ʱʱ��

	queue<Connection*> _connectionQue;//�洢mysql���ӵĶ���
	mutex _queMutex;//ά�����Ӷ��е��̰߳�ȫ������
	atomic_int _connectionCnt;//��¼����������connection��������
	condition_variable _cv;//�����������������������������̺߳������̵߳�ͨ��
};