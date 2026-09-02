// Takes raw HTML code and feeds it into the HtmlParser library and extracts necessary data.

#include "Scraper.hpp"

#include <iostream>
#include <sstream>

#include "LexborDocument.hpp"
#include "LexborSelector.hpp"

std::vector<DrawResult> Scraper::ParseHtml(const std::string& RawHtml, LotteryGame Game) const
{
  std::vector<DrawResult> RC;

  if (RawHtml == "")
  {
    return RC;
  }

  unsigned int BallCountInGame = GetBallCountForGame(Game);
  LexborDocument HtmlDocument(RawHtml);
  LexborSelector SearchEngine;
  LexborCollection AllAnchorTags;
  LexborCollection AllBalls;

  SearchEngine.QuerySelect(HtmlDocument, "table.past-results tr a.details-btn, .draw a.details-btn", AllAnchorTags);
  SearchEngine.QuerySelect(HtmlDocument, "table.past-results tr li, .draw li", AllBalls);

  for (size_t i = 0; i < AllAnchorTags.GetSize(); i++)
  {
    DrawResult CurrentDraw;
    CurrentDraw.GameType = Game;

    // Get the Href value of the anchor tag which has the date information
    std::string HrefString = this->GetElementAttribute(AllAnchorTags.GetElementAt(i), "href");

    if (HrefString.empty() == false)
    {
      std::stringstream HrefStream(HrefString);
      std::string Segment;
      bool IsDateNext = false;

      while (std::getline(HrefStream, Segment, '/'))
      {
        if (Segment.empty() == true)
        {
          continue;
        }

        if (IsDateNext == true)
        {
          if (Segment.length() >= 10)
          {
            CurrentDraw.Date = Segment.substr(0, 10);
          }

          IsDateNext = false;
        }

        if (Segment == "numbers")
        {
          IsDateNext = true;
        }
      }
    }

    // Find the start of the balls for the current draw by multiplying i by the number of balls for the game
    unsigned int BallStartIndex = i * BallCountInGame;

    // Iterate through the balls for this draw
    for (unsigned int BallOffset = 0; BallOffset < BallCountInGame; BallOffset++)
    {
      unsigned int CurrentBallIndex = BallStartIndex + BallOffset;
      lxb_dom_element_t* BallElement = AllBalls.GetElementAt(CurrentBallIndex);

      if (BallElement != nullptr)
      {
        // The <li> will only ever have a ball number and no other nested HTML
        lxb_dom_node_t* TextNode = lxb_dom_node_first_child(lxb_dom_interface_node(BallElement));

        // Validate the Text node is proper
        if (TextNode != nullptr && lxb_dom_node_type(TextNode) == LXB_DOM_NODE_TYPE_TEXT)
        {
          // Use Lexbor to get the raw chracter array
          lxb_dom_character_data_t* CharData = lxb_dom_interface_character_data(TextNode);
          std::string BallString(reinterpret_cast<const char*>(CharData->data.data), CharData->data.length);
          CurrentDraw.Numbers.push_back(std::stoul(BallString));
        }
      }
    }

    CurrentDraw.BonusNumber = CurrentDraw.Numbers.back();
    CurrentDraw.Numbers.pop_back();
    RC.push_back(CurrentDraw);
  }

  return RC;
}

std::string Scraper::GetElementAttribute(lxb_dom_element_t* Element, const std::string& AttrName) const
{
  if (Element == nullptr)
  {
    return "";
  }

  lxb_dom_attr_t* AttrNode = lxb_dom_element_attr_by_name(
      Element,
      reinterpret_cast<const lxb_char_t*>(AttrName.c_str()),
      AttrName.length());

  if (AttrNode != nullptr)
  {
    size_t ValueLength = 0;
    const lxb_char_t* ValueBytes = lxb_dom_attr_value(AttrNode, &ValueLength);

    if (ValueBytes != nullptr)
    {
      return std::string(reinterpret_cast<const char*>(ValueBytes), ValueLength);
    }
  }

  return "";
}
