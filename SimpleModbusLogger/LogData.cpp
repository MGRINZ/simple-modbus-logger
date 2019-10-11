#include "LogData.h"

LogData::LogData(string path)
{
	this->path = path;
}

short LogData::minimum(vector<LogItem> &items, int start, int end)
{
	short min_index = start;

	for (int i = start + 1; i < end; i++)
	{
		if (items[min_index].getAddress() > items[i].getAddress())
			min_index = i;
	}
	return min_index;
}

void LogData::sort(vector<LogItem> &items)
{
	int items_length = items.size();
	int min = 0;
	for (int i = 0; i < items_length; i++)
	{
		min = minimum(items, i, items_length);
		LogItem temp = items[i];
		items[i] = items[min];
		items[min] = temp;
	}
}

void LogData::optimize()
{
	std::ifstream data_file(path);
	string item_str;

	char type[2];
	short address = 0;
	string label;

	int semicolon = 0;

	while (std::getline(data_file, item_str))
	{
		type[0] = item_str[1];
		
		if (!isdigit(item_str[2]))
			type[1] = item_str[2];
		else
			type[1] = '\0';

		semicolon = item_str.find(";");

		address = atoi(item_str.substr(strlen(type) + 1, semicolon).c_str());
		
		if (semicolon != -1)
			label = item_str.substr(semicolon + 1);
		else
			label = "";

		LogItem item(type, address, label);

		if(!strcmp(type, LogItem::ITEM_INPUT))
			inputs.push_back(item);
		else if(!strcmp(type, LogItem::ITEM_OUTPUT))
			outputs.push_back(item);
		else if(!strcmp(type, LogItem::ITEM_REGISTER))
			registers.push_back(item);
		else if(!strcmp(type, LogItem::ITEM_ANALOG_INPUT))
			analogInputs.push_back(item);
	}
	
	sort(inputs);
	sort(outputs);
	sort(registers);
	sort(analogInputs);
}

void LogData::log()
{
	vector<LogItem>* items[4] = {
		&inputs,
		&outputs,
		&registers,
		&analogInputs
	};

	for (int i = 0; i < 4; i++)
	{
		vector<LogItem>& item = *items[i];
		int items_length = item.size();

		int start = item[0].getAddress();
		int count = 1;


		for (int j = 1; j < items_length; j++)
		{
			if (item[j].getAddress() - item[j - 1].getAddress() == 1)
				count++;
			else
			{
				//std::cout << item[0].getType() << std::endl;
				count = 1;
				start = item[j].getAddress();
			}
		}

	}
}

void LogData::logOutput()
{
}

void LogData::logInput()
{
}

void LogData::logRegisters()
{
	
}

void LogData::logAnalogInput()
{
}
