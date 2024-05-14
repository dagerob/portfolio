#pragma once

namespace strGer
{
	constexpr char endChar = '\0';
	
	class String;

	class StringIterator {
		private:
			char* position;
			char* start;
			char* end;
		public:
			StringIterator(char* startPos, char* start, char* end);
			~StringIterator();
			StringIterator(const StringIterator& other);
			StringIterator& operator=(const StringIterator& other);
			StringIterator& operator++();
			StringIterator& operator++(int);
			StringIterator& operator--();
			StringIterator& operator--(int);
			char& operator*();
			bool operator==(const StringIterator& other);
			bool operator!=(const StringIterator& other);
			bool operator>(const StringIterator& other);
			bool operator<(const StringIterator& other);
			bool operator>=(const StringIterator& other);
			bool operator<=(const StringIterator& other);
	};

	class String {
	private:
		char* string;
		size_t size;
	public:
		String();
		String(const String& other);
		String(String&& other);
		String& operator=(const String& other);
		String& operator=(String&& other);
		~String();
		size_t getLength();
		StringIterator findChar();
		StringIterator begin();
		StringIterator end();
	};
}