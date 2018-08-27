#include "stock_chart_wx.hh"
#include "stock_chart.hh"
#include <curl/curl.h>
#include <algorithm>

//https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol=^DJI&datatype=csv&interval=60min&apikey=IBT6KR8WTX30IUHS
//https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol=^DJI&interval=60min&outputsize=full&apikey=IBT6KR8WTX30IUHS
//https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol=^INX&datatype=csv&interval=60min&apikey=IBT6KR8WTX30IUHS
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxString api_key;
const int panel_size = 510;
const int alpha = 1;
const int alpha_interval = 60;
//^DJI
//^INX
std::string alpha_ticker("^DJI");
const time_series alpha_time_series = TIME_SERIES_INTRADAY;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppAlert::OnInit()
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_APP(wxAppAlert);

bool wxAppAlert::OnInit()
{
  if (!wxApp::OnInit())
  {
    return false;
  }
  wxFrameMain *frame = new wxFrameMain();
  frame->Show(true);
  frame->Maximize();
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppAlert::OnInitCmdLine
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxAppAlert::OnInitCmdLine(wxCmdLineParser& pParser)
{
  wxApp::OnInitCmdLine(pParser);
  pParser.SetSwitchChars("-"); //both windows, unix
  pParser.AddOption("k", "key", "API key", wxCMD_LINE_VAL_STRING);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppAlert::OnCmdLineParsed
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool wxAppAlert::OnCmdLineParsed(wxCmdLineParser& pParser)
{
  wxString m_uid;

  if (!pParser.Found("k", &api_key))
  {
    wxMessageDialog dlg(NULL, "Usage: ./stock_chart_wx -k API_KEY");
    dlg.ShowModal();
    exit(1);
  }
  return wxApp::OnCmdLineParsed(pParser);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//DialogConnect::DialogConnect
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(DialogConnect, wxDialog)
EVT_BUTTON(ID_CONNECT_BUTTON, DialogConnect::OnConnect)
EVT_COMBOBOX(ID_COMBO_INTERVAL, DialogConnect::OnComboInterval)
EVT_COMBOBOX(ID_COMBO_PERIOD, DialogConnect::OnComboPeriod)
EVT_COMBOBOX(ID_COMBO_TICKER, DialogConnect::OnComboTicker)
wxEND_EVENT_TABLE()

DialogConnect::DialogConnect(wxWindow *parent)
  : wxDialog(parent, wxID_ANY, wxT("Connect to server"),
    wxDefaultPosition, wxDefaultSize,
    wxDEFAULT_DIALOG_STYLE)
{
  wxFrameMain *main = (wxFrameMain*)wxGetApp().GetTopWindow();
  wxGridSizer *sizer = new wxGridSizer(3);
  int flag = wxALL | wxALIGN_CENTER;
  int flag_right = wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL;
  int border = 15; //width

  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Interval")), 0, flag_right, border);
  wxString str_interval = wxString::Format(wxT("%i"), main->m_interval);
  wxTextCtrl *text_interval = new wxTextCtrl(this, ID_TEXT_CTRL_INTERVAL, str_interval);
  sizer->Add(text_interval, 0, flag, border);

  wxArrayString m_arr_interval;
  m_arr_interval.push_back("900");
  m_arr_interval.push_back("1800");
  m_arr_interval.push_back("3600");
  m_arr_interval.push_back("86400");
  wxComboBox *combobox_interval = new wxComboBox(this, ID_COMBO_INTERVAL, wxT("Interval"),
    wxDefaultPosition, wxDefaultSize, 0, NULL,
    wxCB_READONLY | wxCB_DROPDOWN);
  for (size_t idx = 0; idx < m_arr_interval.GetCount(); idx++)
  {
    combobox_interval->Append(m_arr_interval[idx]);
  }
  combobox_interval->Select(0);
  sizer->Add(combobox_interval, 0, flag, border);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //symbol
  // Dow ".DJI"
  // SP500 ".INX" 
  // Nasdaq  ".IXIC"
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Symbol")), 0, flag_right, border);
  wxTextCtrl *text_title = new wxTextCtrl(this, ID_TEXT_CTRL_TITLE, main->m_ticker);
  sizer->Add(text_title, 0, flag, border);

  wxArrayString m_arr_symbol;
  m_arr_symbol.push_back(".DJI");
  m_arr_symbol.push_back(".INX");
  m_arr_symbol.push_back(".IXIC");
  m_arr_symbol.push_back("N225");
  m_arr_symbol.push_back("DOG");
  m_arr_symbol.push_back("DXD");
  m_arr_symbol.push_back("SDOW");
  m_arr_symbol.push_back("SPXU");
  m_arr_symbol.push_back("SQQQ");
  m_arr_symbol.push_back("TBF"); //30 year Treasury Yield inverted 
  m_arr_symbol.push_back("EURUSD");

  wxComboBox *combobox_symbol = new wxComboBox(this, ID_COMBO_TICKER, wxT("Ticker"),
    wxDefaultPosition, wxDefaultSize, 0, NULL,
    wxCB_READONLY | wxCB_DROPDOWN);
  for (size_t idx = 0; idx < m_arr_symbol.GetCount(); idx++)
  {
    combobox_symbol->Append(m_arr_symbol[idx]);
  }
  combobox_symbol->Select(0);
  sizer->Add(combobox_symbol, 0, flag, border);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //period
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Period")), 0, flag_right, border);
  wxTextCtrl *text_period = new wxTextCtrl(this, ID_TEXT_CTRL_PERIOD, main->m_period);
  sizer->Add(text_period, 0, flag, border);

  wxArrayString m_arr_period;
  m_arr_period.push_back("40d");
  m_arr_period.push_back("60d");
  m_arr_period.push_back("90d");
  m_arr_period.push_back("1Y");
  m_arr_period.push_back("2Y");
  m_arr_period.push_back("3Y");
  wxComboBox *combobox_period = new wxComboBox(this, ID_COMBO_PERIOD, wxT("Period"),
    wxDefaultPosition, wxDefaultSize, 0, NULL,
    wxCB_READONLY | wxCB_DROPDOWN);
  for (size_t idx = 0; idx < m_arr_period.GetCount(); idx++)
  {
    combobox_period->Append(m_arr_period[idx]);
  }
  combobox_period->Select(0);
  sizer->Add(combobox_period, 0, flag, border);

  wxButton *btn = new wxButton(this, ID_CONNECT_BUTTON, _("Connect"));
  btn->SetDefault();
  sizer->Add(0, 0, 1, flag, border);
  sizer->Add(btn, 0, flag, border);
  sizer->Add(0, 0, 1, flag, border);

  SetSizer(sizer);
  sizer->Fit(this);
  Centre();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//DialogConnect::OnConnect
/////////////////////////////////////////////////////////////////////////////////////////////////////

void DialogConnect::OnConnect(wxCommandEvent& WXUNUSED(event))
{
  EndModal(wxID_OK);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//DialogConnect::OnComboInterval
/////////////////////////////////////////////////////////////////////////////////////////////////////

void DialogConnect::OnComboInterval(wxCommandEvent& event)
{
  wxString str = event.GetString().c_str();
  wxTextCtrl *text = (wxTextCtrl *)FindWindow(ID_TEXT_CTRL_INTERVAL);
  text->SetValue(str);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//DialogConnect::OnComboPeriod
/////////////////////////////////////////////////////////////////////////////////////////////////////

void DialogConnect::OnComboPeriod(wxCommandEvent& event)
{
  wxString str = event.GetString().c_str();
  wxTextCtrl *text = (wxTextCtrl *)FindWindow(ID_TEXT_CTRL_PERIOD);
  text->SetValue(str);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//DialogConnect::OnComboTicker
/////////////////////////////////////////////////////////////////////////////////////////////////////

void DialogConnect::OnComboTicker(wxCommandEvent& event)
{
  wxString str = event.GetString().c_str();
  wxTextCtrl *text = (wxTextCtrl *)FindWindow(ID_TEXT_CTRL_TITLE);
  text->SetValue(str);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::wxFrameMain
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(wxFrameMain, wxFrame)
EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, wxFrameMain::OnMRUFile)
EVT_MENU(wxID_OPEN, wxFrameMain::OnFileOpen)
EVT_MENU(wxID_EXIT, wxFrameMain::OnQuit)
EVT_MENU(wxID_ABOUT, wxFrameMain::OnAbout)
EVT_TOOL(ID_CONNECT_HTTP, wxFrameMain::OnConnect)
EVT_TOOL(ID_CONNECT_DLG, wxFrameMain::OnInput)
EVT_TOOL(ID_TRADE_DLG, wxFrameMain::OnTrade)
EVT_TOOL(ID_CONNECT_HTTP_UPDATE, wxFrameMain::OnConnectUpdate)
EVT_TOOL(WAVE_0, wxFrameMain::OnWave_0)
EVT_TOOL(WAVE_1, wxFrameMain::OnWave_1)
EVT_TOOL(WAVE_2, wxFrameMain::OnWave_2)
EVT_TOOL(WAVE_3, wxFrameMain::OnWave_3)
EVT_TOOL(WAVE_4, wxFrameMain::OnWave_4)
EVT_TOOL(WAVE_5, wxFrameMain::OnWave_5)
wxEND_EVENT_TABLE()

wxFrameMain::wxFrameMain()
  : wxFrame(NULL, wxID_ANY, "Elliot Wave model"),
  m_interval(900), //connect, in seconds
  m_period("40d"), //connect, days "d" or years "Y"
  m_ticker(".DJI")
{
  SetIcon(wxICON(sample));
  wxMenu *menu_file = new wxMenu;
  menu_file->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");
  wxMenu *menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(menu_file, "&File");
  menu_bar->Append(menu_help, "&Help");
  SetMenuBar(menu_bar);
  CreateStatusBar(2);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //toolbar
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
  wxToolBar* tb = GetToolBar();
#if defined (__WXMSW__)
  tb->SetToolBitmapSize(tb->GetToolBitmapSize() + wxSize(0, 10));
#endif
  tb->AddTool(wxID_OPEN, wxT("Open file"), wxBitmap(xpm_folder), wxT("Open file"));
  tb->AddTool(ID_CONNECT_HTTP, wxT("Retrieve"), wxBitmap(forward_xpm), wxT("Retrieve"));
  tb->AddTool(ID_CONNECT_HTTP_UPDATE, wxT("Update"), wxBitmap(forward_xpm), wxT("Update"));
  tb->AddTool(WAVE_0, wxT("Wave 0"), wxBitmap(xpm_file), wxT("Wave 0"));
  tb->AddTool(WAVE_1, wxT("Wave 1"), wxBitmap(xpm_file), wxT("Wave 1"));
  tb->AddTool(WAVE_2, wxT("Wave 2"), wxBitmap(xpm_file), wxT("Wave 2"));
  tb->AddTool(WAVE_3, wxT("Wave 3"), wxBitmap(xpm_file), wxT("Wave 3"));
  tb->AddTool(WAVE_4, wxT("Wave 4"), wxBitmap(xpm_file), wxT("Wave 4"));
  tb->AddTool(WAVE_5, wxT("Wave 5"), wxBitmap(xpm_file), wxT("Wave 5"));

  tb->Realize();

  m_splitter = new wxSplitterWindow(this);
  m_splitter->SetSashInvisible(true);
  wxPanel *grid = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER);
  grid->SetMinSize(wxSize(panel_size, -1));
  wxWindow *chart = new wxWindow(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER);
  m_splitter->SplitVertically(grid, chart, panel_size);

  //avoid update on empty windows 
  m_win_grid = NULL;
  m_win_chart = NULL;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //file history
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_file_history.UseMenu(menu_file);
  m_file_history.AddFilesToMenu(menu_file);
  m_file_history.Load(*wxConfig::Get());

  wxLogDebug("%d %s %s", m_interval, m_ticker, m_period);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::~wxFrameMain
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxFrameMain::~wxFrameMain()
{
  m_file_history.Save(*wxConfig::Get());
  if (m_win_chart)
  {
    m_win_chart->Destroy();
  }
  if (m_win_grid)
  {
    m_win_grid->Destroy();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnQuit
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  m_file_history.Save(*wxConfig::Get());
  Close(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnAbout
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString str("Pedro Vicente (2018)\n\n");
  wxMessageBox(str, "Chart", wxOK, this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnTrade
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnTrade(wxCommandEvent&)
{
  wxChart *chart = (wxChart *)m_win_chart;
  if (!m_win_chart) return;
  DialogTrade dlg(this);
  if (dlg.ShowModal() != wxID_OK)
  {
    return;
  }

  wxTextCtrl *text_index = (wxTextCtrl *)dlg.FindWindow(ID_TEXT_CTRL_TRADE_INDEX);
  wxTextCtrl *text_amount = (wxTextCtrl *)dlg.FindWindow(ID_TEXT_CTRL_TRADE_AMOUNT);
  if (text_index->GetLineText(0).IsEmpty() || text_amount->GetLineText(0).IsEmpty())
  {
    return;
  }

  int idx = wxAtoi(text_index->GetLineText(0));
  chart->m_model.m_tp.at(idx).trade = text_amount->GetLineText(0);
  chart->write();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnInput
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnInput(wxCommandEvent&)
{
  DialogConnect dlg(this);
  if (dlg.ShowModal() != wxID_OK)
  {
    return;
  }

  wxTextCtrl *text_interval = (wxTextCtrl *)dlg.FindWindow(ID_TEXT_CTRL_INTERVAL);
  wxTextCtrl *text_title = (wxTextCtrl *)dlg.FindWindow(ID_TEXT_CTRL_TITLE);
  wxTextCtrl *text_period = (wxTextCtrl *)dlg.FindWindow(ID_TEXT_CTRL_PERIOD);
  if (!text_interval->GetLineText(0).IsEmpty())
  {
    wxString str_interval = text_interval->GetLineText(0);
    m_interval = wxAtoi(str_interval);
  }
  if (!text_title->GetLineText(0).IsEmpty())
  {
    m_ticker = text_title->GetLineText(0);
  }
  if (!text_period->GetLineText(0).IsEmpty())
  {
    m_period = text_period->GetLineText(0);
  }
  get_results();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::get_results
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::get_results()
{
  m_win_chart = m_splitter->GetWindow2();
  wxChart *chart = new wxChart(m_splitter);
  if (alpha)
  {
    std::vector<time_price_t> tp;
    //^DJI
    //^INX
    m_interval = alpha_interval; //min
    m_ticker = alpha_ticker;

    connect_alpha(m_ticker, m_interval, alpha_time_series, api_key.c_str(), tp);
    chart->parse_response(tp, m_interval, m_ticker.ToStdString());
    chart->write();
  }
  else
  {

  }
  m_splitter->ReplaceWindow(m_win_chart, chart);
  m_win_chart->Destroy();
  m_win_chart = chart;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnConnect
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnConnect(wxCommandEvent&)
{
  get_results();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnConnectUpdate
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnConnectUpdate(wxCommandEvent&)
{
  wxChart *chart = (wxChart *)m_win_chart;
  if (!m_win_chart) return;
  chart->update();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnMRUFile
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnMRUFile(wxCommandEvent& event)
{
  size_t idx = event.GetId() - wxID_FILE1;
  wxString path(m_file_history.GetHistoryFile(idx));
  if (!path.empty())
  {
    if (read(path.ToStdString()) < 0)
    {
      m_file_history.RemoveFileFromHistory(idx);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnFileOpen
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnFileOpen(wxCommandEvent &WXUNUSED(event))
{
  wxString path;
  wxFileDialog dlg(this, wxT("Open file"),
    wxEmptyString,
    wxEmptyString,
    wxString::Format
    (
      wxT("EWT (*.txt)|*.txt|All files (%s)|%s"),
      wxFileSelectorDefaultWildcardStr,
      wxFileSelectorDefaultWildcardStr
    ),
    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
  if (dlg.ShowModal() != wxID_OK)
  {
    return;
  }
  path = dlg.GetPath();
  if (this->read(path.ToStdString()) != 0)
  {
    return;
  }
  m_file_history.AddFileToHistory(path);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::read
/////////////////////////////////////////////////////////////////////////////////////////////////////

int wxFrameMain::read(const std::string &file_name)
{
  std::ifstream ifs(file_name.c_str());
  if (!ifs.is_open())
  {
    return -1;
  }

  m_win_chart = m_splitter->GetWindow2();
  wxChart *chart = new wxChart(m_splitter);
  chart->read_file(file_name);

  if (
    (chart->m_model.m_ticker.compare("DOG") == 0) ||
    (chart->m_model.m_ticker.compare("DXD") == 0) ||
    (chart->m_model.m_ticker.compare("SDOW") == 0) ||
    (chart->m_model.m_ticker.compare("SPXU") == 0) ||
    (chart->m_model.m_ticker.compare("SQQQ") == 0) ||
    (chart->m_model.m_ticker.compare("TBF") == 0) ||
    (chart->m_model.m_ticker.compare("EURUSD") == 0)
    )
  {
    chart->m_graf.nbr_decimals = 2;
  }

  m_splitter->ReplaceWindow(m_win_chart, chart);
  m_win_chart->Destroy();
  m_win_chart = chart;

  m_win_grid = m_splitter->GetWindow1();
  wxPanel *grid = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER);
  m_splitter->ReplaceWindow(m_win_grid, grid);
  m_win_grid->Destroy();
  m_win_grid = grid;

  m_current_file = file_name;
  SetTitle(file_name);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnWave_0
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnWave_0(wxCommandEvent&)
{
  wxChart *chart = (wxChart *)m_win_chart;
  if (!m_win_chart) return;
  chart->store("0");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnWave_1
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnWave_1(wxCommandEvent&)
{
  wxChart *chart = (wxChart *)m_win_chart;
  if (!m_win_chart) return;
  chart->store("1");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnWave_2
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnWave_2(wxCommandEvent&)
{
  wxChart *chart = (wxChart *)m_win_chart;
  if (!m_win_chart) return;
  chart->store("2");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnWave_3
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnWave_3(wxCommandEvent&)
{
  wxChart *chart = (wxChart *)m_win_chart;
  if (!m_win_chart) return;
  chart->store("3");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnWave_4
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnWave_4(wxCommandEvent&)
{
  wxChart *chart = (wxChart *)m_win_chart;
  if (!m_win_chart) return;
  chart->store("4");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnWave_5
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnWave_5(wxCommandEvent&)
{
  wxChart *chart = (wxChart *)m_win_chart;
  if (!m_win_chart) return;
  chart->store("5");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::wxChart()
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(wxChart, wxScrolledWindow)
EVT_LEFT_DOWN(wxChart::OnMouseDown)
EVT_MOTION(wxChart::OnMouseMove)
wxEND_EVENT_TABLE()

wxChart::wxChart(wxWindow *parent) :
  wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxDOUBLE_BORDER),
  m_pt(0)
{


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::read_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::read_file(const std::string &file_name)
{
  if (m_model.read(file_name) < 0)
  {
    assert(0);
  }
  int interval = m_model.m_tp.at(1).time - m_model.m_tp.at(0).time;
  //interval is read from file
  if (m_model.m_interval != interval)
  {
    wxLogDebug("intervals differ, file, time difference [1]-[0], %i,  %i", m_model.m_interval, interval);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //initialize_chart
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  initialize_chart();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::initialize_chart
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::initialize_chart()
{
  size_t nbr_pts = m_model.m_tp.size();
  double x_low, y_low, x_high, y_high;
  int x_org, y_org;
  int x_min, x_max, y_min, y_max;

  //origin
  x_org = 80;
  y_org = 30;

  float low = 1E6;
  float high = -1E6;
  float v;
  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    v = m_model.m_tp.at(idx).value;
    if (v > high)
    {
      high = v;
    }
    if (v < low)
    {
      low = v;
    }
  }

  //data
  x_low = 0;
  x_high = nbr_pts;
  y_low = low;
  y_high = high;
  //screen
  x_min = x_org;
  y_min = y_org;
  x_max = 1300;
  y_max = 750;
  SetScrollbar(wxVERTICAL, 0, 0, 0);
  m_graf.init(x_min, y_min, x_max, y_max, x_low, y_low, x_high, y_high);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::OnDraw
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnDraw(wxDC& dc)
{
  size_t nbr_pts = m_model.m_tp.size();
  m_graf.draw_back(dc);
  if (nbr_pts == 0)
  {
    return;
  }
  m_graf.draw_scale(dc);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //data line
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  for (size_t idx = 1; idx < nbr_pts; idx++)
  {
    time_price_t tv1 = m_model.m_tp.at(idx - 1);
    time_price_t tv2 = m_model.m_tp.at(idx);
    m_graf.draw_line(dc, idx - 1, tv1.value, idx, tv2.value);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //data points
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    time_price_t tv = m_model.m_tp.at(idx);
    m_graf.draw_circle(dc, idx, tv.value, wxColour(0, 255, 0));
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //trades (after data points)
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    time_price_t tv = m_model.m_tp.at(idx);
    if (tv.trade != "-")
    {
      const wxColour color_trade(255, 255, 0);
      m_graf.draw_circle(dc, idx, tv.value, color_trade, 4);
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //waves 
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t wave_1_index = 0;
  size_t wave_3_index = 0;
  time_price_t wave1(0, 0, "-");
  time_price_t wave3(0, 0, "-");

  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    time_price_t tv = m_model.m_tp.at(idx);
    if (tv.wave != "-")
    {
      bool up = false;
      if (tv.wave == "1" || tv.wave == "3" || tv.wave == "5")
      {
        up = true;
      }
      m_graf.draw_text(dc, tv.wave, idx, tv.value, wxColour(255, 0, 0), up);
      m_graf.draw_circle(dc, idx, tv.value, wxColour(255, 0, 0));

      if (tv.wave == "1")
      {
        wave_1_index = idx;
        wave1 = tv;
      }
      else if (tv.wave == "3")
      {
        wave_3_index = idx;
        wave3 = tv;
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //selected point
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_graf.draw_circle(dc, m_pt, m_model.m_tp.at(m_pt).value, wxColour(255, 0, 0));


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::trades
/////////////////////////////////////////////////////////////////////////////////////////////////////

size_t wxChart::trades()
{
  size_t trd = 0;
  size_t nbr_pts = m_model.m_tp.size();
  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    time_price_t tv = m_model.m_tp.at(idx);
    if (tv.trade != "-")
    {
      trd++;
    }
  }
  return trd;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::delete_trades
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::delete_trades()
{
  size_t nbr_pts = m_model.m_tp.size();
  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    m_model.m_tp.at(idx).trade = "-";
  }
  write();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::OnMouseDown
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseDown(wxMouseEvent &event)
{
  wxFrameMain *main = (wxFrameMain*)wxGetApp().GetTopWindow();
  int x, y, xx, yy;
  event.GetPosition(&x, &y);
  CalcUnscrolledPosition(x, y, &xx, &yy);
  size_t pt = m_graf.screen_to_data_x(xx);
  if (pt >= 0 && pt < m_model.m_tp.size())
  {
    m_pt = pt;
    time_price_t tv = m_model.m_tp.at(pt);
    wxString str;
    str.Printf(wxT("%d %f"), (int)pt, tv.value);
    main->SetStatusText(str);
    Refresh();
    Update();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::OnMouseMove
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseMove(wxMouseEvent &event)
{
  wxFrameMain *main = (wxFrameMain*)wxGetApp().GetTopWindow();
  wxClientDC dc(this);
  PrepareDC(dc);
  wxString str;
  wxPoint pos = event.GetPosition();
  int x = dc.DeviceToLogicalX(pos.x);
  size_t idx = m_graf.screen_to_data_x(x);
  if (idx >= 0 && idx < m_model.m_tp.size())
  {
    time_price_t tv = m_model.m_tp.at(idx);
    wxDateTime date;
    date.Set(tv.time);
    str.Printf(wxT("%d\t\t%s\t\t%.1f"), (int)idx, date.FormatISOCombined(':'), tv.value);
  }
  main->SetStatusText(str);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::write
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::write()
{
  std::string path;
  wxFrameMain *main = (wxFrameMain*)wxGetApp().GetTopWindow();
  if (main->m_current_file.empty())
  {
    path = "ewf.txt";
  }
  else
  {
    path = main->m_current_file.ToStdString();
  }
  m_model.write(path);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::parse_response
//alpha_vantage
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::parse_response(const std::vector<time_price_t> &tp, int interval, const std::string &ticker)
{
  //model only needs interval
  m_model.m_interval = interval;
  //title and period to read file
  m_model.m_ticker = ticker;
  m_model.m_period = "not_used";
  size_t size = tp.size();

  //update
  if (m_model.m_tp.size())
  {
    for (size_t idx = 0; idx < size; idx++)
    {
      time_price_t tv = tp.at(idx);
      time_t time_last = m_model.m_tp.back().time;
      if (tv.time > time_last)
      {
        m_model.m_tp.push_back(tv);
      }
    }
  }
  else
  {
    for (size_t idx = 0; idx < size; idx++)
    {
      m_model.m_tp.push_back(tp.at(idx));
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //initialize_chart
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  initialize_chart();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::store
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::store(const std::string &wave)
{
  m_model.reset_wave(wave);
  m_model.m_tp.at(m_pt).wave = wave;
  write();
  Refresh();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::update
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::update()
{
  if (alpha)
  {
    std::vector<time_price_t> tp;

    //convert from Google to alpha
    if (m_model.m_ticker == ".DJI")
    {
      m_model.m_ticker = "^DJI";
      m_model.m_interval = m_model.m_interval / 60; //sec to min
    }
    else if (m_model.m_ticker == ".INX")
    {
      m_model.m_ticker = "^INX";
      m_model.m_interval = m_model.m_interval / 60; //sec to min
    }

    connect_alpha(m_model.m_ticker.c_str(), m_model.m_interval, TIME_SERIES_INTRADAY, api_key.c_str(), tp);
    parse_response(tp, m_model.m_interval, m_model.m_ticker.c_str());
  }
  else
  {

  }
  initialize_chart();
  Refresh();
  Update();
  write();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//DialogTrade::DialogTrade
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(DialogTrade, wxDialog)
wxEND_EVENT_TABLE()

DialogTrade::DialogTrade(wxWindow *parent)
  : wxDialog(parent, wxID_ANY, wxT("Trade"),
    wxDefaultPosition, wxDefaultSize,
    wxDEFAULT_DIALOG_STYLE)
{
  wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  wxSizer *sizer_index = new wxBoxSizer(wxHORIZONTAL);
  sizer_index->Add(new wxStaticText(this, wxID_ANY, wxT("Index")), wxSizerFlags(0).Expand().Border());
  sizer_index->Add(new wxTextCtrl(this, ID_TEXT_CTRL_TRADE_INDEX), wxSizerFlags(0).Expand().Border());
  sizer->Add(sizer_index, wxSizerFlags(0).Expand().Border());

  wxSizer *sizer_amount = new wxBoxSizer(wxHORIZONTAL);
  sizer_amount->Add(new wxStaticText(this, wxID_ANY, wxT("Amount")), wxSizerFlags(0).Expand().Border());
  sizer_amount->Add(new wxTextCtrl(this, ID_TEXT_CTRL_TRADE_AMOUNT), wxSizerFlags(0).Expand().Border());
  sizer->Add(sizer_amount, wxSizerFlags(0).Expand().Border());

  wxButton *btn = new wxButton(this, wxID_OK, _("Ok"));
  btn->SetDefault();
  sizer->Add(btn, wxSizerFlags(0).Expand().Border());

  SetSizer(sizer);
  sizer->Fit(this);
  Centre();
}

