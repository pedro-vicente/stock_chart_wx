#ifndef DISPLAY_WX_HH
#define DISPLAY_WX_HH

#include "trader.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// PanelDisplay
/////////////////////////////////////////////////////////////////////////////////////////////////////

class PanelDisplay : public wxPanel
{
public:
  PanelDisplay(wxWindow* parent);
  void get_data();
 
private:
  int read();
  int read_csv(const std::string& file_name, std::vector<entry_t>& entry);
  int get_intraday();
  wxSplitterWindow* splitter;
  std::vector<entry_t> entry;
  wxDECLARE_EVENT_TABLE();
};


#endif
