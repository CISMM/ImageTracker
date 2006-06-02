/////////////////////////////////////////////////////////////////////////////
// Name:        wxTestDesigns.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:
/////////////////////////////////////////////////////////////////////////////

#include "wxTestApp.h"
#include <iostream>
#include "Suite.h"

#include "LoggerPanel.h"

#include "TestBilateralVectorFilter.h"
#include "TestCLGOpticFlowImageFilter.h"
#include "TestDemonsPipeline.h"
#include "TestFileSet.h"
#include "TestFileSetImageReader.h"
#include "TestGlobalRegistrationPipeline.h"
#include "TestHarrisFeatureDetector.h"
#include "TestImagePyramid.h"
#include "TestImageRescale.h"
#include "TestImageStatistics.h"
#include "TestImageWindow.h"
#include "TestItkMagickIO.h"
#include "TestLogger.h"
#include "TestMultiRegionRegistration.h"
#include "TestMultiResolutionRegistration.h"
#include "TestMultiResolutionRegistrationPipeline.h"
#include "TestNaryMeanImageFilter.h"
#include "TestRandomVectorImage.h"
#include "TestRegistrationMotionFilter.h"
#include "TestRegistrationOutput.h"
#include "TestSort.h"
#include "TestStopWatch.h"
#include "TestThresholdPipeline.h"
#include "TestTransformGroup.h"
#include "TestVectorConvert.h"
#include "TestVectorWrite.h"
#include "TestVideoRegistration.h"

#include "ImageWindow.h"

// WDR: class implementations

//------------------------------------------------------------------------------
// TestCaseFrame
//------------------------------------------------------------------------------

// WDR: event table for TestCaseFrame

BEGIN_EVENT_TABLE(TestCaseFrame,wxFrame)
    EVT_MENU(wxID_EXIT, TestCaseFrame::OnQuit)
    EVT_CLOSE(TestCaseFrame::OnCloseWindow)
    EVT_MENU( ID_TEST, TestCaseFrame::OnRunTests )
END_EVENT_TABLE()

TestCaseFrame::TestCaseFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    CreateMyMenuBar();
	CreateTestDialog(this, true);

    CreateStatusBar(1);
    SetStatusText( wxT("Welcome!") );
}

void TestCaseFrame::CreateMyMenuBar()
{
    SetMenuBar( MyMenuBarFunc() );
}

// WDR: handler implementations for TestCaseFrame

void TestCaseFrame::OnRunTests( wxCommandEvent &event )
{
        TestSuite::Suite suite("Video Registration Suite");
        // suite.addTest(new TestRandomVectorImage);
        // suite.addTest(new TestBilateralVectorFilter);
        // suite.addTest(new TestCLGOpticFlowImageFilter);
        // suite.addTest(new TestDemonsPipeline);
        // suite.addTest(new TestFileSet);
        // suite.addTest(new TestFileSetImageReader);
        // suite.addTest(new TestGlobalRegistrationPipeline);        
        // suite.addTest(new TestHarrisFeatureDetector);
        // suite.addTest(new TestImageRescale);
        // suite.addTest(new TestImageStatistics);
        // suite.addTest(new TestImageWindow);        
        // suite.addTest(new TestItkMagickIO);
        // suite.addTest(new TestLogger);
        // suite.addTest(new TestMultiRegionRegistration);
		// suite.addTest(new TestRegistrationMotionFilter);
        // suite.addTest(new TestRegistrationOutput);
        // suite.addTest(new TestSort);
        // suite.addTest(new TestStopWatch);
        // suite.addTest(new TestThresholdPipeline);
        // suite.addTest(new TestTransformGroup);
        // suite.addTest(new TestVectorConvert);
        // suite.addTest(new TestVectorWrite);
        // suite.addTest(new TestVideoRegistration);
        suite.run();
        long errs = suite.report();

        if (errs > 0)
        {
            Logger::logWarn("Errors encountered.");
        }
        else
        {
            Logger::logInfo("Error free.");
        }
        suite.free();    
}

void TestCaseFrame::OnQuit( wxCommandEvent &event )
{
     this->Close(TRUE);
}

void TestCaseFrame::OnCloseWindow( wxCloseEvent &event )
{
    // if ! saved changes -> return

    ImageWindow::Destroy();
    this->Destroy();
}

//------------------------------------------------------------------------------
// TestCaseApp
//------------------------------------------------------------------------------

// IMPLEMENT_APP(TestCaseApp)

int main()
{
    TestSuite::Suite suite("Video Registration Suite");
	suite.addTest(new TestLogger);
	suite.addTest(new TestItkMagickIO);
	suite.addTest(new TestImagePyramid);
	suite.addTest(new TestNaryMeanImageFilter);
	//suite.addTest(new TestMultiResolutionRegistration);
	//suite.addTest(new TestMultiResolutionRegistrationPipeline);
	// suite.addTest(new TestLogStream);
	// suite.addTest(new TestRegistrationMotionFilter);
    // suite.addTest(new TestMultiRegionRegistration);
    // suite.addTest(new TestVectorWrite);
    suite.run();
    long errs = suite.report();

    if (errs > 0)
    {
        Logger::logWarn("Errors encountered.");
    }
    else
    {
        Logger::logInfo("Error free.");
    }
    suite.free();  
}

TestCaseApp::TestCaseApp()
{
}

bool TestCaseApp::OnInit()
{
    TestCaseFrame *frame = new TestCaseFrame( NULL, -1, wxT("TestCases"), wxDefaultPosition, wxSize(450, 320));
    frame->Show( TRUE );

    Logger::logInfo("Ready to test.");

    return TRUE;
}

int TestCaseApp::OnExit()
{
    return 0;
}

