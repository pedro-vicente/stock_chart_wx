#include <cstdio>
#include <ctime>
#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/filehistory.h"
#include "wx/config.h"
#include "grafix.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// init
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::init(int x_min_, int y_min_, int x_max_, int y_max_, double x_low_, double y_low_, double x_high_, double y_high_)
{
  nbr_decimals = 0;
  x_min = x_min_;
  x_max = x_max_;
  y_min = y_min_;
  y_max = y_max_;
  x_low = x_low_;
  x_high = x_high_;
  y_low = y_low_;
  y_high = y_high_;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// data_to_screen
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::data_to_screen(double x, double y, int& screen_x, int& screen_y)
{
  screen_x = int(x_min + (x_max - x_min) * (x - x_low) / (x_high - x_low));
  screen_y = int(y_max + (y_min - y_max) * (y - y_low) / (y_high - y_low));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// screen_to_data
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::screen_to_data(int screen_x, int screen_y, double& data_x, double& data_y)
{
  data_x = x_low + (screen_x - x_min) * (x_high - x_low) / (x_max - x_min);
  data_y = y_high - (screen_y - y_min) * (y_high - y_low) / (y_max - y_min);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_circle
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_circle(wxDC& dc, double x1_data, double y1_data, wxColour color)
{
  dc.SetBrush(wxBrush(color));
  int x1, y1;
  data_to_screen(x1_data, y1_data, x1, y1);
  dc.DrawCircle(x1, y1, 3);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_circle
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_circle(wxDC& dc, double x1_data, double y1_data, wxColour color, int size)
{
  dc.SetBrush(wxBrush(color));
  int x1, y1;
  data_to_screen(x1_data, y1_data, x1, y1);
  dc.DrawCircle(x1, y1, size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_text
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_text(wxDC& dc, const wxString& str, double x1_data, double y1_data, wxColour color, bool up)
{
  int x, y;
  data_to_screen(x1_data, y1_data, x, y);
  wxFont font(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL);
  dc.SetFont(font);
  dc.SetTextForeground(color);
  if (up)
  {
    y -= 20;
  }
  x -= 7;
  dc.DrawText(str, x, y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_rectangle
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_rectangle(wxDC& dc, double x1_data, double y1_data, double x2_data, double y2_data, wxColour color,
  wxBrushStyle style)
{
  dc.SetBrush(wxBrush(color, style));
  int x1, y1, x2, y2;
  data_to_screen(x1_data, y1_data, x1, y1);
  data_to_screen(x2_data, y2_data, x2, y2);
  wxCoord width = x2 - x1;
  wxCoord height = y2 - y1;
  dc.DrawRectangle(x1, y1, width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_candlestick
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_candlestick(wxDC& dc, double x_axis, double high, double low, double open, double close)
{
  //the real body represents the price range between the open and close . 
  //When the real body is filled in or red, it means the close was lower than the open. 
  //If the real body is empty or green, it means the close was higher than the open. 

  wxColour red = wxColour(255, 0, 0);
  wxColour green = wxColour(0, 255, 0);
  if (close < open)
    dc.SetBrush(wxBrush(red, wxBRUSHSTYLE_SOLID));
  else
    dc.SetBrush(wxBrush(green, wxBRUSHSTYLE_SOLID));

  int x, y_high, y_low, y_open, y_close;
  data_to_screen(x_axis, high, x, y_high);
  data_to_screen(x_axis, low, x, y_low);
  data_to_screen(x_axis, open, x, y_open);
  data_to_screen(x_axis, close, x, y_close);

  wxCoord x1 = x - 3;
  wxCoord x2 = x + 3;
  wxCoord width = x2 - x1;

  wxCoord y1 = y_close;
  wxCoord y2 = y_open;
  wxCoord height = y2 - y1;

  int z = 1;
  width *= z;
  height *= z;
  dc.DrawRectangle(x1, y1, width, height);

  //Just above and below the real body are the "shadows" or "wicks." 
  //The shadows show the high and low prices of that day's trading. If the upper shadow on a down candle is short, 
  //it indicates that the open that day was near the high of the day.
  //A short upper shadow on an up day dictates that the close was near the high.The relationship between the days open, high, low, 
  //and close determines the look of the daily candlestick.Real bodies can be long or shortand black or white.Shadows can be long or short.

  dc.SetBrush(wxBrush(wxColour(0, 0, 0), wxBRUSHSTYLE_SOLID));

  //upper shadow high, open
  dc.DrawLine(x, y_high, x, y_open);

  //lower shadow close, low
  dc.DrawLine(x, y_close, x, y_low);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_polygon
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_polygon(wxDC& dc, size_t nbr_points, PointData* points_data, wxColour color, bool border, bool is_bitmap)
{
  if (border == false)
  {
    dc.SetPen(*wxTRANSPARENT_PEN);
  }
  dc.SetBrush(wxBrush(color));
  wxPoint* points_screen = new wxPoint[nbr_points];
  for (size_t idx = 0; idx < nbr_points; idx++)
  {
    int x, y;
    data_to_screen(points_data[idx].x, points_data[idx].y, x, y);
    points_screen[idx].x = x;
    points_screen[idx].y = y;
    if (is_bitmap)
    {
      points_screen[idx].x -= x_min;
      points_screen[idx].y -= y_min;
    }
  }
  dc.DrawPolygon(nbr_points, points_screen);
  delete[] points_screen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_polygon
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_polygon(wxDC& dc, std::vector<PointData>& points_data, wxColour color_brush, wxColour color_pen)
{
  dc.SetBrush(wxBrush(color_brush));
  dc.SetPen(wxPen(color_pen));
  wxPoint* points_screen = new wxPoint[points_data.size()];
  for (size_t idx = 0; idx < points_data.size(); idx++)
  {
    int x, y;
    data_to_screen(points_data[idx].x, points_data[idx].y, x, y);
    points_screen[idx].x = x;
    points_screen[idx].y = y;
  }
  dc.DrawPolygon(points_data.size(), points_screen);
  delete[] points_screen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_back
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_back(wxDC& dc)
{
  dc.SetBrush(wxBrush(wxColour(192, 192, 192)));
  wxCoord width = x_max - x_min;
  wxCoord height = y_max - y_min;
  dc.DrawRectangle(x_min, y_min, width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_line
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_line(wxDC& dc, double x1_data, double y1_data, double x2_data, double y2_data)
{
  int x1, y1, x2, y2;
  data_to_screen(x1_data, y1_data, x1, y1);
  data_to_screen(x2_data, y2_data, x2, y2);
  dc.DrawLine(x1, y1, x2, y2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_spline
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_spline(wxDC& dc, size_t nbr_points, PointData* points_data, wxColour color)
{
  dc.SetBrush(wxBrush(color));
  wxPoint* points_screen = new wxPoint[nbr_points];
  for (size_t idx = 0; idx < nbr_points; idx++)
  {
    int x, y;
    data_to_screen(points_data[idx].x, points_data[idx].y, x, y);
    points_screen[idx].x = x;
    points_screen[idx].y = y;
  }
  dc.DrawSpline(nbr_points, points_screen);
  delete[] points_screen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_polygon
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_polygon(wxGraphicsContext* gc, size_t nbr_points, PointData* points_data, wxColour color, bool border, bool is_bitmap)
{
  if (border == false)
  {
    gc->SetPen(*wxTRANSPARENT_PEN);
  }
  gc->SetBrush(wxBrush(color));
  wxPoint2DDouble* points_screen = new wxPoint2DDouble[nbr_points];
  for (size_t idx = 0; idx < nbr_points; idx++)
  {
    int x, y;
    data_to_screen(points_data[idx].x, points_data[idx].y, x, y);
    points_screen[idx].m_x = x;
    points_screen[idx].m_y = y;
    if (is_bitmap)
    {
      points_screen[idx].m_x -= x_min;
      points_screen[idx].m_y -= y_min;
    }
  }
  gc->DrawLines(nbr_points, points_screen);
  delete[] points_screen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// draw_scale
/////////////////////////////////////////////////////////////////////////////////////////////////////

void graf_t::draw_scale(wxDC& dc)
{
  char buf[64];

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // x-axis
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (!time.empty())
  {
    int nbr_times = time.size();

    time_t first_time = time[0];
    time_t last_time = time[nbr_times - 1];
    double time_range = difftime(last_time, first_time);
    if (time_range <= 0) time_range = 1;

    bool has_hourly = false;
    if (nbr_times > 1)
    {
      double avg_diff = time_range / (nbr_times - 1);
      has_hourly = (avg_diff < 24 * 3600);
    }

    wxString sample_label = has_hourly ? "09-12 23:59" : "09-12";
    wxSize text_size = dc.GetTextExtent(sample_label);
    int min_spacing = text_size.GetWidth();
    int available_width = x_max - x_min;
    int max_labels = std::max(2, available_width / min_spacing);
    double interval_seconds = time_range / (max_labels - 1);
    int prev_label_end_x = x_min - min_spacing;
    time_t current_time = first_time;

    while (current_time <= last_time + interval_seconds)
    {
      double offset = difftime(current_time, first_time);
      double x_data = x_low + (x_high - x_low) * (offset / time_range);
      int x_screen, y_screen;
      data_to_screen(x_data, y_low, x_screen, y_screen);

      if (x_screen >= x_min && x_screen <= x_max)
      {
        std::tm* tm = localtime(&current_time);
        char buf[32];
        if (has_hourly && interval_seconds < 24 * 3600)
        {
          sprintf(buf, "%02d-%02d %02d:%02d", tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
        }
        else
        {
          sprintf(buf, "%02d-%02d", tm->tm_mon + 1, tm->tm_mday);
        }

        wxString label(buf);
        wxSize label_size = dc.GetTextExtent(label);
        int label_x = x_screen - label_size.GetWidth() / 2;
        int label_end_x = label_x + label_size.GetWidth();
        if (label_x >= prev_label_end_x + 5)
        {
          if (label_x < x_min) label_x = x_min;
          if (label_end_x > x_max) label_x = x_max - label_size.GetWidth();
          dc.DrawLine(x_screen, y_max, x_screen, y_max + 7);
          dc.DrawText(label, label_x, y_max + 10);
          prev_label_end_x = label_x + label_size.GetWidth();
        }
      }

      current_time += (time_t)interval_seconds;
    }
  }

  else
  {
    double tick = 20;
    int idx = x_min;
    double step_screen = (double)(x_max - x_min) / tick;
    double value = x_low;
    double step_value = (x_high - x_low) / tick;
    while (idx <= x_max)
    {
      dc.DrawLine(idx, y_max, idx, y_max + 7);
      sprintf(buf, "%.0f", value);
      dc.DrawText(wxString(buf), idx - strlen(buf) * 3, y_max + 10);
      idx += step_screen;
      value += step_value;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // y-axis
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  double tick = 30;
  double step_screen = (double)(y_max - y_min) / tick;
  double value = y_low;
  double step_value = (y_high > y_low) ? (y_high - y_low) / tick : (y_low - y_high) / tick;
  int idx = y_max;

  while (idx >= y_min)
  {
    dc.DrawLine(x_min, idx, x_min - 7, idx);
    if (idx < y_max)
    {
      dc.DrawLine(x_min, idx + step_screen / 2.0, x_min - 4, idx + step_screen / 2.0);
    }
    if (nbr_decimals)
    {
      sprintf(buf, "%.2f", value);
    }
    else
    {
      sprintf(buf, "%.1f", value);
    }

    wxString str(buf);
    dc.DrawText(str, x_min - strlen(buf) * 15 + 15, idx - 15);
    idx -= step_screen;
    value += step_value;
  }
}