#pragma once
#include<mysql.h>
#include<string>
#include<ctime>
using namespace std;
/* ʵ��MySQL���ݿ�Ĳ���*/
class Connection
{
public:
	Connection();//��ʼ�����ݿ�����
	~Connection();//�ͷ����ݿ�������Դ
	bool connect(string ip,//ip��ַ
		unsigned short port,//�˿ں�
		string username,//�û���
		string password,//����
		string dbname);//ѡ������ݿ�
	bool update(string sql);//���²��� insert delete  update
	MYSQL_RES* query(string sql);//��ѯ���� select
	
    //ˢ��һ�����ӵ���ʼ����ʱ���
	void refreshAliveTime() { _alivetime = clock(); }
	//���ش��ʱ��
	clock_t getAliveTime() { return clock() - _alivetime; }

private:
	MYSQL* _conn;//��ʾ��MYSQL  Server��һ������
	clock_t _alivetime;//��¼�������״̬�����ʼ���ʱ��

};
