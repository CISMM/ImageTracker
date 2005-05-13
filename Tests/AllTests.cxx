#include <iostream>

#include "../TestSuite/Suite.h"

#include "TestDemonsPipeline.h"
#include "TestFileSet.h"
#include "TestFileSetImageReader.h"
#include "TestGlobalRegistrationPipeline.h"
#include "TestImageRescale.h"
#include "TestImageStatistics.h"
#include "TestImageWindow.h"
#include "TestLogger.h"
#include "TestMultiRegionRegistration.h"
#include "TestRegistrationOutput.h"
#include "TestStopWatch.h"
#include "TestThresholdPipeline.h"
#include "TestTransformGroup.h"
#include "TestVectorConvert.h"
#include "TestVideoRegistration.h"

#include "../wxGui/ImageWindow.h"
#include "../wxGui/ImageViewer.h"
#include "../wxGui/DialogLogger.h"

// Turn on debugging
#ifndef DEBUG
#define DEBUG true

//--------------------------------------------------------------
// TestCaseFrame and App
//--------------------------------------------------------------
class TestCaseFrame : public wxFrame
{
public:
    TestCaseFrame(wxWindow *parent, wxWindowID id = -1, 
        const wxString &title = _("TestCases"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE) :
    wxFrame(parent, id, title, pos, size, style)
    {
        mb->Append()
        this->SetMenuBar()
        this->CreateStatusBar(1);
    }

    ~TestCaseFrame()
    {
        Logger::Destroy();
    }
};

class TestCaseApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        TestCaseFrame* frame = new TestCaseFrame((wxWindow*) NULL);
        frame->Show(true);
        this->SetTopWindow(frame);

        Logger::SetLogger(new DialogLogger());
        Logger::logDebug("Initialized.");

        this->runTests();
        Logger::setVisible(true);

        return true;
    }

    void runTests()
    {
        TestSuite::Suite suite("Video Registration Suite");
        // suite.addTest(new TestFileSet);
        // suite.addTest(new TestFileSetImageReader);
        // suite.addTest(new TestGlobalRegistrationPipeline);
        // suite.addTest(new TestLogger);
        suite.addTest(new TestImageWindow);
        // suite.addTest(new TestRegistrationOutput);
        // suite.addTest(new TestThresholdPipeline);
        // suite.addTest(new TestTransformGroup);
        // suite.addTest(new TestVideoRegistration);
        // suite.addTest(new TestMultiRegionRegistration);
        // suite.addTest(new TestStopWatch);
        // suite.addTest(new TestImageStatistics);
        // suite.addTest(new TestDemonsPipeline);
        // suite.addTest(new TestVectorConvert);
        // suite.addTest(new TestImageRescale);
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
};

IMPLEMENT_APP(TestCaseApp)

#endif // Debug