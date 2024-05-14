// StringGer.cpp : Defines the functions for the static library.
//
#include "StringGer.h"
#include <iostream>

namespace strGer {
	StringIterator::StringIterator(char* startPos, char* start, char* end) : position{ startPos }, start{ start }, end{ end }
	{}

	StringIterator::~StringIterator()
	{}

	StringIterator::StringIterator(const StringIterator& other) : position{ other.position }, start{ other.start }, end{ other.end }
	{}

	StringIterator& StringIterator::operator=(const StringIterator& other)
	{
		if (this == &other)
			return *this;

		this->end = other.end;
		this->start = other.start;
		this->position = other.position;

		return *this;
	}

	StringIterator& StringIterator::operator++()
	{
		if (position < end)
			position++;

		return *this;
	}

	StringIterator& StringIterator::operator++(int)
	{
		StringIterator temp(*this);
		if (position < end)
			position++;

		return temp;
	}

	StringIterator& StringIterator::operator--()
	{
		if (position > start)
			position--;

		return *this;
	}

	StringIterator& StringIterator::operator--(int)
	{
		StringIterator temp(*this);
		if (position > start)
			position--;

		return temp;
	}

	char& StringIterator::operator*()
	{
		return *position;
	}

	bool StringIterator::operator==(const StringIterator& other)
	{
		return position == other.position;
	}

	bool StringIterator::operator!=(const StringIterator& other)
	{
		return position != other.position;
	}

	bool StringIterator::operator>(const StringIterator& other)
	{
		return position > other.position;
	}

	bool StringIterator::operator<(const StringIterator& other)
	{
		return position < other.position;
	}

	bool StringIterator::operator>=(const StringIterator& other)
	{
		return position >= other.position;
	}

	bool StringIterator::operator<=(const StringIterator& other)
	{
		return position <= other.position;
	}
}
