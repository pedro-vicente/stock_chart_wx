#ifndef WX_ELLIOT_FRAME
#define WX_ELLIOT_FRAME 1


#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/splitter.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/grid.h"
#include "wx/mdi.h"
#include "wx/toolbar.h"
#include "wx/laywin.h"
#include "wx/list.h"
#include "wx/cmdline.h"
#include "wx/datetime.h"
#include "wx/datectrl.h"
#include "wx/stattext.h"
#include "wx/dateevt.h"
#include "wx/panel.h"
#include "wx/calctrl.h"
#include "wx/timectrl.h"
#include "wx/collpane.h"
#include "icons/sample.xpm"
#include "icons/file.xpm"
#include "icons/forward.xpm"
#include "icons/folder.xpm"
#include "icons/doc_blue.xpm"
#include "grafix.hh"
#include "model.hh"
#include "connect_alpha.hh"

enum
{
  SOCKET_ID = wxID_HIGHEST + 1,
  ID_CONNECT_DLG,
  ID_TRADE_DLG,
  ID_COMBO_PERIOD,
  ID_COMBO_INTERVAL,
  ID_COMBO_TICKER,
  ID_CONNECT_HTTP,
  ID_CONNECT_HTTP_UPDATE,
  ID_CONNECT_BUTTON,
  ID_TEXT_CTRL_INTERVAL,
  ID_TEXT_CTRL_TITLE,
  ID_TEXT_CTRL_PERIOD,
  ID_TEXT_CTRL_TRADE_INDEX,
  ID_TEXT_CTRL_TRADE_AMOUNT,
  WAVE_0,
  WAVE_1,
  WAVE_2,
  WAVE_3,
  WAVE_4,
  WAVE_5,
  WAVE_A,
  WAVE_B,
  WAVE_C,
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//DialogConnect
/////////////////////////////////////////////////////////////////////////////////////////////////////

class DialogConnect : public wxDialog
{
public:
  DialogConnect(wxWindow *parent);
  void OnConnect(wxCommandEvent& event);
  void OnComboInterval(wxCommandEvent& event);
  void OnComboPeriod(wxCommandEvent& event);
  void OnComboTicker(wxCommandEvent& event);
private:
  wxDECLARE_EVENT_TABLE();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//DialogTrade
/////////////////////////////////////////////////////////////////////////////////////////////////////

class DialogTrade : public wxDialog
{
public:
  DialogTrade(wxWindow *parent);
private:
  wxDECLARE_EVENT_TABLE();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxChart : public wxScrolledWindow
{
public:
  wxChart(wxWindow *parent);
  virtual void OnDraw(wxDC& dc);
  void OnMouseDown(wxMouseEvent &event);
  void OnMouseMove(wxMouseEvent &event);
  void parse_response(const std::vector<time_price_t> &tp, int interval, const std::string &ticker);
  void read_file(const std::string &file_name);
  void store(const std::string &wave);
  void update();
  size_t trades();
  void delete_trades();
  graf_t m_graf;
  void write();
  model_t m_model;

private:
  void initialize_chart();
  size_t m_pt; //current selected index data point (mouse down)
  wxDECLARE_EVENT_TABLE();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxFrameMain : public wxFrame
{
public:
  wxFrameMain();
  ~wxFrameMain();
  void OnInput(wxCommandEvent& event);
  void OnTrade(wxCommandEvent& event);
  void OnFileOpen(wxCommandEvent &event);
  void OnMRUFile(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnConnect(wxCommandEvent& event);
  void OnWave_0(wxCommandEvent& event);
  void OnWave_1(wxCommandEvent& event);
  void OnWave_2(wxCommandEvent& event);
  void OnWave_3(wxCommandEvent& event);
  void OnWave_4(wxCommandEvent& event);
  void OnWave_5(wxCommandEvent& event);
  void OnConnectUpdate(wxCommandEvent& event);
  wxString m_current_file;

protected:
  wxWindow *m_win_grid;
  wxWindow *m_win_chart;
  wxSplitterWindow* m_splitter;
  void get_results();
  int read(const std::string &file_name);
  wxFileHistory m_file_history;

  //[INTERVAL] : Interval or frequency in seconds
  //[PERIOD] : The historical data period, "d" or "Y"
  //[TICKER] : This is the ticker symbol of the stock

public:
  int m_interval; //model, in seconds
  wxString m_period; //"90d"
  wxString m_ticker;

private:
  wxDECLARE_EVENT_TABLE();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppAlert
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxAppAlert : public wxApp
{
public:
  virtual bool OnInit();
  void OnInitCmdLine(wxCmdLineParser& pParser);
  bool OnCmdLineParsed(wxCmdLineParser& pParser);
};

#endif

