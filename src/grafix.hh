#ifndef WX_GRAFIX_HPP
#define WX_GRAFIX_HPP 1

#include <vector>
#include <string>
#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/graphics.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// PointData
/////////////////////////////////////////////////////////////////////////////////////////////////////

class PointData
{
public:
  double x;
  double y;
  PointData(double x_, double y_) :
    x(x_),
    y(y_)
  {
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// graf_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class graf_t
{
public:
  void init(int, int, int, int, double, double, double, double);
  void draw_back(wxDC& dc);
  void draw_text(wxDC& dc, const wxString& str, double x1_data, double y1_data, wxColour color, bool up);
  void draw_circle(wxDC& dc, double x1_data, double y1_data, wxColour color);
  void draw_circle(wxDC& dc, double x1_data, double y1_data, wxColour color, int size);
  void draw_line(wxDC& dc, double x1_data, double y1_data, double x2_data, double y2_data);
  void draw_rectangle(wxDC& dc, double x1_data, double y1_data, double x2_data, double y2_data, wxColour color, wxBrushStyle style = wxBRUSHSTYLE_SOLID);
  void draw_candlestick(wxDC& dc, double x_axis, double high, double low, double open, double close);
  void draw_polygon(wxDC& dc, size_t nbr_points, PointData* points_data, wxColour color, bool border, bool is_bitmap);
  void draw_polygon(wxDC& dc, std::vector<PointData>& points, wxColour color_brush, wxColour color_pen);
  void draw_spline(wxDC& dc, size_t nbr_points, PointData* points_data, wxColour color);
  void draw_scale(wxDC& dc);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //wxGraphicsContext
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void draw_polygon(wxGraphicsContext* gc, size_t nbr_points, PointData* points_data, wxColour color, bool border, bool is_bitmap);
  void screen_to_data(int screen_x, int screen_y, double& data_x, double& data_y);
  void data_to_screen(double x_data, double y_data, int& screen_x, int& screen_y);

  int nbr_decimals;

  //converted time strings to time_t for scale drawing
  std::vector<time_t> time;

protected:
  //screen
  int x_min, x_max, y_min, y_max;
  //data
  double x_low, y_low, x_high, y_high;
};


#endif