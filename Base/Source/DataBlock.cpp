#include "../DataBlock.h"
#include "../NetSafeDataBlock.h"
#include "../DataBlockOut.h"
#include "../NetSafeDataBlockOut.h"

#ifdef WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif


void NetSafeDataBlockOut::WriteVal( uint16 val )
{
	val = htons( val );
	WriteVal_Defualt( val );
}

void NetSafeDataBlockOut::WriteVal( uint32 val )
{
	val = htonl( val );
	WriteVal_Defualt( val );
}

/// Retrieve an unsigned 32-bit integer
uint32 NetSafeDataBlock::ReadUint32()
{
	return ntohl( ReadVal<uint32>() );
}

/// Retrieve an unsigned 16-bit integer
uint16 NetSafeDataBlock::ReadUint16()
{
	return ntohs( ReadVal<uint16>() );
}