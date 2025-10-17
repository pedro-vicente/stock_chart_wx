#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "sample.xpm"
#include "trader.hh"
#include "display.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// TraderApp
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_APP(TraderApp);

bool TraderApp::OnInit()
{
  if (!wxApp::OnInit())
    return false;

  MainFrame* frame = new MainFrame("Trader");
  frame->Show(true);
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// MainFrame
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
  : wxFrame(nullptr, wxID_ANY, title)
{
  SetIcon(wxICON(sample));
  wxMenu* fileMenu = new wxMenu;
  wxMenu* helpMenu = new wxMenu;
  helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
  fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");
  wxMenuBar* menuBar = new wxMenuBar();
  menuBar->Append(fileMenu, "&File");
  menuBar->Append(helpMenu, "&Help");
  SetMenuBar(menuBar);
  CreateStatusBar(2);
  SetStatusText("Ready");

  wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  PanelDisplay* panel_current = new PanelDisplay(this);
  sizer->Add(panel_current, 1, wxGROW | wxALL, 0);
  SetSizerAndFit(sizer);

  wxDisplay display(wxDisplay::GetFromWindow(this));
  wxRect rect = display.GetClientArea();
  SetSize(rect);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// OnQuit
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// OnAbout
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString message(wxString::Format("Trader 1.0\n\n%s", wxGetOsDescription()));
  wxMessageBox(message, "About Trader", wxOK | wxICON_INFORMATION, this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// PanelInput
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(PanelInput, wxPanel)
EVT_BUTTON(ID_BUTTON_GET_DATA, PanelInput::OnGet)
wxEND_EVENT_TABLE()

PanelInput::PanelInput(wxWindow* parent) :
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
  SetBackgroundColour(wxColour(240, 240, 240));
  wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  wxButton* btn = new wxButton(this, ID_BUTTON_GET_DATA, _T("Get"), wxDefaultPosition, wxDefaultSize, 0);
  sizer->Add(btn);
  SetSizerAndFit(sizer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// OnGet
/////////////////////////////////////////////////////////////////////////////////////////////////////

void PanelInput::OnGet(wxCommandEvent&)
{
  PanelDisplay* display = dynamic_cast<PanelDisplay*>(GetParent()->GetParent());
  display->get_data();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// parse_timestamp
/////////////////////////////////////////////////////////////////////////////////////////////////////

time_t parse_timestamp(const std::string& ts)
{
  std::tm tm = {};
  int sec = 0;
  if (ts.length() >= 19)
  {
    sscanf(ts.c_str(), "%d-%d-%d %d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &sec);
    tm.tm_sec = sec;
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
  }
  else if (ts.length() >= 10)
  {
    sscanf(ts.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    tm.tm_hour = 12;
  }
  return mktime(&tm);
}

