#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "trader.hh"
#include "display.hh"
#include "chart.hh"
#include "get.hh"
#include "grafix.hh"
#include <algorithm>
#include <set>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// PanelDisplay
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(PanelDisplay, wxPanel)
wxEND_EVENT_TABLE()

PanelDisplay::PanelDisplay(wxWindow* parent) :
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
  SetBackgroundColour(wxColour(0, 255, 0));
  splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_THIN_SASH);
  splitter->SetSashInvisible(false);
  PanelInput* panel = new PanelInput(splitter);
  read();
  wxChart* graf = new wxChart(splitter, entry);
  splitter->SplitHorizontally(panel, graf, 30);
  wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(splitter, 1, wxGROW | wxALL, 0);
  SetSizerAndFit(sizer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// read_csv
/////////////////////////////////////////////////////////////////////////////////////////////////////

int PanelDisplay::read_csv(const std::string& file_name, std::vector<entry_t>& data)
{
  std::ifstream ifs(file_name);
  if (!ifs.is_open())
  {
    return -1;
  }

  std::string line;
  if (!std::getline(ifs, line))
  {
    ifs.close();
    return -1;
  }

  int nbr_rows = 0;
  while (std::getline(ifs, line))
  {
    std::stringstream ss(line);
    entry_t e;
    std::string token;
    std::getline(ss, e.timestamp, ',');
    std::getline(ss, token, ',');
    e.open = std::stod(token);
    std::getline(ss, token, ',');
    e.high = std::stod(token);
    std::getline(ss, token, ',');
    e.low = std::stod(token);
    std::getline(ss, token, ',');
    e.close = std::stod(token);
    std::getline(ss, token);
    e.volume = std::stol(token);
    data.push_back(e);
    nbr_rows++;
  }

  ifs.close();
  return nbr_rows;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// read
/////////////////////////////////////////////////////////////////////////////////////////////////////

int PanelDisplay::read()
{
  int nbr_rows = read_csv("SPY.csv", this->entry);
  wxFrame* frame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
  if (frame)
  {
    wxString msg = wxString::Format("Loaded %d records from SPY.csv", nbr_rows);
    frame->SetStatusText(msg);
  }
  return nbr_rows;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// get_intraday
/////////////////////////////////////////////////////////////////////////////////////////////////////

int PanelDisplay::get_intraday()
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // read API key from file
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::ifstream key_file("api_key.txt");
  if (!key_file.is_open())
  {
    return -1;
  }

  std::string key;
  std::getline(key_file, key);
  key_file.close();

  if (key.empty())
  {
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // get new data from web
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (get(key) < 0)
  {
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // read new data
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<entry_t> data;
  int nbr_rows = read_csv("SPY_intraday.csv", data);
  if (nbr_rows == 0)
  {
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // merge with existing data, removing duplicates based on timestamp
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::set<std::string> timestamp;
  for (size_t idx = 0; idx < entry.size(); ++idx)
  {
    const entry_t& e = entry[idx];
    timestamp.insert(e.timestamp);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // find() returns an iterator to the element if found, or timestamp.end() if not found
  // comparison is true if the timestamp is not found in the set, meaning this is a new, unique record.
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  int count = 0;
  for (size_t idx = 0; idx < data.size(); ++idx)
  {
    const entry_t& e = data[idx];
    if (timestamp.find(e.timestamp) == timestamp.end())
    {
      entry.push_back(e);
      count++;
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // sort by timestamp
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::sort(entry.begin(), entry.end(), [](const entry_t& a, const entry_t& b) {
    return a.timestamp < b.timestamp;
    }
  );

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // write merged data 
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::ofstream ofs("SPY.csv");
  ofs << "timestamp,open,high,low,close,volume\n";
  for (size_t idx = 0; idx < entry.size(); ++idx)
  {
    const entry_t& e = entry[idx];
    ofs << e.timestamp << ","
      << e.open << ","
      << e.high << ","
      << e.low << ","
      << e.close << ","
      << e.volume << "\n";
  }
  ofs.close();

  wxFrame* frame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
  if (frame)
  {
    wxString msg = wxString::Format("Added %d new records (total: %zu)", count, entry.size());
    frame->SetStatusText(msg);
  }

  return count;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// get_data
/////////////////////////////////////////////////////////////////////////////////////////////////////

void PanelDisplay::get_data()
{
  get_intraday();
  wxChart* graf = new wxChart(splitter, entry);
  wxWindow* win = splitter->GetWindow2();
  splitter->ReplaceWindow(win, graf);
  win->Destroy();
}
