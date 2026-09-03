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

  SearchEngine.QuerySelect(HtmlDocument, "table.past-results td a", AllAnchorTags);
  SearchEngine.QuerySelect(HtmlDocument, "table.past-results td ul li", AllBalls);

  for (size_t i = 0; i < AllAnchorTags.GetSize(); i++)
  {
    bool IsMalformed = false;  // Used to skip an entire draw if a single ball/number or date is erroneous
    DrawResult CurrentDraw;
    CurrentDraw.GameType = Game;

    lxb_dom_element_t* AnchorElement = AllAnchorTags.GetElementAt(i);
    if (AnchorElement == nullptr)
    {
      continue;
    }

    std::string HrefString = this->GetElementAttribute(AnchorElement, "href");

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
          else
          {
            std::cerr << "[WARNING] Scraper skipped row at index [" << i << "] due to missing or malformed date." << std::endl;
            IsMalformed = true;
            break;
          }

          IsDateNext = false;
        }

        if (Segment == "numbers")
        {
          IsDateNext = true;
        }
      }
    }

    if (IsMalformed == true)
    {
      IsMalformed = false;
      continue;
    }

    unsigned int BallStartIndex = i * BallCountInGame;

    for (unsigned int BallOffset = 0; BallOffset < BallCountInGame; BallOffset++)
    {
      unsigned int CurrentBallIndex = BallStartIndex + BallOffset;
      lxb_dom_element_t* BallElement = AllBalls.GetElementAt(CurrentBallIndex);

      if (BallElement != nullptr)
      {
        lxb_dom_node_t* BaseNode = lxb_dom_interface_node(BallElement);
        lxb_dom_node_t* TextNode = lxb_dom_node_first_child(BaseNode);

        if (TextNode != nullptr && lxb_dom_node_type(TextNode) == LXB_DOM_NODE_TYPE_TEXT)
        {
          lxb_dom_character_data_t* CharData = lxb_dom_interface_character_data(TextNode);
          std::string BallString(reinterpret_cast<const char*>(CharData->data.data), CharData->data.length);

          if (BallString.empty() == false && IsNumericString(BallString) == true)
          {
            CurrentDraw.Numbers.push_back(std::stoul(BallString));
          }
          else
          {
            IsMalformed = true;
            break;
          }
        }
        else
        {
          IsMalformed = true;
          break;
        }
      }
      else
      {
        IsMalformed = true;
        break;
      }
    }

    if (IsMalformed == false && CurrentDraw.Numbers.empty() == false)
    {
      CurrentDraw.BonusNumber = CurrentDraw.Numbers.back();
      CurrentDraw.Numbers.pop_back();
      RC.push_back(CurrentDraw);
    }
    else
    {
      std::cerr << "[WARNING] Scraper skipped row for date [" << CurrentDraw.Date << "] due to malform draw number." << std::endl;
      IsMalformed = false;
    }
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
