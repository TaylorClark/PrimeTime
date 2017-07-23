#ifndef __Serializer_h
#define __Serializer_h

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <typeinfo.h>

// A class used to read in/write out data
class Serializer
{
public:
	// constructors
	Serializer( std::ostream* pOutStream ) : m_pInStream(0)
	{
		m_pOutStream = pOutStream;
	}
	Serializer( std::istream* pInStream ) : m_pOutStream(0)
	{
		m_pInStream = pInStream;
	}

	// The generic data reader/writer
	template< class T >
	void AddData( T& obj )
	{
		typedef typename ObjectTraits<T>::cat objCat;
        AddData( obj, objCat() );		
	}

private:

	struct DefaultTag {};
	struct StringTag {};
	struct STLContainerTag {};

	template< class T >
	struct ObjectTraits
	{
		typedef DefaultTag cat;
	};

	template<>
	struct ObjectTraits< std::string >
	{
		typedef StringTag cat;
	};

	template< class T >
	struct ObjectTraits< std::list<T> >
	{
		typedef STLContainerTag cat;
	};
	template< class T >
	struct ObjectTraits< std::vector<T> >
	{
		typedef STLContainerTag cat;
	};

	// The generic data reader/writer
	template< class T >
	void AddData( T& obj, DefaultTag )
	{
		if( m_pOutStream )
			m_pOutStream->write( (const char*)&obj, sizeof(T) );
		else if( m_pInStream )
			m_pInStream->read( (char*)&obj, sizeof(T) );
	}

	// The specialized string function
	template< class T >
		void AddData( T& obj, StringTag )
	{
		if( m_pOutStream )
		{
			// Write the number of characters
			unsigned int numChars = (unsigned int)obj.size();
			m_pOutStream->write( (const char*)&numChars, sizeof(numChars) );

			// Write the string
			m_pOutStream->write( obj.c_str(), numChars );
		}
		else if( m_pInStream )
		{
			// Read in the number of characters
			unsigned int numChars = 0;
			m_pInStream->read( (char*)&numChars, sizeof(numChars) );

			// If there are no characters, bail
			if( numChars == 0 )
			{
				obj.clear();
				return;
			}

			// Create a buffer to hold the string
			std::string::allocator_type alloc = obj.get_allocator();
			char* szBuffer = alloc.allocate( numChars + 1 );

			// Read the string and NULL-terminate it
			m_pInStream->read( szBuffer, numChars );
			szBuffer[ numChars ] = 0;

			// Store the string and free the buffer
			obj = szBuffer;
			alloc.deallocate( szBuffer, numChars + 1 );
		}
	}

	// The list specialized function
	template< class T >
	void AddData( T& listObj, STLContainerTag )
	{
		if( m_pOutStream )
		{
			// Write out the size
			int listSize = (int)listObj.size();
			m_pOutStream->write( (const char*)&listSize, sizeof(listSize) );

			// Write out each item
			T::iterator iterItem = listObj.begin();
			for( ; iterItem != listObj.end(); ++iterItem )
				AddData( *iterItem );
		}
		else if( m_pInStream )
		{
			// Read in the size of the list
			int listSize = 0;
			m_pInStream->read( (char*)&listSize, sizeof(listSize) );
			listObj.clear();

			// Read in the list items
			typename T::value_type newItem;
			for( int itemIndex = 0; itemIndex < listSize; ++itemIndex )
			{
				AddData( newItem );
				listObj.push_back( newItem );
			}
		}
	}

private:
	// The input and output streams
	std::ostream* m_pOutStream;
	std::istream* m_pInStream;	
};


#endif // __Serializer_h