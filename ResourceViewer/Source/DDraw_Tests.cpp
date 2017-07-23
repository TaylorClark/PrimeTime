#include "Graphics2D/GraphicsMgr.h"
#include "Graphics2D/TCImage.h"
#include "Base/MsgLogger.h"
#include <windows.h>
#include "Math/Box2i.h"
#include <list>
#include "Base/NumFuncs.h"


typedef void (*TestFunc)();
const uint32 NUM_TEST_RUNS = 40;
static TCImage* g_pImage = 0;


class GfxTest
{
public:
	virtual ~GfxTest(){}

	virtual void Setup(){}

	virtual void RunTest(){}

	virtual void TearDown(){}

	virtual const wchar_t* GetName() const = 0;
};

class DrawFullImageTest : GfxTest
{
	Box2i m_SrcRect;

public:

	virtual void Setup()
	{		
		m_SrcRect.Set( 0, 0, g_pImage->GetDims().x, g_pImage->GetDims().y );
	}

	virtual void RunTest()
	{
		g_pGraphicsMgr->DrawImage( Point2i(), g_pImage, m_SrcRect );
	}

	virtual const wchar_t* GetName() const
	{
		return L"Drawing full image";
	}
};

class DrawFullScreenTest : GfxTest
{
	int32 m_Divs;
	std::list<Box2i> m_DestRects;
	Box2i m_SrcRect;

public:
	DrawFullScreenTest( int32 divisions ) : m_Divs(divisions)
	{
	}

	virtual void Setup()
	{
		m_SrcRect.Set( 0, 0, g_pImage->GetDims().x, g_pImage->GetDims().y );
		m_DestRects.clear();

		// If there are no divisions
		if( m_Divs <= 1 )
		{
			m_DestRects.push_back( Box2i( Point2i(), g_pGraphicsMgr->GetDisplayDims()));
		}
		else
		{
			Vector2i destSize( g_pGraphicsMgr->GetDisplayDims() / m_Divs );

			if( m_SrcRect.size.x > destSize.x )
				m_SrcRect.size.x = destSize.x;

			if( m_SrcRect.size.y > destSize.y )
				m_SrcRect.size.y = destSize.y;

			for( int32 row = 0; row < m_Divs; ++row )
			{
				int32 y = row * destSize.y;
				for( int32 col = 0; col < m_Divs; ++col )
				{
					int32 x = col * destSize.x;

					m_DestRects.push_back( Box2i( x, y, destSize.x, destSize.y ) );
				}
			}
		}
	}

	virtual void RunTest()
	{
		for( std::list<Box2i>::iterator iterBox = m_DestRects.begin(); iterBox != m_DestRects.end(); ++iterBox )
		{
			g_pGraphicsMgr->DrawImage( *iterBox, g_pImage, m_SrcRect, 0 );
		}
	}

	virtual const wchar_t* GetName() const
	{
		static std::wstring sName;
		sName = L"Full Screen Draw (Split into ";
		sName += TCBase::EasyUIToA( m_Divs * m_Divs );
		sName += L" equal-sized rectangles";
		return sName.c_str();
	}
};

class Scale4thTest : GfxTest
{
	int32 m_Scale;
	std::list<Box2i> m_DestRects;
	Box2i m_SrcRect;

public:
	Scale4thTest( int32 scale ) : m_Scale(scale)
	{
	}

	virtual void Setup()
	{
		
		m_SrcRect.Set( 0, 0, g_pGraphicsMgr->GetDisplayDims().x / 4, g_pGraphicsMgr->GetDisplayDims().y / 4 );
		m_DestRects.clear();

		// If there are no divisions
		if( m_Scale == 4 )
		{
			m_DestRects.push_back( Box2i( Point2i(), g_pGraphicsMgr->GetDisplayDims()));
		}
		else
		{
			int32 divs = 4 / m_Scale;

			Vector2i destSize( m_SrcRect.size.x * m_Scale, m_SrcRect.size.y * m_Scale );
			for( int32 row = 0; row < divs; ++row )
			{
				int32 y = row * destSize.y;
				for( int32 col = 0; col < divs; ++col )
				{
					int32 x = col * destSize.x;

					m_DestRects.push_back( Box2i( x, y, destSize.x, destSize.y ) );
				}
			}
		}
	}

	virtual void RunTest()
	{
		for( std::list<Box2i>::iterator iterBox = m_DestRects.begin(); iterBox != m_DestRects.end(); ++iterBox )
		{
			g_pGraphicsMgr->DrawImage( *iterBox, g_pImage, m_SrcRect, 0 );
		}
	}

	virtual const wchar_t* GetName() const
	{
		static std::wstring sName;
		sName = L"Draw 4th image (";
		sName += TCBase::EasyUIToA( 16 / (m_Scale * m_Scale) );
		sName += L" times)";
		return sName.c_str();
	}
};

class BlocksTest : GfxTest
{
	std::list<Box2i> m_DestRects;
	Box2i m_SrcRect;
	int32 m_NumBlocks;

public:

	BlocksTest( int32 numBlocks ) : m_NumBlocks( numBlocks )
	{}

	virtual void Setup()
	{
		m_SrcRect.Set( 0, 0, 55, 55 );
		m_DestRects.clear();

		Point2i blockPos;
		for( int32 blockIndex = 0; blockIndex < m_NumBlocks; ++blockIndex )
		{
			if( blockIndex % 5 == 0 )
			{
				blockPos.y += 55;
				blockPos.x = 0;
			}

			m_DestRects.push_back( Box2i( blockPos, m_SrcRect.size ) );

			blockPos.x += 55;
		}
	}

	virtual void RunTest()
	{
		for( std::list<Box2i>::iterator iterBox = m_DestRects.begin(); iterBox != m_DestRects.end(); ++iterBox )
		{
			g_pGraphicsMgr->DrawImage( *iterBox, g_pImage, m_SrcRect, 0 );
		}
	}

	virtual const wchar_t* GetName() const
	{
		static std::wstring sName;
		sName = L"Draw ";
		sName += TCBase::EasyUIToA( m_NumBlocks );
		sName += L" blocks";
		return sName.c_str();
	}
};

class ClearFieldTest : GfxTest
{
	int32 m_Scale;
	Box2i m_DestRect;
	Box2i m_SrcRect;

public:

	virtual void Setup()
	{
		
		m_SrcRect.Set( 0, 0, g_pImage->GetDims().x, g_pImage->GetDims().y );
		m_DestRect.Set( 0, 0, 5 * 55, 12 * 55 );
	}

	virtual void RunTest()
	{
		g_pGraphicsMgr->DrawImage( m_DestRect, g_pImage, m_SrcRect, 0 );
	}

	virtual const wchar_t* GetName() const
	{
		return L"Draw area the size of the field 10";
	}
};

class RedrawPartialTest : GfxTest
{
	Box2i m_SrcRect;

	int32 m_NumDraw;

public:

	RedrawPartialTest( int32 numDraw ) : m_NumDraw( numDraw )
	{}

	virtual void Setup()
	{
		m_SrcRect.Set( 0, 0, g_pImage->GetDims().x / 8, g_pImage->GetDims().y / 8 );
	}

	virtual void RunTest()
	{
		//for( int32 drawIndex = 0; drawIndex < m_NumDraw; ++drawIndex )
		{
			g_pGraphicsMgr->DrawImage( Point2i(), g_pImage, m_SrcRect );
		}
	}

	virtual const wchar_t* GetName() const
	{
		static std::wstring sName;
		sName = L"Drawing 1/8th size blocks ";
		sName += TCBase::EasyUIToA( m_NumDraw );
		sName += L" times";
		return sName.c_str();
	}
};

void TestDDrawPerformance( TCImage* pImage )
{
	g_pImage = pImage;

	LARGE_INTEGER timerFreq, startTime, endTime;
	QueryPerformanceFrequency( &timerFreq );	
	float32 timerFreqScalar = 1.0f / (float32)timerFreq.QuadPart;

	GfxTest* testFuncs[] = {(GfxTest*)new DrawFullImageTest(),
							(GfxTest*)new DrawFullScreenTest(1),
							(GfxTest*)new DrawFullScreenTest(2),
							(GfxTest*)new DrawFullScreenTest(4),
							(GfxTest*)new Scale4thTest(4),
							(GfxTest*)new Scale4thTest(2),
							(GfxTest*)new Scale4thTest(1),
							(GfxTest*)new BlocksTest(10),
							(GfxTest*)new BlocksTest(5 * 6),
							(GfxTest*)new BlocksTest(5 * 10),
							(GfxTest*)new ClearFieldTest(),
							(GfxTest*)new RedrawPartialTest(1),
							(GfxTest*)new RedrawPartialTest(3),
							(GfxTest*)new RedrawPartialTest(6) };

	const uint32 NUM_TESTS = sizeof(testFuncs) / sizeof(testFuncs[0]);
	std::list<float32> avgValues;

	for( int curFuncIndex = 0; curFuncIndex < NUM_TESTS; ++curFuncIndex )
	{
		float32 totalElapsed = 0.0f;
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"*************STARTING TEST: %s***************", testFuncs[curFuncIndex]->GetName() );

		// Setup the test
		testFuncs[curFuncIndex]->Setup();

		// Run the test once to "load the cache"
		testFuncs[curFuncIndex]->RunTest();

		// Run the test with measurements
		for( uint32 testRunIndex = 0; testRunIndex < NUM_TEST_RUNS; ++testRunIndex )
		{
			QueryPerformanceCounter( &startTime );
			testFuncs[curFuncIndex]->RunTest();
			QueryPerformanceCounter( &endTime );			

			float32 elapsedTime = (float32)(endTime.QuadPart - startTime.QuadPart) * timerFreqScalar;
			MsgLogger::Get().Output( MsgLogger::MI_Note, L"%f", elapsedTime );
			totalElapsed += elapsedTime;
		}

		g_pGraphicsMgr->DisplayScene( true );
		Sleep( 500 );

		testFuncs[curFuncIndex]->TearDown();

		avgValues.push_back( (float32)totalElapsed / (float32)NUM_TEST_RUNS );
		MsgLogger::Get().Output( MsgLogger::MI_Note, L"Average time for %s: %f seconds", testFuncs[curFuncIndex]->GetName(), totalElapsed / (float32)NUM_TEST_RUNS );
	}

	MsgLogger::Get().Output( MsgLogger::MI_Note, L"******************************" );
		// Write out the summaries
	std::list<float32>::iterator iterAvg = avgValues.begin();
	for( int curFuncIndex = 0; curFuncIndex < NUM_TESTS; ++curFuncIndex )
	{
		MsgLogger::Get().Output( MsgLogger::MI_Note, L"Average time for %s: %e seconds", testFuncs[curFuncIndex]->GetName(), *iterAvg );
		++iterAvg;

		delete testFuncs[curFuncIndex];
		testFuncs[curFuncIndex] = 0;
	}
}