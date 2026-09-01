// Takes raw HTML code and feeds it into the HtmlParser library and extracts necessary data.

#include "Scraper.hpp"

#include <HtmlParser/Query.hpp>
#include <iostream>
#include <sstream>
#include <string_view>

std::vector<DrawResult> Scraper::ParseHtml(const std::string& RawHtml) const
{
  std::vector<DrawResult> RC;

  if (RawHtml.empty() == true)
  {
    return RC;
  }

  HtmlParser::Parser WebParser;
  HtmlParser::DOM DocumentObjectTree = WebParser.Parse(RawHtml);  // Parse raw string to in-memory document tree object
  HtmlParser::Query SearchEngine(DocumentObjectTree.Root());      // Initialise query engine starting at the root node of the page
  auto AllDraws = SearchEngine.Select(".draw");

  for (const auto& Draw : AllDraws)
  {
    DrawResult CurrentDraw;
    // Create a Query object to search only the draw
    HtmlParser::Query DrawSearchEngine(Draw);

    // Find the anchor tag that has the date and extract it
    auto AnchorWithDate = DrawSearchEngine.SelectFirst("a.details-btn");

    if (AnchorWithDate != nullptr)
    {
      std::string Href = AnchorWithDate->GetAttribute("href");
      std::stringstream HrefStream(Href);
      std::string Segment;
      bool NextSegmentIsDate = false;

      while (std::getline(HrefStream, Segment, '/'))
      {
        if (Segment.empty() == true)
        {
          continue;
        }

        // Check for game type
        // If the game type is unknown the programme will throw an exception anyways.
        // BONUS: Don't need to scale game types here
        std::string_view SegmentView(Segment);

        if (SegmentView.length() >= 6 && SegmentView.substr(0, 6) == "lotto-")
        {
          CurrentDraw.GameType = StringToGame(Segment);
        }

        // Check for date
        if (NextSegmentIsDate == true)
        {
          CurrentDraw.Date = Segment;
          NextSegmentIsDate = false;
        }

        if (Segment == "numbers")
        {
          NextSegmentIsDate = true;
        }
      }
    }

    // Find and extract ball numbers
    auto Balls = DrawSearchEngine.Select("li.ball");

    for (const auto& Ball : Balls)
    {
      if (Ball->HasClass("bonus-ball") == true)
      {
        CurrentDraw.BonusNumber = std::stoul(Ball->GetTextContent());
      }
      else
      {
        CurrentDraw.Numbers.push_back(std::stoul(Ball->GetTextContent()));
      }
    }

    RC.push_back(CurrentDraw);
  }

  return RC;
}
