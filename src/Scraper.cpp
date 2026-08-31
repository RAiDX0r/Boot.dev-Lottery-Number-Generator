// Takes raw HTML code and feeds it into the HtmlParser library and extracts necessary data.

#include "Scraper.hpp"

#include <HtmlParser/Query.hpp>
#include <iostream>

std::vector<DrawResult> Scraper::ParseHtml(const std::string& RawHtml) const
{
  std::vector<DrawResult> RC;

  if (RawHtml.empty() == true)
  {
    return RC;
  }

  HtmlParser::Parser WebParser;
  HtmlParser::DOM DocumentObjectTree = WebParser.Parse(RawHtml);  // Parse raw string to in-memory document tree object
  HtmlParser::Query SearchEngine(DocumentObjectTree.Root());  // Initialise query engine starting at the root node of the page
  auto AllDraws = SearchEngine.Select(".draw");

  for (const auto& Draw : AllDraws)
  {
    HtmlParser::Query DrawSearchEngine(Draw);
    auto AnchorWithDate = DrawSearchEngine.SelectFirst("a.details-btn");
  }

  return RC;
}
