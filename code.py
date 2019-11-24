#!/usr/bin/env python3

# Copyright 2019 Christopher Mumford
# This code is licensed under MIT license (see LICENSE for details)

# apt-get install python3-pil
from PIL import Image, ImageDraw, ImageFont
# apt-get install python3-feedparser
import feedparser

draw_borders = False
alert_url = 'http://www.baaqmd.gov/Feeds/AlertRSS.aspx'
forecast_url= 'http://www.baaqmd.gov/Feeds/AirForecastRSS.aspx'

image_size = (264, 176)
image_rect = (0, 0, image_size[0]-1, image_size[1]-1)
today_rect = (0, 0, image_rect[2], 88)
forecast_width = int(image_size[0] / 3)
forecast_rects = [
    (0 * forecast_width, today_rect[3], 1 * forecast_width, image_rect[3]),
    (1 * forecast_width, today_rect[3], 2 * forecast_width, image_rect[3]),
    (2 * forecast_width, today_rect[3], 3 * forecast_width, image_rect[3]),
    (3 * forecast_width, today_rect[3], image_rect[2], image_rect[3]),
]
black = (0, 0, 0, 255)
white = (255, 255, 255, 255)
red = (255, 0, 0, 255)
blue = (0, 0, 255, 255)
num_aqi_categories = 6

def IsAlert(rss_entry):
    return rss_entry.summary == 'Alert In Effect'

def GetAlertText(rss_entry):
    return rss_entry.summary

def GetEntryDate(rss_entry):
    return rss_entry.updated

def GetEntryDateAbbrev(rss_entry):
    prefix = 'BAAQMD Air Quality Forecast for '
    idx = rss_entry.title.find(prefix)
    if idx < 0:
        return 'Unknown'
    idx = len(prefix)
    return rss_entry.title[idx:idx+3]

def GetRangeNameAbbrev(value):
    if value == 'Good':
        return 'G'
    if value == 'Moderate':
        return 'M'
    if value == 'Unhealthy for Sensitive Groups':
        return 'USG'
    if value == 'Unhealthy':
        return 'U'
    if value == 'Very Unhealthy':
        return 'VH'
    if value == 'Hazardous':
        return 'H'
    return value

def GetAQIRangeName(value):
    """Return the AQI range name given an integer value 0..500.

    http://www.sparetheair.org/understanding-air-quality/reading-the-air-quality-index
    """
    if value <= 50:
        return ('Good', 'G')
    if value <= 100:
        return ('Moderate', 'M')
    if value <= 150:
        return ('Unhealthy for Sensitive Groups', 'USG')
    if value <= 200:
        return ('Unhealthy', 'U')
    if value <= 300:
        return ('Very Unhealthy', 'VH')
    return ('Hazardous', 'H')

def GetValue(text, valname):
    """Primitive routine to retrieve a value from a string of this form:
    'Santa Clara Valley - AQI: 80, Pollutant: PM2.5'

    AQI: 0..500 http://www.sparetheair.org/understanding-air-quality/reading-the-air-quality-index
    101+ in any district triggers a spair the air.
    """
    find_str = valname + ': '
    idx = text.find(find_str)
    if idx < 0:
        return None
    endidx = text.find(',', idx)
    if endidx < 0:
        endidx = len(text)
    if endidx > idx:
        return text[idx + len(find_str):endidx]
    return None

def DrawScale(ctx, tl, rss_entry):
    """Draw the AQI scale and where |rss_entry| is on that scale."""
    rect_size = 10
    bounds = (tl, (tl[0] + rect_size, tl[1] + num_aqi_categories * rect_size))
    ctx.rectangle(bounds, fill=None, outline=black)
    for i in range(num_aqi_categories):
        x = tl[0]
        y = tl[1] + i * rect_size
        ctx.line(((x, y), (x + rect_size, y)), fill=black)

def DrawTodayEntry(rss_entry, ctx):
    if draw_borders:
        ctx.rectangle(today_rect, fill=None, outline=blue)
    margin = 4
    ctx.text((margin,margin), GetEntryDate(rss_entry),
            font=large_font, fill=black)
    alert_color = red if IsAlert(rss_entry) else black
    ctx.text((margin + 8, 50), GetAlertText(rss_entry),
            font=status_font, fill=alert_color)

def ParseSummary(rss_entry):
    districts = dict()
    delim = ' - '
    for line in rss_entry.summary.splitlines():
        idx = line.find(delim)
        if idx <= 0:
            next
        name = line[0:idx]
        values = line[idx:]
        district = dict()
        val = GetValue(values, 'AQI')
        if val:
            district['AQI'] = val
            try:
                district['AQI labels'] = GetAQIRangeName(int(val))
                district['AQI disp'] = val
            except ValueError as ex:
                # Forecasts sometimes only forcast the range and not a specific
                # value.
                district['AQI labels'] = (val, GetRangeNameAbbrev(val))
                district['AQI disp'] = district['AQI labels'][1]

        val = GetValue(values, 'Pollutant')
        if val:
            district['Pollutant'] = val
            districts[name] = district
    return districts

def DrawForecast(rss_entry, entry_idx, ctx):
    bounds = forecast_rects[entry_idx]
    if draw_borders:
        ctx.rectangle(bounds, fill=None, outline=blue)
    margin = 4
    ctx.text((margin + bounds[0], margin + bounds[1]),
            GetEntryDateAbbrev(rss_entry),
    font=medium_font, fill=black)
    line_height = 14
    districts = ParseSummary(rss_entry)
    my_district = districts['Santa Clara Valley']
    aqi = my_district['AQI disp']
    hoffset = 14
    ctx.text((margin+hoffset+bounds[0],
              margin+bounds[1] + line_height),
              aqi, font=forecast_font, fill=black)
    DrawScale(ctx, (bounds[2]-16, bounds[1] + 22), rss_entry)

def DrawForecastLines(ctx):
    ctx.line(((0, today_rect[3]), (today_rect[2], today_rect[3])), fill=black)
    for r in forecast_rects[:-1]:
        ctx.line(((r[2], r[1]), (r[2], r[3])), fill=black)

def DrawForecasts(rss_entries, ctx):
    """The first "forecast" is today, so ignore it."""
    num_forecasts = min(3, len(rss_entries))
    for idx in range(num_forecasts):
        DrawForecast(rss_entries[idx+1], idx, ctx)

def DrawLogo(img):
    logo=Image.open('images/winter_logo.png')
    offset=(image_size[0] - 80 - 2, 4)
    img.paste(logo, offset)

alert_feed = feedparser.parse(alert_url)
# Uncomment for testing.
#alert_feed.entries[0].summary = 'Alert In Effect'
forecast_feed = feedparser.parse(forecast_url)

large_font = ImageFont.truetype("fonts/windows_command_prompt.ttf", 16)
medium_font = ImageFont.truetype("fonts/windows_command_prompt.ttf", 16)
status_font_size = 22 if IsAlert(alert_feed.entries[0]) else 28
status_font = ImageFont.truetype("fonts/LibreBaskerville-Bold.ttf",
                                 status_font_size)
forecast_font = ImageFont.truetype("fonts/LibreBaskerville-Bold.ttf", 28)

img = Image.new('RGB', image_size)
ctx = ImageDraw.Draw(img)
ctx.fontmode = "1"

# Clear context.
ctx.rectangle(image_rect, white);

DrawLogo(img)
DrawTodayEntry(alert_feed.entries[0], ctx)
DrawForecastLines(ctx)
DrawForecasts(forecast_feed.entries[1:], ctx)

# May require ImageMagick (varies by platform).
mult = 2
disp = img.resize((mult * image_size[0], mult * image_size[1]))
disp.show("Spare the Air")
