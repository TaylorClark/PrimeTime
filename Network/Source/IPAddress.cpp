#include "../IPAddress.h"
#include <winsock2.h>


IPAddress::IPAddress( const char* szIPAddress )
{
	m_IPAddress = inet_addr( szIPAddress );
}

/// Retrieve the value as a string
const char* IPAddress::ToString() const
{
	in_addr ipVal;
	ipVal.S_un.S_addr = (u_long)m_IPAddress;
	return inet_ntoa( ipVal );
}