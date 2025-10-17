#ifndef CHART_WX_HH
#define CHART_WX_HH

#include "trader.hh"
#include "grafix.hh"
#include "model.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxChart
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxChart : public wxPanel
{
public:
  wxChart(wxWindow* parent, const std::vector<entry_t>& data);

private:
  void OnPaint(wxPaintEvent& event);
  void OnMouseDown(wxMouseEvent& event);
  void OnMouseMove(wxMouseEvent& event);
  void OnMouseUp(wxMouseEvent& event);
  void OnMouseLeave(wxMouseEvent& event);
  void initialize_chart();
  size_t find_nearest_index(double x_data);

  graf_t graf;
  model_t model;
  int initialized;

  bool mouse_down;
  bool show_crosshair;
  wxPoint mouse_pos;
  size_t selected_index;

  wxDECLARE_EVENT_TABLE();
};


#endif
