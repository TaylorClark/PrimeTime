//=================================================================================================
/*!
	\file ResourceDB.cpp
	Resource Manager Tool
	Resource Database Source
	\author Taylor Clark
	\date March 3, 2006

	This source file contains the implementation for resource database class.
*/
//=================================================================================================

#include "../ResourceToolsDB.h"
#include <fstream>
#include <windows.h>
#include "Base/StringFuncs.h"
#include "Base/Serializer.h"
#include "Base/NumFuncs.h"
#include <set>
#ifndef NOJPEG
#include "Jpeg/Corona.h"
#endif
#include "Base/FourCC.h"
#include "Base/NetSafeSerializer.h"
#include "Base/NetSafeDataBlockOut.h"
#include "../RLEBitmap.h"
//#include <Winsock2.h>


// Initialize the static variables
const FourCC ResourceToolsDB::FOURCCKEY_RESFILE( "GRES" );
const wchar_t* ResourceToolsDB::REGPATH_RootKeyPath = L"SOFTWARE\\PrimeTime Tools\\";
const wchar_t* ResourceToolsDB::REGPATH_ResPath = L"ResPath";
const wchar_t* ResourceToolsDB::REGPATH_GamePath = L"GamePath";
const wchar_t* ResourceToolsDB::FILENAME_ToolsResIndex = L"ToolsResIndex.dat";

const FourCC FOURCC_TOOLSDB( "TRDB" );

/// Get a predicted resource type based on file extension
static EResourceType GetResourceType( const std::wstring& sFileName );


/// Get the maximum resource ID
uint32 ResourceToolsDB::GetMaxResID() const
{
	uint32 retResID = 0;
	for( ResourceList::const_iterator iterRes = m_ResourceList.begin(); iterRes != m_ResourceList.end(); ++iterRes )
	{
		if( iterRes->resID > retResID )
			retResID = iterRes->resID;
	}

	return retResID;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::AddFile  Public
///
///	\param sFilePath The full file path of the file to add
///	\param pRetInfo A pointer to an object to retrieve information about the added resource, this
///					can be NULL
///	\returns True if the file is a valid resource and was added, false otherwise
///
///	Add a file to be compiled into the database.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ResourceToolsDB::AddFile( const std::wstring& sFilePath, ToolsResourceInfo* pRetInfo )
{
	// Get an predicted resource type to see if we should test for image/sounds first
	GameResourceFileInfo fileInfo;
	fileInfo.resType = GetResourceType( sFilePath );
	bool successfullyHandled = false;
	ToolsResourceInfo newRes;
	
	switch( fileInfo.resType )
	{
	case RT_Image:
		{
			bool isBitmap = sFilePath.find( L".bmp" ) != std::wstring::npos;

			if( isBitmap )
			{
				newRes.imgResType = IRT_BitmapRLE;

				// Attempt to load the image
				HANDLE hLoadedImg = LoadImage( NULL, sFilePath.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE );
				
				// If the image loaded
				if( hLoadedImg != NULL )
				{
					// Get the bitmap info to get the size
					BITMAP bmInfo;
					memset( &bmInfo, 0, sizeof(bmInfo) );
					GetObject( hLoadedImg, sizeof(bmInfo), &bmInfo );

					// Determine if the dimensions are powers of 2
					bool isImagePowerOf2 = (bmInfo.bmWidth & (bmInfo.bmWidth - 1)) == 0;
					isImagePowerOf2 &= ((bmInfo.bmHeight & (bmInfo.bmHeight - 1)) == 0 || bmInfo.bmHeight == 768 );

					// The image was successfully loaded
					DeleteObject( hLoadedImg );
					successfullyHandled = true;

					// If the dimensions are not a power of 2
					if( !isImagePowerOf2 )
					{
						/*std::wstring sMsg = L"The image file's (";
						sMsg += sFilePath;
						sMsg += L") dimensions are not a power of 2.  The image will be enlarged so that the width and height are a power of 2 and filled in with black.  It is \
						recommended that you enlarge the image to be a power of 2 yourself to make sure you get the full use out of the \
						image space.";*/
						std::wstring sMsg = L"The image file's (";
						sMsg += sFilePath;
						sMsg += L") dimensions are not a power of 2. The resource manager used to modify the size, but now its up to you. The only issue with image not a power-of-2 size is Direct3D increases the size in memory of the image to a power of 2, so it's image space that could have been used for something useful but isn't.";
						MessageBox( NULL, sMsg.c_str(), L"Image Size Problem", MB_OK );
					}
				}
				else
				{
					DWORD errorNum = GetLastError();
					int x = (int)errorNum;
				}
			}
			// Else it is a JPEG
			else
			{
				newRes.imgResType = IRT_Jpeg;
				successfullyHandled = true;
			}
		}
		break;

	case RT_Sound:
	case RT_Music:
		{
			// We have no sound checks right now
			successfullyHandled = true;
		}
		break;

	case RT_Font:
	case RT_Sprite:
		break;
	}
	
	// If we didn't handle the type then attempt to use it as a game resource
	if( !successfullyHandled )
	{
		// Get the resource type while determining if the file is a valid resource
		if( !GetResFileInfo( sFilePath, fileInfo ) )
			return false;
	}

	// Build the tools resource data
	newRes.sFilePath = TCBase::MakePathRelative( m_sResRootPath.c_str(), sFilePath.c_str() );
	newRes.resType = fileInfo.resType;
	newRes.sName = fileInfo.sName;
	if( fileInfo.resType != RT_Image )
		newRes.sResHeaderDefine = ConvertResToHeaderDefine( newRes );
	
	newRes.resID = m_NextResID++;

	// Store the resource
	m_ResourceList.push_back( newRes );

	// If the user wants to store the data
	if( pRetInfo )
		*pRetInfo = newRes;

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::RemoveResource  Public
///
///	\param resID The ID of the resource to remove
///	\returns True if the file was removed, false otherwise
///
///	Remove a file from the tools resource data file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ResourceToolsDB::RemoveResource( int resID )
{
	// Go through the resources and see if there is one with this ID
	for( ResourceList::iterator iterRes = m_ResourceList.begin(); iterRes != m_ResourceList.end(); ++iterRes )
	{
		// If this is the resource then remove it
		if( iterRes->resID == resID )
		{
			m_ResourceList.erase( iterRes );
			return true;
		}
	}

	// We did not remove the resource
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::ResTypeEnumToString  Static Public
///
///	\param type The resource type
///
///	Get a string defining the resource type.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t* ResourceToolsDB::ResTypeEnumToString( EResourceType type )
{
	switch( type )
	{
	case RT_Image:
		return L"Image";
	case RT_Sound:
		return L"Sound";
	case RT_Music:
		return L"Music";
	case RT_Font:
		return L"Font";
	case RT_Sprite:
		return L"Sprite";
	}

	return L"Unknown";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::ConvertResToHeaderDefine  Static Public
///
///	\param res The resource info
///
///	Get the string that will be used as the #define for this resource ID
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring ResourceToolsDB::ConvertResToHeaderDefine( const ResourceToolsDB::ToolsResourceInfo& res )
{
	std::wstring sRetString = L"RESID_";
	sRetString += ResourceToolsDB::ResTypeEnumToString( res.resType );
	sRetString += L"_";
	sRetString += res.sName;

	// Make the string all upper case
	const unsigned int BUF_SIZE = 512;
	wchar_t STR_BUF[BUF_SIZE] = {0};
	wcscpy_s( STR_BUF, BUF_SIZE, sRetString.c_str() );
	_wcsupr_s( STR_BUF, BUF_SIZE );
	
	// Replace spaces with underscores
	const size_t strLen = wcslen( STR_BUF );
	for( unsigned int charPos = 0; charPos < strLen; ++charPos )
	{
		if( STR_BUF[charPos] == L' ' )
			STR_BUF[charPos] = L'_';
	}

	// Store the string and bail
	sRetString = STR_BUF;
	return sRetString;
}


struct ResOutFile
{
	uint32 outFileIndex;
	uint32 numResourcesToWrite;
};

typedef std::map<std::wstring,ResOutFile> OutputResFileMap;

OutputResFileMap GetUniqueFiles( const ResourceToolsDB::ExportMap& resFileMap  )
{
	OutputResFileMap retMap;
	uint32 curIndex = 0;

	// Go through all of the resource entries
	for( ResourceToolsDB::ExportMap::const_iterator iterFile = resFileMap.begin(); iterFile != resFileMap.end(); ++iterFile )
	{
		// If the file has an empty output file string then skip it
		if( iterFile->second == L"" )
			continue;

		// If the file is already in the map then increment the resource count
		OutputResFileMap::iterator iterStr = retMap.find( iterFile->second );
		if( iterStr != retMap.end() )
		{
			iterStr->second.numResourcesToWrite++;
			continue;
		}

		// Create a new file entry
		ResOutFile newFile;
		newFile.outFileIndex = curIndex++;
		newFile.numResourcesToWrite = 1;
		retMap[ iterFile->second ] = newFile;
	}

	return retMap;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::CompileFiles  Public
///
///	\param sOutPath The output directory
///	\param outputMap The object that maps resource IDs to their output file name
///
///	Compile the files into a database
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ResourceToolsDB::CompileFiles( const std::wstring& sOutPath, const ExportMap& outputMap, ProgressCB pProgressCB )
{
	// Create the output files
	std::ofstream* outFiles;
	std::vector<uint32> outFileCurIndexOffset;
	std::vector<uint32> outFileCurDataOffset;

	// Create the resource ID header file
	std::wstring sOutFilePath = sOutPath + L"ResourceIDs.h";
	std::wofstream headerOutFile( sOutFilePath.c_str() );
	
	// Get the unique output files
	OutputResFileMap outResFileMap = GetUniqueFiles( outputMap );
	const uint32 numResFiles = (uint32)outResFileMap.size();
	if( numResFiles == 0 )
		throw L"No output files specified.";
	outFiles = new std::ofstream[ numResFiles ];
	outFileCurIndexOffset.resize( numResFiles );
	outFileCurDataOffset.resize( numResFiles );

	const wchar_t* HEADER_STR = L"//=================================================================================================\n//\n//\t\
THIS FILE IS AUTO GENERATED BY THE RESOURCE MANAGER.  DO NOT EDIT MANUALLY.\n//\n\
//=================================================================================================\n\n\
#pragma once\n#ifndef __ResourceIDs_h\n#define __ResourceIDs_h\n\n";

	headerOutFile << HEADER_STR;

	// Go through each of the output files and write the file headers
	for( OutputResFileMap::iterator iteroutfile = outResFileMap.begin(); iteroutfile != outResFileMap.end(); ++iteroutfile )
	{
		// get the index of this file within the resource file vector
		const uint32 resFileIndex = iteroutfile->second.outFileIndex;

		// create the game resource data file
		std::wstring sOutFilePath = sOutPath + iteroutfile->first;
		outFiles[resFileIndex].open( sOutFilePath.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );

		// if the file failed to load
		if( !outFiles[resFileIndex] )
		{
			// close the previous files
			for( int prevIndex = resFileIndex - 1; prevIndex >= 0; --prevIndex )
				outFiles[ prevIndex ].close();
			delete [] outFiles;
			headerOutFile.close();
			throw( "failed to open one of the output files for writing." );

			return;
		}

		NetSafeSerializer curResFile( outFiles + resFileIndex );

		// write out the key and the version
		int32 fourCC = ResourceMgr::FOURCCKEY_RESDB.ToInt32();
		curResFile.AddData( fourCC );
		
		uint32 resDBVersion = RES_DB_VER;
		curResFile.AddData( resDBVersion );

		// write the number of resources contained in this rdb file
		int32 numResources = iteroutfile->second.numResourcesToWrite;
		curResFile.AddData( numResources );
		
		// store the position in the file were the index starts
		outFileCurIndexOffset[resFileIndex] = static_cast<uint32>(outFiles[resFileIndex].tellp());

		// set up enough space for the resource indices
		size_t indexSize = numResources * sizeof(ResourceMgr::ResourceIndexItem);
		char* pFiller = new char[ indexSize ];
		memset( pFiller, 0, indexSize );
		outFiles[resFileIndex].write( pFiller, (std::streamsize)indexSize );
		delete [] pFiller;

		// store the position in the file were the data starts
		outFileCurDataOffset[resFileIndex] = static_cast<uint32>(outFiles[resFileIndex].tellp());
	}

	// Go through all of the files
	ResourceMgr::ResourceIndexItem resIndexItem;
	int32 numPacked = 0;
	for( ResourceList::iterator iterRes = m_ResourceList.begin(); iterRes != m_ResourceList.end(); ++iterRes )
	{
		// Get the output file index
		ExportMap::const_iterator iterResFile = outputMap.find( iterRes->resID );
		if( iterResFile == outputMap.end() )
			continue;
		
		std::wstring sOutFileName = iterResFile->second;
		if( sOutFileName == L"" )
			continue;

		if( pProgressCB )
			pProgressCB( (float32)numPacked / (float32)m_ResourceList.size(), iterRes->sFilePath.c_str() );

		const uint32 outFileIndex = outResFileMap[ sOutFileName ].outFileIndex;

		// Store a reference to the file stream
		NetSafeSerializer serializer( outFiles + outFileIndex );

		// Write the index information
		serializer.Seek( outFileCurIndexOffset[outFileIndex] );
		
		// Fill in the resource index item structure
		resIndexItem.dataOffset = outFileCurDataOffset[outFileIndex];
		resIndexItem.resourceID = iterRes->resID;
		resIndexItem.resType = iterRes->resType;
		memset( resIndexItem.szName, 0, ResourceMgr::RES_IDX_NAME_LEN * sizeof(wchar_t) );
		wcscpy_s( resIndexItem.szName, ResourceMgr::RES_IDX_NAME_LEN, iterRes->sName.c_str() );
		
		// Write out the index item
		resIndexItem.ToFromFile(serializer);
		
		// Update the index position
		outFileCurIndexOffset[outFileIndex] = serializer.GetOffset();

		// Write out the header file line
		if( iterRes->sResHeaderDefine.length() > 0 )
			headerOutFile << "#define " << iterRes->sResHeaderDefine << " " << iterRes->resID << std::endl;
		
		// Step to the data section
		serializer.Seek( outFileCurDataOffset[outFileIndex] );
		
		// Convert the resource to a data chunk
		try
		{
			uint8* pData = NULL;
			uint32 dataLen = 0;

			// Store the data
			PackData( *iterRes, &pData, &dataLen );

			// Write the data length out
			serializer.AddData( dataLen );
			
			// Write the data out
			serializer.AddRawData( pData, dataLen );
			
			if( pData && dataLen > 0 )
				delete [] pData;
		}
		catch( const wchar_t* szMsg )
		{
			// Display an error
			std::wstring sMsg = L"The file ";
			sMsg += iterRes->sFilePath;
			sMsg += L" had the following error during compilation: ";
			sMsg += szMsg;
			MessageBox( NULL, sMsg.c_str(), L"Resource Database Compilation Error", MB_OK | MB_ICONERROR );

			break;
		}

		// Update the data section
		outFileCurDataOffset[outFileIndex] = serializer.GetOffset();

		++numPacked;
	}

	// End the header file
	headerOutFile << "\n#endif // __ResourceIDs_h";
	headerOutFile.close();

	// Close the files
	for( uint32 resFileIndex = 0; resFileIndex < numResFiles; ++resFileIndex )
		outFiles[resFileIndex].close();
	delete [] outFiles;
}	


//struct PngDataChunk
//{
//	void* pData;
//	uint32 dataLen;
//};
//
//HBITMAP OpenPNG( const std::wstring& sFullFilePath )
//{
//	// Open the file
//	std::ifstream inPng( sFullFilePath.c_str(), std::ios_base::in | std::ios_base::binary );
//	if( !inPng )
//	{
//		MessageBox( NULL, sFullFilePath.c_str(), L"Failed to open PNG file", MB_OK );
//		return 0;
//	}
//
//	// Read the opening bytes
//	const uint8 pngSigBytes[] = {137, 80, 78, 71, 13, 10, 26, 10};
//	for( uint32 pngSigByteIndex = 0; pngSigByteIndex < 8; ++pngSigByteIndex )
//	{
//		uint8 val = 0;
//		inPng.read( (char*)&val, 1 );
//		if( val != pngSigBytes[pngSigByteIndex] )
//		{
//			++val;
//		}
//	}
//
//	// The PNG data
//	uint32 imgWidth = 0;
//	uint32 imgHeight = 0;
//	std::list<PngDataChunk> pngImgDataChunks;
//
//	// Read the chunks
//	const FourCC infoFourCC( "IHDR" );
//	const FourCC dataFourCC( "IDAT" );
//	const FourCC endFourCC( "IEND" );
//	for(;inPng.good();)
//	{
//		// Read this chunk's data length
//		uint32 curChunkDataLen = 0;
//		inPng.read( (char*)&curChunkDataLen, sizeof(curChunkDataLen) );
//		curChunkDataLen = ntohl( curChunkDataLen );
//
//		// Read in the chunk's type
//		uint32 curChunkTypeVal = 0;
//		inPng.read( (char*)&curChunkTypeVal, sizeof(curChunkTypeVal) );
//		FourCC curChunkType(curChunkTypeVal);
//
//		// If this is the info header
//		if( curChunkType == infoFourCC )
//		{
//			// Read the width and height
//			inPng.read( (char*)&imgWidth, sizeof(imgWidth) );
//			inPng.read( (char*)&imgHeight, sizeof(imgHeight) );
//			imgWidth = ntohl( imgWidth );
//			imgHeight = ntohl( imgHeight );
//
//			// Ensure the bit depth is 32
//			uint8 bitDepth = 0;
//			inPng.read( (char*)&bitDepth, sizeof(bitDepth) );
//			if( bitDepth != 32 )
//			{
//			}
//
//			// Read past the rest of the items
//			inPng.seekg( curChunkDataLen - 9, std::ios_base::cur );
//		}
//		// Else if this is the data chunk
//		else if(curChunkType == dataFourCC )
//		{
//			// Create the chunk
//			PngDataChunk newData;
//			newData.dataLen = curChunkDataLen;
//			newData.pData = new uint8[ curChunkDataLen ];
//
//			// Read the data
//			inPng.read( (char*)newData.pData, curChunkDataLen );
//
//			// Store the chunk
//			pngImgDataChunks.push_back( newData );
//
//		}
//		// Else if this is the end chunk
//		else if(curChunkType == endFourCC )
//			break;
//		// Else skip ahead
//		else
//			inPng.seekg( curChunkDataLen, std::ios_base::cur );
//
//		// Skip the CRC
//		inPng.seekg( 4, std::ios_base::cur );
//	}
//
//	// Close the file
//	inPng.close();
//
//	
//	// Merge all of the data chunks into one
//	uint32 pngImgDataLen = 0;
//	for( std::list<PngDataChunk>::iterator iterCurDat = pngImgDataChunks.begin(); iterCurDat != pngImgDataChunks.end(); ++iterCurDat )
//		pngImgDataLen += iterCurDat->dataLen;
//	uint8* pPngImgData = new uint8[ pngImgDataLen ];
//	uint32 curDataOffset = 0;
//	for( std::list<PngDataChunk>::iterator iterCurDat = pngImgDataChunks.begin(); iterCurDat != pngImgDataChunks.end(); ++iterCurDat )	
//	{
//		// Copy the memory from the chunk, increment the offset, and free the chunk data
//		memcpy( pPngImgData + curDataOffset, iterCurDat->pData, iterCurDat->dataLen );
//		curDataOffset += iterCurDat->dataLen;
//		delete [] iterCurDat->pData;
//	}
//	pngImgDataChunks.clear();
//
//
//	// Fill in the bitmap info header
//	BITMAPINFO bmInfo;
//	bmInfo.bmiHeader.biBitCount = 0;
//	bmInfo.bmiHeader.biClrImportant = 0;
//	bmInfo.bmiHeader.biClrUsed = 0;
//	bmInfo.bmiHeader.biCompression = BI_PNG;
//	bmInfo.bmiHeader.biHeight = imgHeight;
//	bmInfo.bmiHeader.biPlanes = 1;
//	bmInfo.bmiHeader.biSize = sizeof( bmInfo.bmiHeader );
//	bmInfo.bmiHeader.biSizeImage = pngImgDataLen;
//	bmInfo.bmiHeader.biWidth = imgWidth;
//	bmInfo.bmiHeader.biXPelsPerMeter = 72;
//	bmInfo.bmiHeader.biYPelsPerMeter = 72;
//
//	// Create a device context to work with the data
//	HDC hDC = CreateCompatibleDC( NULL );
//	HBITMAP retBmp = CreateDIBitmap( hDC, &bmInfo.bmiHeader, CBM_INIT, pPngImgData, &bmInfo, DIB_RGB_COLORS );
//	DWORD lastError = GetLastError();
//	DeleteDC( hDC );
//
//	return retBmp;
//}


void PackBitmap( const std::wstring& sFullFilePath, uint8** ppData, uint32* pDataLen )
{
	// Load the image
	HBITMAP hLoadedImage = (HBITMAP)LoadImage( NULL,
									sFullFilePath.c_str(),
									IMAGE_BITMAP,
									0,
									0,
									LR_DEFAULTSIZE | LR_LOADFROMFILE );

	// Test if the image loaded
	if( !hLoadedImage )
		throw L"Failed to load image, ensure the path is valid and it is in the bitmap format.";

	// Get the DIB section to describe the bitmap
	DIBSECTION dibSection;
	if( GetObject( hLoadedImage, sizeof(dibSection), &dibSection ) == 0 )
		throw L"Bitmap could not be loaded in a format containing the data necessary to read it.";

	// Ensure a valid format
	if( dibSection.dsBm.bmBitsPixel < 24 )
		throw L"The image color depth is too low, ensure it is saved with at least 24-bit RGB color.";
	else if( dibSection.dsBm.bmBitsPixel > 32 )
		throw L"The image color depth is too low, ensure it is saved with at most 32-bit RGB color.";

	// Get the absolute value of the height in case the image it top down
	int absDIBHeight = (dibSection.dsBm.bmHeight > 0) ? dibSection.dsBm.bmHeight : -dibSection.dsBm.bmHeight;

	// Determine if the dimensions are powers of 2
	bool isWidthPowerOf2 = (dibSection.dsBm.bmWidth & (dibSection.dsBm.bmWidth - 1)) == 0;
	bool isHeightPowerOf2 = (absDIBHeight & (absDIBHeight - 1)) == 0;

	// Get the color flags
	DWORD redFlags = dibSection.dsBitfields[0];
	DWORD greenFlags = dibSection.dsBitfields[1];
	DWORD blueFlags = dibSection.dsBitfields[2];

	// If the dimensions are not a power of 2
	int modifiedBMWidth = dibSection.dsBm.bmWidth;
	//if( !isWidthPowerOf2 )
	//{
	//	// Upgrade the width to the next power of 2
	//	int leftMost1 = TCBase::GetLeftMost1Index( modifiedBMWidth );
	//	modifiedBMWidth = 1 << (leftMost1 + 1);
	//}
	int modifiedBMHeight = absDIBHeight;
	//if( !isHeightPowerOf2 && modifiedBMHeight != 768 )
	//{
	//	// Upgrade the width to the next power of 2
	//	int leftMost1 = TCBase::GetLeftMost1Index( modifiedBMHeight );
	//	modifiedBMHeight = 1 << (leftMost1 + 1);
	//}
	
	// Create a device context to work with the data
	HDC hDC = CreateCompatibleDC( NULL );
	
	// Fill in the bitmap info header
	BITMAPINFO bmInfo;
	bmInfo.bmiHeader.biBitCount = dibSection.dsBm.bmBitsPixel;
	bmInfo.bmiHeader.biClrImportant = 0;
	bmInfo.bmiHeader.biClrUsed = 0;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biHeight = dibSection.dsBm.bmHeight;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biSize = sizeof( bmInfo.bmiHeader );
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biWidth = dibSection.dsBm.bmWidth;
	bmInfo.bmiHeader.biXPelsPerMeter = 72;
	bmInfo.bmiHeader.biYPelsPerMeter = 72;

	// Get the bitmap data
	unsigned int dataLen = dibSection.dsBm.bmWidthBytes * absDIBHeight;
	unsigned char* pDIBPixelData = new unsigned char[ dataLen ];
	GetDIBits( hDC, hLoadedImage, 0, dibSection.dsBm.bmHeight, (void*)pDIBPixelData, &bmInfo, DIB_RGB_COLORS );
	
	// Free the device context and bitmap
	DeleteDC( hDC );
	DeleteObject( hLoadedImage );

	// Get the size of the data block
	// width dword, height dword, width * height * 3 rgb arrays of pixels top-down
	*pDataLen = (sizeof(int32) * 3) + (modifiedBMWidth * modifiedBMHeight * 3);

	// Create the buffer to store the data
	*ppData = new uint8[ *pDataLen ];

	// Store the dimensions of the image
	int32* pIntPtr = (int32*)*ppData;
	*pIntPtr = (int32)modifiedBMWidth;
	*(pIntPtr + 1) = (int32)modifiedBMHeight;

	// Store the type
	*(pIntPtr + 2) = (int32)IRT_Bitmap;

	// Handle if the data is bottom-up or top-down
	int rowStep = 1;
	int rowStartIndex = 0;
	int rowEndIndex = dibSection.dsBm.bmHeight;

	// If the bitmap is bottom up
	if( dibSection.dsBm.bmHeight > 0 )
	{
		rowStep = -1;
		rowStartIndex = dibSection.dsBm.bmHeight - 1;
		rowEndIndex = -1;
	}

	// If we using 32-bit color
	int redOffset = 0;
	if( dibSection.dsBm.bmBitsPixel == 32 )
		redOffset = 1;

	// Store an uint8 pointer to our data for less type casting
	uint8* pStoreData = (uint8*)(*ppData + (sizeof(int32) * 3));

	// Go through the pixel data
	unsigned int dataOffset = 0;
	const unsigned int pixelStep = dibSection.dsBm.bmBitsPixel / 8;
	for( int rowIndex = rowStartIndex; rowIndex != rowEndIndex; rowIndex += rowStep )
	{
		// Get a pointer to this row of pixels on the source image
		uint8* pSrcRow = pDIBPixelData + (rowIndex * dibSection.dsBm.bmWidthBytes);

		// Go through the pixels
		int colIndex = 0;
		for( ; colIndex < dibSection.dsBm.bmWidth; ++colIndex )
		{
			// Store the color components, red green blue
			int pixelOffset = (colIndex * pixelStep);
			pStoreData[ dataOffset++ ] = pSrcRow[ pixelOffset + 2 ];
			pStoreData[ dataOffset++ ] = pSrcRow[ pixelOffset + 1 ];
			pStoreData[ dataOffset++ ] = pSrcRow[ pixelOffset + 0 ];
		}

		// If the image width was enlarged up to a power of 2
		for( ; colIndex < modifiedBMWidth; ++colIndex )
		{
			pStoreData[ dataOffset++ ] = 0;
			pStoreData[ dataOffset++ ] = 0;
			pStoreData[ dataOffset++ ] = 0;
		}
	}

	// If the image width was enlarged up to a power of 2
	if( absDIBHeight < modifiedBMHeight )
	{
		// Fill in the remaining rows with 0s
		for( int rowIndex = absDIBHeight; rowIndex < modifiedBMHeight; ++rowIndex )
		{
			for( int xIndex = 0; xIndex < modifiedBMWidth; ++xIndex )
			{
				pStoreData[ dataOffset++ ] = 0;
				pStoreData[ dataOffset++ ] = 0;
				pStoreData[ dataOffset++ ] = 0;
			}
		}
	}

	// Free the DIBits
	delete [] pDIBPixelData;
}

void PackJpeg( const std::wstring& sFullFilePath, uint8** ppData, uint32* pDataLen )
{
#ifndef NOJPEG
	// Open the jpeg resource file
	std::string sAsciiPath = TCBase::Narrow( sFullFilePath.c_str() );
	corona::File* pJpgFile = corona::OpenFile( sAsciiPath.c_str(), false );
	if( !pJpgFile )
		throw L"Failed to open the Jpeg resource file.";

	// Get the file size
	pJpgFile->seek( 0, corona::File::END );
	uint32 fileSize = pJpgFile->tell();
	pJpgFile->seek( 0, corona::File::BEGIN );
	if( fileSize == 0 )
		throw L"Empty Jpeg file.";

	// Store the data size (The file size plus the image headings)
	*pDataLen = fileSize + (sizeof(int32) * 3);

	// Allocate memory for all of the data
	*ppData = new uint8[*pDataLen];

	// Open the image to retrieve the dimensions
	corona::Image* pImage = corona::OpenJPEG( pJpgFile );
	if( !pImage )
	{
		delete pJpgFile;
		throw L"Failed to read jpeg resource.";
	}

	// Store the dimensions of the image
	NetSafeDataBlockOut dataOut( *ppData, *pDataLen );
	dataOut.WriteVal( static_cast<int32>( pImage->getWidth() ) );
	dataOut.WriteVal( static_cast<int32>( pImage->getHeight() ) );
	dataOut.WriteVal( static_cast<int32>( IRT_Jpeg ) );

	// Close the image
	delete pImage;
	pImage = 0;

	// Read in the entire file
	pJpgFile->seek( 0, corona::File::BEGIN );
	pJpgFile->read( *ppData + dataOut.GetCurPosition(), fileSize );

	// Close the file
	delete pJpgFile;
#endif
}

void ResourceToolsDB::GameResourceFileInfo::Transfer(TCBase::ISerializer& serializer)
{
	int32 tempVal = fourCCKey.ToInt32();
	serializer.AddData(	tempVal );
	fourCCKey.FromInt32( tempVal );

	tempVal = static_cast<int32>( resType );
	serializer.AddData(	tempVal );
	resType = static_cast<EResourceType>( tempVal );

	if( serializer.InReadMode() )
		TCBase::ReadChars( serializer, sName, ResourceMgr::RES_IDX_NAME_LEN );
	else
		TCBase::WriteChars( serializer, sName, ResourceMgr::RES_IDX_NAME_LEN );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::PackData  Public
///
///	\param resInfo The resource to load into a memory block
///	\param ppData A pointer to a char* that will be used to store the resource data, ensure this
///					this data is free with delete []
///	\param pDataLen A pointer to an integer to store the length of the data block
///
///	\exception If there is failure, a const char* exception is thrown containing a string
///				describing the problem
///
///	Convert a resource into a block of memory to be inserted into a resource data file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ResourceToolsDB::PackData( const ToolsResourceInfo& resInfo, uint8** ppData, uint32* pDataLen ) const
{
	if( !ppData || !pDataLen )
		throw L"NULL pointers for data to fill in is not allowed.";

	// Get the full file path
	std::wstring sFullFilePath = TCBase::CombinePaths( m_sResRootPath, resInfo.sFilePath );

	// Handle the type
	switch( resInfo.resType )
	{
		case RT_Image:
			{
				// If the image is a bitmap
				if( resInfo.imgResType == IRT_Bitmap )
					PackBitmap( sFullFilePath, ppData, pDataLen );
				else if( resInfo.imgResType == IRT_BitmapRLE )
				{
					// Attempt to load the image
					HANDLE hLoadedImg = LoadImage( NULL, sFullFilePath.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE );

					// If the image loaded
					if( hLoadedImg != NULL )
					{
						// Get the bitmap info to get the size
						BITMAP bmInfo;
						memset( &bmInfo, 0, sizeof(bmInfo) );
						GetObject( hLoadedImg, sizeof(bmInfo), &bmInfo );

						// Create a device context to work with the data
						HDC hDC = CreateCompatibleDC( NULL );
						HGDIOBJ hPrevObj = SelectObject( hDC, hLoadedImg );

						// Encode the image
						RLEEncodeInfo rleInfo = RLEEncodeBitmap( hDC, bmInfo.bmWidth, bmInfo.bmHeight );

						// Free the GDI objects
						SelectObject( hDC, hPrevObj );
						DeleteDC( hDC );
						DeleteObject( hLoadedImg );

						// Store the data size (The file size plus the image headings)
						*pDataLen = rleInfo.dataSize + (sizeof(int32) * 3);

						// Allocate memory for all of the data
						*ppData = new uint8[*pDataLen];

						NetSafeDataBlockOut dataOut( *ppData, *pDataLen );

						// Store the width, height, then type
						dataOut.WriteVal( static_cast<int32>( bmInfo.bmWidth ) );
						dataOut.WriteVal( static_cast<int32>( bmInfo.bmHeight ) );
						dataOut.WriteVal( static_cast<int32>( IRT_BitmapRLE ) );

						// Go through the runs
						//uint32* pPixelData = (uint32*)( *ppData + (sizeof(int32) * 3) );
						const uint32 NON_MATCHING_RUN_FLAG = 0x80000000;
						for( RunList::iterator iterRun = rleInfo.colorRuns.begin(); iterRun != rleInfo.colorRuns.end(); ++iterRun )
						{
							// If it is a non-matching run
							if( iterRun->length == iterRun->colors.size() )
							{
								// Write the count with the non-matching flag set
								dataOut.WriteVal( iterRun->length | NON_MATCHING_RUN_FLAG );
								//*pPixelData = iterRun->length | NON_MATCHING_RUN_FLAG;
								//pPixelData++;

								// Write each color
								for( std::list<uint32>::iterator iterColor = iterRun->colors.begin(); iterColor != iterRun->colors.end(); ++iterColor )
								{
									dataOut.WriteVal( *iterColor );
									//*pPixelData = *iterColor;
									//pPixelData++;
								}
							}
							// Else it is a matching run
							else
							{
								// Write the count with the non-matching flag set
								dataOut.WriteVal( iterRun->length );
								//*pPixelData = iterRun->length;
								//pPixelData++;

								// Write the color
								dataOut.WriteVal( iterRun->colors.front() );
								//*pPixelData = iterRun->colors.front();
								//pPixelData++;
							}
						}
					}
				}
				else
					PackJpeg( sFullFilePath, ppData, pDataLen );
				
			}
			break;

		case RT_Sound:
		case RT_Music:
			{
				// Open the sound resource file
				std::ifstream inFile( sFullFilePath.c_str(), std::ios_base::in | std::ios_base::binary );
				if( !inFile )
					throw L"Failed to open the sound resource file.";

				// Get the file size
				inFile.seekg( 0, std::ios_base::end );
				uint32 fileSize = (uint32)inFile.tellg();

				if( fileSize < 1 )
					throw L"Empty sound file.";
				*pDataLen = fileSize;

				// Allocate memory for the whole file
				*ppData = new uint8[fileSize];

				// Read in the file data
				inFile.seekg( 0, std::ios_base::beg );
				inFile.read( (char*)*ppData, fileSize );

				// Close the file
				inFile.close();
			}
			break;

		case RT_Font:
		case RT_Sprite:
			{
				// Open the game resource file
				std::ifstream inFile( sFullFilePath.c_str(), std::ios_base::in | std::ios_base::binary );
				if( !inFile )
					throw L"Failed to open the game resource file.";

				NetSafeSerializer serializer( &inFile );

				// Read in the game resource file header
				GameResourceFileInfo gameResHeader;
				gameResHeader.Transfer( serializer );
				
				// Ensure it is valid game resource
				if( !inFile || gameResHeader.fourCCKey != ResourceToolsDB::FOURCCKEY_RESFILE )
				{
					inFile.close();
					throw L"File is not a valid game resource.";
				}

				// Get the remaining amount of file data
				uint32 dataStartPos = serializer.GetOffset();
				uint32 dataEndPos = serializer.GetInputLength();
				if( dataEndPos < dataStartPos || dataStartPos == dataEndPos )
				{
					inFile.close();
					throw L"Game resource file is of invalid size.";
				}
				*pDataLen = dataEndPos - dataStartPos;

				// Allocate memory for the data
				*ppData = new uint8[ *pDataLen ];

				// Read in the data
				serializer.AddRawData( *ppData, *pDataLen );

				// Close the file
				inFile.close();
			}
			break;

		default:
			throw L"Unknown resource type.";
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::SaveToolsIndex  Public
///
///	\param sOutFile The output file name
///
///	Save a tools version of the database.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ResourceToolsDB::SaveToolsIndex( const std::wstring& sOutFile )
{
	// Create the output file stream
	std::ofstream outFile( sOutFile.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
	if( !outFile )
		return false;

	Serializer serializer( &outFile );
	TransferToolsIndex( serializer );
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::LoadToolsIndex  Public
///
///	\param sInFile The full file path of the file to load
///
///	Save a tools version of the database.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ResourceToolsDB::LoadToolsIndex( const wchar_t* szInFileName )
{
	// Create the input file stream
	std::ifstream inFile( szInFileName, std::ios_base::in | std::ios_base::binary );
	if( !inFile )
		return false;

	Serializer serializer( &inFile );
	TransferToolsIndex( serializer );
	return true;
}


void ResourceToolsDB::TransferToolsIndex( TCBase::ISerializer& serializer )
{
	// Transfer the fourCC code
	int fourCCVal = FOURCC_TOOLSDB.ToInt32();
	serializer.AddData( fourCCVal );
	if( serializer.InReadMode() )
	{
		// Ensure a proper key
		if( fourCCVal != FOURCC_TOOLSDB.ToInt32() )
			return;
	}

	// The version of the resource manager
	uint32 fileVersion = TOOLSDB_VERSION;
	serializer.AddData( fileVersion );

	if( fileVersion == 1 )
		TransferToolsIndexV1(serializer);
	else if( fileVersion == 2 )
		TransferToolsIndexV2(serializer);	
}

void ResourceToolsDB::TransferToolsIndexV1( TCBase::ISerializer& serializer )
{
	// The number of resources
	int numResources = (int)m_ResourceList.size();
	serializer.AddData( numResources );

	// If we are loading then prepare the list for the items by resizing and reset the max ID value
	if( serializer.InReadMode() )
	{
		m_ResourceList.resize( numResources );
		m_NextResID = ResourceMgr::STARTING_RES_ID;
	}

	// Each resource
	for( ResourceList::iterator iterRes = m_ResourceList.begin(); iterRes != m_ResourceList.end(); ++iterRes )
	{
		// The name
		serializer.AddData( iterRes->sName );

		// The file path
		serializer.AddData( iterRes->sFilePath );

		// The type
		uint32 resType = iterRes->resType;
		serializer.AddData( resType );
		iterRes->resType = static_cast<EResourceType>( resType );

		// The resource ID header define identifier
		serializer.AddData( iterRes->sResHeaderDefine );

		// The ID
		serializer.AddData( iterRes->resID );

		// The image type
		uint32 imageResType = iterRes->imgResType;
		serializer.AddData( imageResType );
		iterRes->imgResType = static_cast<EImageResourceType>( imageResType );
 
		// If this resource ID is greater than our current max then store it
		if( iterRes->resID >= m_NextResID )
			m_NextResID = iterRes->resID + 1;
	}
}

void ResourceToolsDB::TransferToolsIndexV2( TCBase::ISerializer& serializer )
{
	// The resource path
	serializer.AddData( m_sResRootPath );

	// The number of resources
	int numResources = (int)m_ResourceList.size();
	serializer.AddData( numResources );

	// If we are loading then prepare the list for the items by resizing and reset the max ID value
	if( serializer.InReadMode() )
	{
		m_ResourceList.resize( numResources );
		m_NextResID = ResourceMgr::STARTING_RES_ID;
	}

	// Each resource
	for( ResourceList::iterator iterRes = m_ResourceList.begin(); iterRes != m_ResourceList.end(); ++iterRes )
	{
		// The name
		serializer.AddData( iterRes->sName );

		// The file path
		serializer.AddData( iterRes->sFilePath );

		// The type
		uint32 resType = iterRes->resType;
		serializer.AddData( resType );
		iterRes->resType = static_cast<EResourceType>( resType );

		// The resource ID header define identifier
		serializer.AddData( iterRes->sResHeaderDefine );

		// The ID
		serializer.AddData( iterRes->resID );

		// The image type
		uint32 imageResType = iterRes->imgResType;
		serializer.AddData( imageResType );
		iterRes->imgResType = static_cast<EImageResourceType>( imageResType );

		// If this resource ID is greater than our current max then store it
		if( iterRes->resID >= m_NextResID )
			m_NextResID = iterRes->resID + 1;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::GetResFileInfo  Public
///
///	\param sFileName The full file path of the file for which we are retrieving info
///	\param retItem The resource file info to fill in
///	\returns Whether or not the file is a valid resource file
///
///	Retrieve information about a cadidate for a resource file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ResourceToolsDB::GetResFileInfo( const std::wstring& sFileName, GameResourceFileInfo& retItem )
{
	// Open the file stream
	std::ifstream inFile( sFileName.c_str(), std::ios_base::in | std::ios_base::binary );
	if( !inFile )
		return false;

	NetSafeSerializer resSerializer( &inFile );
	retItem.Transfer( resSerializer );
	
	// If the data failed to read in or the key is not that of a valid game resource
	if( !inFile || retItem.fourCCKey != ResourceToolsDB::FOURCCKEY_RESFILE )
	{
		inFile.close();
		return false;
	}

	// Close the file
	inFile.close();

	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::GetNumResOfType  Public
///
///	\param resType The type of resources that we are counting.
/// \returns The number of resources in the database that match the passed-in type
///
///	Get the number of resources in the database of a specified type.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int ResourceToolsDB::GetNumResOfType( EResourceType resType ) const
{
	// Go through all of the resources in the database
	int resCount = 0;
	for( ResourceList::const_iterator iterFile = m_ResourceList.begin(); iterFile != m_ResourceList.end(); ++iterFile )
	{
		// If this file matches our type
		if( iterFile->resType == resType )
			++resCount;
	}

	// Return the count
	return resCount;
}


uint32 GetFileSize( const wchar_t* szFileName )
{
	std::ifstream inFile( szFileName );
	if( !inFile )
		return 0;

	inFile.seekg( 0, std::ios_base::end );
	uint32 retVal = (uint32)inFile.tellg();

	inFile.close();

	return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GetResourceType  Public
///
///	\param sFileName The full file name
///
///	Get the resource type from the file extension.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
EResourceType GetResourceType( const std::wstring& sFileName )
{
	// Get the file extension
	wchar_t szExt[ _MAX_PATH ] = {0};
	_wsplitpath_s( sFileName.c_str(), NULL, 0, NULL, 0, NULL, 0, szExt, _MAX_PATH );
	std::wstring sExt = szExt + 1;
	if( sExt.length() < 3 )
		return RT_Error;

	// Make sure the extension is all caps
	wchar_t* szUpperBuffer = new wchar_t[ sExt.length() + 1 ];
	wcscpy_s( szUpperBuffer, sExt.length() + 1, sExt.c_str() );
	_wcsupr_s( szUpperBuffer, sExt.length() + 1 );
	sExt = szUpperBuffer;
	delete [] szUpperBuffer;

	// Compare the extensions
	if( sExt.compare( L"BMP" ) == 0
		|| sExt.compare( L"JPG" ) == 0 )
	{
		return RT_Image;
	}
	else if( sExt.compare( L"WAV" ) == 0 )
		return RT_Sound;
	else if( sExt.compare( L"MP3" ) == 0 || sExt.compare( L"OGG" ) == 0 )
	{
		// Get the file size (songs are considered music files if they are larger than 100k)
		const uint32 MUSIC_THRESHOLD = 1024 * 100;
		uint32 fileSize = GetFileSize( sFileName.c_str() );
		if( fileSize > MUSIC_THRESHOLD )
			return RT_Music;

		return RT_Sound;
	}
	else if( sExt.compare( L"FNT" ) == 0 )
		return RT_Font;
	else if( sExt.compare( L"SPR" ) == 0 )
		return RT_Sprite;

	return RT_Error;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::GetResInfo  Public
///
///	\param resID The resource ID for which we are finding the info
///	\param resInfo The resource info structure to fill in if the resource is found
///	\returns True if the resource was found, false otherwise
///
///	Get the info about a resource.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ResourceToolsDB::GetResInfo( ResourceID resID, ToolsResourceInfo& resInfo )
{
	// Go through the list and find the requested resource
	for( ResourceList::iterator iterRes = m_ResourceList.begin(); iterRes != m_ResourceList.end(); ++iterRes )
	{
		// If this is the resource for which we are looking
		if( iterRes->resID == resID )
		{
			// Store the data and return success
			resInfo = *iterRes;
			return true;
		}
	}

	// We did not find the resource so return failure
	return false;
}

/// Update a resource entry
void ResourceToolsDB::UpdateResourceInfo( ResourceID oldResID, const ResourceToolsDB::ToolsResourceInfo& newInfo )
{
	// If the resource ID was updated
	if( oldResID != newInfo.resID )
	{
		// Ensure the ID is not in use
		ToolsResourceInfo resInfo;
		if( GetResInfo( newInfo.resID, resInfo ) )
		{
			// There is already a resource with this ID
			throw( L"Failed to change resource ID since there is already a resource with the new ID" );
		}
	}

	// Go through the list and find the requested resource
	for( ResourceList::iterator iterRes = m_ResourceList.begin(); iterRes != m_ResourceList.end(); ++iterRes )
	{
		// If this is the resource for which we are looking
		if( iterRes->resID == oldResID )
		{
			// Store the data and return success
			*iterRes = newInfo;
			break;
		}
	}

	// Update the next resource ID if needed
	m_NextResID = GetMaxResID() + 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::Clear  Public
///
///	Clear the tool resource database.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ResourceToolsDB::Clear()
{
	// Clear the data
	m_ResourceList.clear();
	m_NextResID = ResourceMgr::STARTING_RES_ID;
	m_sResRootPath = L"";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ResourceToolsDB::SetNextResID  Public
///
/// Set the next resource ID if it is valid
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ResourceToolsDB::SetNextResID( uint32 nextResID )
{
	// If the next res ID is in a range that already contains used IDs then ignore it
	if( nextResID < GetMaxResID() || nextResID < ResourceMgr::STARTING_RES_ID )
		return false;

	// Store the new ID
	m_NextResID = nextResID;
	return true;
}