/**
	@class WCTest

	@brief
		단위테스트 상위 클래스

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(), gtest(), Sample(), Stress(), valg(), prof()

	@remark
		- 단위테스트 클래스들의 상위 클래스

	@warning

	@bug

	@todo

	@date
	    - 2009-09-04
	        -# 최초 작성
*/

#ifndef __WCTEST_H__
#define __WCTEST_H__

// 테스트 객체
class WCTest : public WCObject
{
};

// 테스트 객체
class WCTestManager : public WCTest,WIMember
{
public:
	WCTestManager(const char *pcName);
};

// 테스트 객체 : WITimer 인터페이스를 상속받은 객체는 타이머 사용 가능
class WCTestTimer : public WCTest, WITimer
{
public:
	WCTestTimer(const char *pcName);
	~WCTestTimer();
	virtual void VTimer(const timer_t tTimer, const int nTimerID);
public:
	char m_cTest[10];
	int m_nSTmCnt;	// 타이머 발생 카운트
	int m_nRTmCnt; 	// 타이머 이벤트 수신 카운트 
};

// 테스트 객체 : Thread 클래스를 상속받아서 사용
class WCTestThread : public WCThread
{
public:
	WCTestThread(const char *pcName);
	~WCTestThread();
	virtual void VRun();
	virtual void VStop();
	virtual int VEvent(const char *pcData, const int nPackSz);

public:
	bool m_bLoop;
	int m_nThread;
	int m_nCountIn;
	int m_nCountOut;
};


// 단위 테스트 (WCObject)
class UnitTest_WCObject : public testing::Test
{
public:
	UnitTest_WCObject() {}
	virtual ~UnitTest_WCObject() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCLog)
class UnitTest_WCLog : public testing::Test
{
public:
	UnitTest_WCLog() {}
	virtual ~UnitTest_WCLog() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCCfg)
class UnitTest_WCCfg : public testing::Test
{
public:
	UnitTest_WCCfg() {}
	virtual ~UnitTest_WCCfg() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCTimer)
class UnitTest_WCTimer : public testing::Test
{
public:
	UnitTest_WCTimer() {}
	virtual ~UnitTest_WCTimer() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCMd5)
class UnitTest_WCMd5 : public testing::Test
{
public:
	UnitTest_WCMd5() {}
	virtual ~UnitTest_WCMd5() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCMysql)
class UnitTest_WCMysql : public testing::Test
{
public:
	UnitTest_WCMysql() {}
	virtual ~UnitTest_WCMysql() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCOracle)
class UnitTest_WCOracle : public testing::Test
{
public:
	UnitTest_WCOracle() {}
	virtual ~UnitTest_WCOracle() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCManager)
class UnitTest_WCManager : public testing::Test
{
public:
	UnitTest_WCManager() {}
	virtual ~UnitTest_WCManager() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCManagerDB)
class UnitTest_WCManagerDB : public testing::Test
{
public:
	UnitTest_WCManagerDB() {}
	virtual ~UnitTest_WCManagerDB() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCThread)
class UnitTest_WCThread : public testing::Test
{
public:
	UnitTest_WCThread() {}
	virtual ~UnitTest_WCThread() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCManagerThread)
class UnitTest_WCManagerThread : public testing::Test
{
public:
	UnitTest_WCManagerThread() {}
	virtual ~UnitTest_WCManagerThread() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCSocket)
class UnitTest_WCSocket : public testing::Test
{
public:
	UnitTest_WCSocket() {}
	virtual ~UnitTest_WCSocket() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// 단위 테스트 (WCManagerSocket)
class UnitTest_WCManagerSocket : public testing::Test
{
public:
	UnitTest_WCManagerSocket() {}
	virtual ~UnitTest_WCManagerSocket() {}

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

#endif
