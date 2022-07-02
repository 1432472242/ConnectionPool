#pragma once
#include<iostream>
using namespace std;
#define LOG(str)\
cout<<__FILEW__<<": "<<__LINE__<<": "\
	<<__TIMESTAMP__<<": "<<str<<endl;
