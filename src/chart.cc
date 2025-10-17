#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <set>
#include "trader.hh"
#include "display.hh"
#include "chart.hh"
#include "get.hh"
#include "grafix.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxChart
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(wxChart, wxPanel)
EVT_PAINT(wxChart::OnPaint)
EVT_LEFT_DOWN(wxChart::OnMouseDown)
EVT_MOTION(wxChart::OnMouseMove)
EVT_LEFT_UP(wxChart::OnMouseUp)
EVT_LEAVE_WINDOW(wxChart::OnMouseLeave)
wxEND_EVENT_TABLE()

wxChart::wxChart(wxWindow* parent, const std::vector<entry_t>& data)
  : wxPanel(parent, wxID_ANY),
  initialized(0),
  model(data),
  mouse_down(false),
  show_crosshair(false),
  selected_index(0)
{
  SetBackgroundColour(wxColour(240, 240, 240));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// find_nearest_index
/////////////////////////////////////////////////////////////////////////////////////////////////////

size_t wxChart::find_nearest_index(double x_data)
{
  size_t nbr_pts = model.entry.size();
  if (nbr_pts == 0)
  {
    return 0;
  }

  size_t nearest_idx = 0;
  double min_dist = 1E10;
  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    double x = difftime(graf.time[idx], graf.time[0]);
    double dist = fabs(x - x_data);
    if (dist < min_dist)
    {
      min_dist = dist;
      nearest_idx = idx;
    }
  }

  return nearest_idx;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxChart::initialize_chart
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::initialize_chart()
{
  double x_low, y_low, x_high, y_high;
  int x_min, x_max, y_min, y_max;

  size_t nbr_pts = model.entry.size();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //convert timestamps to time_t
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    graf.time.push_back(parse_timestamp(model.entry.at(idx).timestamp));
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // X data points, time 
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  x_low = 0;
  x_high = nbr_pts;
  if (nbr_pts > 0)
  {
    x_high = difftime(graf.time[nbr_pts - 1], graf.time[0]);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Y data points 
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  y_low = 1E6;
  y_high = -1E6;
  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    double v = model.entry.at(idx).close;
    if (v > y_high)
    {
      y_high = v;
    }
    if (v < y_low)
    {
      y_low = v;
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // screen
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  //origin
  x_min = 80;
  y_min = 0;
  wxSize size = GetClientSize();
  x_max = size.GetWidth() - 50;
  y_max = size.GetHeight() - 50;
  graf.init(x_min, y_min, x_max, y_max, x_low, y_low, x_high, y_high);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// OnPaint
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  wxSize size = GetClientSize();
  if (!initialized)
  {
    initialize_chart();
    initialized = 1;
  }

  size_t nbr_pts = model.entry.size();
  graf.draw_back(dc);
  if (nbr_pts == 0)
  {
    return;
  }
  graf.draw_scale(dc);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //data line
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  for (size_t idx = 1; idx < nbr_pts; idx++)
  {
    entry_t e1 = model.entry.at(idx - 1);
    entry_t e2 = model.entry.at(idx);
    double x1 = difftime(graf.time[idx - 1], graf.time[0]);
    double x2 = difftime(graf.time[idx], graf.time[0]);
    graf.draw_line(dc, x1, e1.close, x2, e2.close);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //data points
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    entry_t e = model.entry.at(idx);
    double x = difftime(graf.time[idx], graf.time[0]);
    graf.draw_circle(dc, x, e.close, wxColour(0, 255, 0));
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //selected point
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  entry_t e = model.entry.at(this->selected_index);
  double x = difftime(graf.time[this->selected_index], graf.time[0]);
  graf.draw_circle(dc, x, e.close, wxColour(255, 0, 0));

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // crosshair and tooltip
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (show_crosshair)
  {
    dc.SetPen(wxPen(wxColour(100, 100, 100), 1, wxPENSTYLE_DOT));
    dc.DrawLine(mouse_pos.x, 0, mouse_pos.x, size.GetHeight());
    dc.DrawLine(0, mouse_pos.y, size.GetWidth(), mouse_pos.y);

    double x_data, y_data;
    graf.screen_to_data(mouse_pos.x, mouse_pos.y, x_data, y_data);

    size_t nearest_idx = find_nearest_index(x_data);

    if (nbr_pts > 0)
    {
      wxString tooltip = wxString::Format("%s: %.2f", model.entry[nearest_idx].timestamp.c_str(), model.entry[nearest_idx].close);
      dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
      wxSize text_size = dc.GetTextExtent(tooltip);
      int tooltip_x = mouse_pos.x + 10;
      int tooltip_y = mouse_pos.y - 20;
      if (tooltip_x + text_size.GetWidth() + 10 > size.GetWidth())
        tooltip_x = mouse_pos.x - text_size.GetWidth() - 10;
      if (tooltip_y < 0)
        tooltip_y = mouse_pos.y + 10;
      dc.SetBrush(wxBrush(wxColour(255, 255, 200)));
      dc.SetPen(wxPen(wxColour(0, 0, 0), 1));
      dc.DrawRectangle(tooltip_x, tooltip_y, text_size.GetWidth() + 10, text_size.GetHeight() + 6);
      dc.DrawText(tooltip, tooltip_x + 5, tooltip_y + 3);
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //volume bars
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  double max_volume = 0;
  for (size_t idx = 0; idx < nbr_pts; idx++)
  {
    if (model.entry.at(idx).volume > max_volume)
    {
      max_volume = model.entry.at(idx).volume;
    }
  }

  if (max_volume > 0)
  {
    double volume_height_ratio = 0.2;
    wxSize chart_size = GetClientSize();
    int y_max_screen = chart_size.GetHeight() - 50;
    int y_min_screen = 0;
    int volume_area_height = (y_max_screen - y_min_screen) * volume_height_ratio;
    for (size_t idx = 0; idx < nbr_pts; idx++)
    {
      entry_t e = model.entry.at(idx);
      double x = difftime(graf.time[idx], graf.time[0]);
      double normalized_volume = e.volume / max_volume;
      int bar_height = normalized_volume * volume_area_height;
      wxColour bar_color = (e.close >= e.open) ? wxColour(0, 200, 0, 128) : wxColour(200, 0, 0, 128);
      int screen_x, screen_y;
      graf.data_to_screen(x, e.close, screen_x, screen_y);
      dc.SetBrush(wxBrush(bar_color));
      dc.SetPen(wxPen(bar_color));
      dc.DrawRectangle(screen_x - 2, y_max_screen - bar_height, 4, bar_height);
    }
  }

  wxBitmap bmp(dc.GetSize().GetWidth(), dc.GetSize().GetHeight());
  wxMemoryDC mdc(bmp);
  mdc.Blit(0, 0, bmp.GetWidth(), bmp.GetHeight(), &dc, 0, 0);
  mdc.SelectObject(wxNullBitmap);
  bmp.SaveFile("chart.bmp", wxBITMAP_TYPE_BMP);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// OnMouseDown
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseDown(wxMouseEvent& event)
{
  mouse_down = true;
  mouse_pos = event.GetPosition();
  show_crosshair = true;

  size_t nbr_pts = model.entry.size();
  if (nbr_pts > 0)
  {
    double x_data, y_data;
    graf.screen_to_data(mouse_pos.x, mouse_pos.y, x_data, y_data);
    this->selected_index = find_nearest_index(x_data);
  }

  Refresh();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// OnMouseMove
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseMove(wxMouseEvent& event)
{
  if (event.Dragging() || mouse_down)
  {
    mouse_pos = event.GetPosition();
    show_crosshair = true;
    Refresh();
  }
  else if (event.Moving())
  {
    mouse_pos = event.GetPosition();
    show_crosshair = true;
    Refresh();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// OnMouseUp
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseUp(wxMouseEvent& event)
{
  mouse_down = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// OnMouseLeave
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxChart::OnMouseLeave(wxMouseEvent& event)
{
  show_crosshair = false;
  mouse_down = false;
  Refresh();
}