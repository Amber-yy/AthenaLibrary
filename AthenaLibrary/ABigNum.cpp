#include "ABigNum.h"
#include <algorithm>
#include <vector>

ATHENA_BEGIN

struct bigNumData
{
	char *base;
	int size;
	int currentPos;
	bool isNegative;
	bool isOk;
};

ABigNum::ABigNum()
{
	data = std::make_unique<bigNumData>();
	data->size = baseSize;
	data->currentPos = 0;
	data->isNegative = false;
	data->base = reinterpret_cast<char *>(malloc(baseSize));
	if (data->base)
	{
		data->isOk = true;
	}
	else
	{
		data->isOk = false;
	}
}

ABigNum::ABigNum(long long num) :ABigNum()
{

	if (num < 0)
	{
		data->isNegative = true;
		num = -num;
	}

	for (;; data->currentPos++)
	{
		data->base[data->currentPos] = num % 10;
		if (num < 10)
		{
			data->base[data->currentPos++] = static_cast<char>(num);
			break;
		}
		num /= 10;
	}

}

ABigNum::ABigNum(ABigNum && num)
{
	data.swap(num.data);
}

ABigNum::ABigNum(const ABigNum & num)
{
	data = std::make_unique<bigNumData>();
	data->size = num.data->size;
	data->currentPos = num.data->currentPos;
	data->isNegative = num.data->isNegative;
	data->base = reinterpret_cast<char *>(malloc(data->size));
	memcpy(data->base, num.data->base, data->currentPos);
}

ABigNum::~ABigNum()
{
	free(data->base);
}

std::ostream & operator<<(std::ostream &out,const ABigNum &num)
{
	int i = num.data->currentPos-1;
	char *temp = num.data->base;
	if (num.data->currentPos == 1 && num.data->base[0] == 0)
	{
		out <<'0';
		return out;
	}
	if (num.data->isNegative)
	{
		out << '-';
	}
	for (; i >= 0; i--)
	{
		out << static_cast<char>(temp[i]+'0');
	}
	return out;
}

std::istream & operator>>(std::istream &in,  ABigNum &num)
{
	char temp;
	int index = 0;
	num.data->isNegative = false;
	while (true)
	{
		temp = in.get();
		if (!(temp>='0'&&temp<='1'))
		{
			if (temp == '-')
			{
				num.data->isNegative = true;
				continue;
			}
			break;
		}
		num.data->base[index] = temp-'0';
		index++;
	}

	int half = index / 2;
	for (int i = 0; i < half; i++)
	{
		std::swap(num.data->base[i],num.data->base[index-i-1]);
	}

	num.data->currentPos = index;
	return in;
}

bool ABigNum::less(const ABigNum &num) const
{
	bool result[] = { false ,true,false,true,false };
	return compare(result, num);
}

bool ABigNum::greater(const ABigNum &num) const
{
	bool result[] = { true ,false,true,false,false };
	return compare(result, num);
}

bool ABigNum::equal(const ABigNum &num) const
{
	bool result[] = { false ,false,false,false,true };
	return compare(result, num);
}

bool ABigNum::lessEqual(const ABigNum &num) const
{
	bool result[] = { false ,true,false,true,true };
	return compare(result, num);
}

bool ABigNum::greaterEqual(const ABigNum &num) const
{
	bool result[] = { true,false,true,false,true };
	return compare(result, num);
}

bool ABigNum::notEqual(const ABigNum &num) const
{
	bool result[]={ true,true,true,true,false };
	return compare(result, num);
}

bool ABigNum::compare(const bool *result, const ABigNum &num) const
{
	if (data->currentPos > num.data->currentPos)
	{
		return result[0];
	}
	else if (data->currentPos < num.data->currentPos)
	{
		return result[1];
	}

	for (int i = data->currentPos - 1; i >= 0; i--)
	{
		if (data->base[i]>num.data->base[i])
		{
			return result[2];
		}
		if (data->base[i]<num.data->base[i])
		{
			return result[3];
		}
	}

	return result[4];
}

bool ABigNum::isOk() const
{
	return data->isOk;
}

int ABigNum::getLength() const
{
	return data->currentPos;
}

const char * ABigNum::getBase() const
{
	return data->base;
}

ABigNum & ABigNum::operator=(ABigNum &&num)
{
	data.swap(num.data);
	return *this;
}

ABigNum & ABigNum::operator=(const ABigNum &num)
{
	if (num.data->size > data->size)
	{
		char *newbase = reinterpret_cast<char *>(realloc(data->base, num.data->size));
		if (newbase == nullptr)
		{
			throw ABadAlloc("ABigNum &ABigNum::operator=(const ABigNum &):Memory Allocation Failed");
		}
		data->base = newbase;
		data->size =num.data->size;
	}

	memcpy(data->base,num.data->base,num.data->currentPos);
	data->currentPos = num.data->currentPos;
	data->isNegative = num.data->isNegative;

	return *this;
}

ABigNum ABigNum::operator*(const ABigNum &num) const
{
	ABigNum temp(0);

	if (data->currentPos>num.data->currentPos)
	{
		metaMulti(&temp, this, &num);
	}
	else
	{
		metaMulti(&temp, &num, this);
	}

	return temp;
}

ABigNum ABigNum::operator/(const ABigNum &num) const
{
	ABigNum temp;
	if (this->greater(num))
	{
		metaDivi(&temp,this,&num);
		return temp;
	}
	temp.data->currentPos = 1;
	temp.data->base[0] = 0;
	return temp;
}

bool ABigNum::operator<(const ABigNum &num) const
{
	if (data->isNegative)
	{
		if (num.data->isNegative)//- -
		{
			return greater(num);
		}
		return true;//- +
	}

	if (num.data->isNegative)//- -
	{
		return false;
	}
	return less(num);
}

bool ABigNum::operator>(const ABigNum &num) const
{
	if (data->isNegative)
	{
		if (num.data->isNegative)//- -
		{
			return less(num);
		}
		return false;//- +
	}

	if (num.data->isNegative)//- -
	{
		return true;
	}
	return greater(num);
}

bool ABigNum::operator==(const ABigNum &num) const
{
	if (data->isNegative == num.data->isNegative)
	{
		return equal(num);
	}

	return false;
}

bool ABigNum::operator<=(const ABigNum &num) const
{
	if (data->isNegative)
	{
		if (num.data->isNegative)//- -
		{
			return greaterEqual(num);
		}
		return true;//- +
	}

	if (num.data->isNegative)//- -
	{
		return false;
	}
	return lessEqual(num);
}

bool ABigNum::operator>=(const ABigNum &num) const
{
	if (data->isNegative)
	{
		if (num.data->isNegative)//- -
		{
			return lessEqual(num);
		}
		return false;//- +
	}

	if (num.data->isNegative)//- -
	{
		return true;
	}
	return greaterEqual(num);
}

bool ABigNum::operator!=(const ABigNum &num) const
{
	if (data->isNegative == num.data->isNegative)
	{
		return notEqual(num);
	}

	return true;
}

ABigNum & ABigNum::operator/=(const ABigNum &num)
{
	if (this->greater(num))
	{
		ABigNum temp;
		metaDivi(&temp, this, &num);
		*this = std::move(temp);
		return *this;
	}
	data->currentPos = 1;
	data->base[0] = 0;
	return *this;
}

ABigNum & ABigNum::operator*=(const ABigNum &num)
{
	if (data->currentPos>num.data->currentPos)
	{
		metaMulti(this, this, &num);
	}
	else
	{
		metaMulti(this, &num, this);
	}

	return *this;
}

ABigNum & ABigNum::operator+=(const ABigNum &num)
{
	ABigNum *bigOne, *smallOne;

	if (this->greater(num))
	{
		bigOne = const_cast<ABigNum *>(this);
		smallOne = const_cast<ABigNum *>(&num);
	}
	else
	{
		smallOne = const_cast<ABigNum *>(this);
		bigOne = const_cast<ABigNum *>(&num);
	}

	if (num.data->isNegative == data->isNegative)//同号
	{
		if (std::max(data->currentPos,num.data->currentPos) == data->size)
		{
			resetSize(2 * data->size);
		}
		metaAdd(this, bigOne, smallOne);
	}
	else//异号
	{
		if (bigOne != this)
		{
			data->isNegative = !data->isNegative;
		}
		metaSub(this, bigOne, smallOne);
	}

	return *this;
}

ABigNum & ABigNum::operator-=(const ABigNum &num)
{

	ABigNum *bigOne, *smallOne;

	if (this->greater(num))
	{
		bigOne = const_cast<ABigNum *>(this);
		smallOne = const_cast<ABigNum *>(&num);
	}
	else
	{
		smallOne = const_cast<ABigNum *>(this);
		bigOne = const_cast<ABigNum *>(&num);
	}

	if (num.data->isNegative && data->isNegative)//同为负
	{
		if (bigOne == this)
		{
			data->isNegative = true;
		}
		else
		{
			data->isNegative = false;
		}
		metaSub(this, bigOne, smallOne);
	}
	else if (!num.data->isNegative && !data->isNegative)//同为正
	{
		if (bigOne == this)
		{
			data->isNegative = false;
		}
		else
		{
			data->isNegative = true;
		}
		metaSub(this, bigOne, smallOne);
	}
	else if (!num.data->isNegative && data->isNegative)//前负后正
	{
		data->isNegative = true;
		metaAdd(this, bigOne, smallOne);
	}
	else if (num.data->isNegative && !data->isNegative)//前正后负
	{
		data->isNegative = false;
		metaAdd(this, bigOne, smallOne);
	}

	return *this;
}

ABigNum ABigNum::operator + (const ABigNum &num) const
{
	ABigNum temp;
	char *str = temp.data->base;
	ABigNum *bigOne, *smallOne;

	if (this->greater(num))
	{
		bigOne = const_cast<ABigNum *>(this);
		smallOne = const_cast<ABigNum *>(&num);
	}
	else
	{
		smallOne = const_cast<ABigNum *>(this);
		bigOne = const_cast<ABigNum *>(&num);
	}

	if (num.data->isNegative == data->isNegative)//同号
	{
		if (std::max(data->currentPos, num.data->currentPos) == temp.data->size)
		{
			temp.resetSize(2 * temp.data->size);
		}
		temp.data->isNegative = data->isNegative;
		metaAdd(&temp, bigOne, smallOne);
	}
	else//异号
	{
		if (bigOne == this)
		{
			temp.data->isNegative = false;
		}
		else
		{
			temp.data->isNegative = true;
		}
		metaSub(&temp, bigOne, smallOne);
	}

	return temp;
}

ABigNum ABigNum::operator - (const ABigNum &num) const
{
	ABigNum temp;
	char *str = temp.data->base;
	ABigNum *bigOne, *smallOne;

	if (this->greater(num))
	{
		bigOne = const_cast<ABigNum *>(this);
		smallOne = const_cast<ABigNum *>(&num);
	}
	else
	{
		smallOne = const_cast<ABigNum *>(this);
		bigOne = const_cast<ABigNum *>(&num);
	}

	if (num.data->isNegative && data->isNegative)//同为负
	{
		if (bigOne == this)
		{
			temp.data->isNegative = true;
		}
		else
		{
			temp.data->isNegative = false;
		}
		metaSub(&temp, bigOne, smallOne);
	}
	else if (!num.data->isNegative && !data->isNegative)//同为正
	{
		if (bigOne == this)
		{
			temp.data->isNegative = false;
		}
		else
		{
			temp.data->isNegative = true;
		}
		metaSub(&temp, bigOne, smallOne);
	}
	else if (!num.data->isNegative && data->isNegative)//前负后正
	{
		temp.data->isNegative = true;
		metaAdd(&temp, bigOne, smallOne);
	}
	else if (num.data->isNegative && !data->isNegative)//前正后负
	{
		temp.data->isNegative = false;
		metaAdd(&temp, bigOne, smallOne);
	}

	return temp;
}

void ABigNum::metaMulti(ABigNum *result, const ABigNum *bigOne, const ABigNum *smallOne) const
{
	std::vector<ABigNum> row(smallOne->data->currentPos);
	char *tempBase;

	if (bigOne->data->isNegative == bigOne->data->isNegative)
	{
		result->data->isNegative = false;
	}
	else
	{
		result->data->isNegative = false;
	}

	for (int i = 0; i<smallOne->data->currentPos; i++)
	{
		row[i].data->isNegative = false;
		row[i].resetSize(bigOne->data->currentPos+smallOne->data->currentPos);
		tempBase = row[i].data->base;
		memset(tempBase, 0, bigOne->data->currentPos + 1 + i);
		int it = i;

		for (int j = 0; j<bigOne->data->currentPos; j++)
		{
			tempBase[it] = smallOne->data->base[i] * bigOne->data->base[j];
			it++;
		}

		for (int k = i; k <it; k++)
		{
			while (tempBase[k] >= 10)
			{
				tempBase[k] -= 10;
				tempBase[k + 1]++;
			}
		}

		if (tempBase[it] != 0)
		{
			it++;
		}
		row[i].data->currentPos = it;
	}

	int size = smallOne->data->currentPos;
	result->data->currentPos = 0;
	for (int i = 0; i < size; i++)
	{
		*result += row[i];
	}

}

void ABigNum::metaDivi(ABigNum *result, const ABigNum *bigOne, const ABigNum *smallOne) const
{
	char *temp = result->data->base, *bigBase = bigOne->data->base, *smallBase = smallOne->data->base;
	ABigNum rest(*bigOne),tempNum;
	int smallHead, bigHead, iterator;
	bool flag=false;

	if (smallOne->data->currentPos == 1)
	{
		if (bigOne->data->currentPos == 1)
		{
			result->data->currentPos = 1;
			result->data->base[0] = 1;
			return;
		}
		flag = true;
	}

	if (bigOne->data->isNegative == smallOne->data->isNegative)
	{
		result->data->isNegative = false;
	}
	else
	{
		result->data->isNegative = true;
	}

	auto getHead = [&flag](const ABigNum *num)->int
	{
		int len = num->getLength();
		if (len == 1)
		{
			flag = true;
			return num->getBase()[0]; 
		}
		return  num->getBase()[len-1] * 10 + num->getBase()[len-2];
	};

	smallHead = getHead(smallOne);
	iterator = bigOne->data->currentPos - smallOne->data->currentPos;
	if (flag&&smallHead>getHead(&rest)/10)
	{
		iterator--;
	}
	memset(temp, 0, iterator+1);
	result->data->currentPos = iterator+1;

	for (;iterator>=0;)
	{
		iterator = rest.data->currentPos - smallOne->data->currentPos;
		bigHead = getHead(&rest);
		if (flag&&smallHead>bigHead / 10)
		{
			iterator--;
		}
		else if (flag&&smallHead <=bigHead / 10)
		{
			bigHead /= 10;
		}

		if (bigHead < smallHead)
		{
			iterator--;
			if (iterator < 0)
			{
				while (temp[result->data->currentPos-1]==0)
				{
					result->data->currentPos--;
				}
				if (result->data->currentPos == 0)
				{
					result->data->currentPos++;
				}
				return;
			}
			bigHead *= 10;
			bigHead += rest.data->base[rest.data->currentPos - 3];
		}
		
		temp[iterator] = bigHead / smallHead;
		while (true)
		{
			tempNum = (*result)*(*smallOne);
			if (tempNum.greater(*bigOne))
			{
				temp[iterator]--;
				break;
			}
			temp[iterator]++;
		}
	
		rest = *bigOne-(*result)*(*smallOne);
	}

}

void ABigNum::metaAdd(ABigNum *result,const ABigNum *bigOne, const ABigNum *smallOne) const
{
	char *temp = result->data->base, *bigBase = bigOne->data->base,*smallBase=smallOne->data->base;
	if (bigBase == temp)
	{
		temp[bigOne->data->currentPos] = 0;
		for (int i = 0; i < smallOne->data->currentPos; i++)
		{
			temp[i] = bigBase[i] + smallBase[i];
		}
	}
	else if (smallBase == temp)
	{
		memset(smallBase+smallOne->data->currentPos, 0,bigOne->data->currentPos-smallOne->data->currentPos+1);
		for (int i = 0; i < bigOne->data->currentPos; i++)
		{
			temp[i] = bigBase[i] + smallBase[i];
		}
	}
	else
	{
		memcpy(temp, bigBase, bigOne->data->currentPos);
		temp[bigOne->data->currentPos] = 0;
		for (int i = 0; i < smallOne->data->currentPos; i++)
		{
			temp[i] = bigBase[i] + smallBase[i];
		}
	}

	for (int i = 0; i < bigOne->data->currentPos+1; i++)
	{
		while (temp[i] >= 10)
		{
			temp[i] -= 10;
			temp[i + 1]++;
		}
	}

	if (temp[bigOne->data->currentPos] != 0)
	{
		result->data->currentPos = bigOne->data->currentPos + 1;
	}
	else
	{
		result->data->currentPos = bigOne->data->currentPos;
	}

}

void ABigNum::metaSub(ABigNum *result,const ABigNum *bigOne,const ABigNum *smallOne) const
{
	char *temp = result->data->base, *bigBase = bigOne->data->base, *smallBase = smallOne->data->base;
	if (bigBase == temp)
	{
		temp[bigOne->data->currentPos] = 0;
	}
	else if (smallBase == temp)
	{
		memset(smallOne->data->base + smallOne->data->currentPos, 0, bigOne->data->currentPos - smallOne->data->currentPos + 1);
	}
	else
	{
		memcpy(temp, bigBase, bigOne->data->currentPos);
		temp[bigOne->data->currentPos] = 0;
	}

	for (int i = 0; i < bigOne->data->currentPos; i++)
	{
		temp[i] = bigBase[i] - smallBase[i];
	}

	for (int i = 0; i < bigOne->data->currentPos; i++)
	{
		while (temp[i] < 0)
		{
			temp[i] += 10;
			temp[i + 1]--;
		}
	}

	result->data->currentPos = bigOne->data->currentPos;

	for (; (result->data->currentPos >= 2) && (temp[result->data->currentPos - 1] == 0); result->data->currentPos--);

}

void ABigNum::resetSize(int s)
{
	if (s < data->size)
	{
		return;
	}
	while (data->size < s)
	{
		data->size *= 2;
	}
	char *newbase = reinterpret_cast<char *>(realloc(data->base, data->size));
	if (newbase == nullptr)
	{
		throw ABadAlloc("void ABigNum::resetSize(int):Memory Allocation Failed");
	}
	data->base = newbase;
}

ATHENA_END