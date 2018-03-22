#pragma once
#include <vector>
#include <queue>
using namespace std;
class af_message_queue
{
	queue<vector<unsigned char>> _data_queue;
public:
	af_message_queue();
	~af_message_queue();
	void push_new_data(vector<unsigned char>& data_block){ _data_queue.emplace(data_block); }
	virtual void consume_data(vector<unsigned char>& vdata) = 0;
	void consume_queue_data()
	{
		while (!_data_queue.empty())
		{
			consume_data(_data_queue.front());
			_data_queue.pop();
		}
	}
};

