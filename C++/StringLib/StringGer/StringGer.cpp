// StringGer.cpp : Defines the functions for the static library.
//
#include "StringGer.h"
#include <iostream>
#include <cstring>

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
		if (position != end)
			return *position;
		else
			return const_cast<char&>(endChar);
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

	String::String() : string{new char[]{endChar}}, size{0}
	{}

	String::String(const char* charArray, size_t length)
	{
		size = length;
		string = new char[size + 1] {0};
		std::memcpy(string, charArray, size);
		*(string + size) = endChar;
	}

	String::String(const char* cstring)
	{
		long max = 10000000;
		size = 0;
		while (*(cstring + size) != '\0') {
			size++;
			if (size >= max) {
				size = max;
				break;
			}
		}

		string = new char[size + 1] {0};
		std::memcpy(string, cstring, size);
		*(string + size ) = endChar;
	}

	String::String(const String& other)
	{
		size = other.size;
		string = new char[size + 1]{ 0 };
		std::memcpy(string, other.string, size + 1);
	}

	String::String(String&& other)
	{
		size = other.size;
		other.size = 0;

		string = other.string;
		other.string = nullptr;
	}

	String& String::operator=(const String& other)
	{
		if (this == &other) {
			return *this;
		}

		size = other.size;
		std::memcpy(string, other.string, size + 1);
		return *this;
	}

	String& String::operator=(String&& other)
	{
		if (this == &other) {
			return *this;
		}

		size = other.size;
		other.size = 0;

		string = other.string;
		other.string = nullptr;

		return *this;
	}

	char String::operator[](int index)
	{
		return *(string + index);
	}

	String::~String()
	{
		delete[] string;
	}

	size_t String::getLength()
	{
		return size;
	}

	StringIterator String::begin()
	{
		return StringIterator(string, string, string + size + 1);
	}

	StringIterator String::end()
	{
		return StringIterator(string + size + 1, string, string + size + 1);
	}
}
