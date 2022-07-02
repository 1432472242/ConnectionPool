#include"iostream"
#include"Connection.h"
#include"ConnectionPool.h"
int main()
{
	
	clock_t begin = clock();
	ConnectionPool* cp = ConnectionPool::getConnectionPool();

	for (int i = 0; i < 1000; i++)
	{
		/*Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
		conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
		conn.update(sql); */
		shared_ptr<Connection> pcon = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
		pcon->update(sql);
	}
	clock_t end = clock();
	cout << end - begin << "ms" << endl;
	return 0;
}