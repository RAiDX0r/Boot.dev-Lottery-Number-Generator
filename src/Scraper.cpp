// Takes raw HTML code and feeds it into the HtmlParser library and extracts necessary data.

#include "Scraper.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>

#include "LexborDocument.hpp"
#include "LexborSelector.hpp"

std::vector<DrawResult> Scraper::ParseHtml(const std::string& RawHtml, const GameDefinition& GameDef, const GameSource& Source) const
{
  std::vector<DrawResult> RC;

  if (RawHtml == "")
  {
    return RC;
  }

  unsigned int BallCountInGame = GameDef.BallCount;
  LexborDocument HtmlDocument(RawHtml);
  LexborSelector SearchEngine;
  LexborCollection AllDates;
  LexborCollection AllBalls;

  SearchEngine.QuerySelect(HtmlDocument, Source.DateSelector, AllDates);
  SearchEngine.QuerySelect(HtmlDocument, Source.BallSelector, AllBalls);

  for (size_t i = 0; i < AllDates.GetSize(); i++)
  {
    bool IsMalformed = false;  // Used to skip an entire
    DrawResult CurrentDraw;
    CurrentDraw.GameType = GameDef.Id;

    lxb_dom_element_t* DateElement = AllDates.GetElementAt(i);
    if (DateElement == nullptr)
    {
      continue;
    }

    // Parse Date
    std::string HrefString;

    if (Source.DateStrategy == "url_segment")
    {
      HrefString = this->GetElementAttribute(DateElement, "href");
    }
    else if (Source.DateStrategy == "text_parse")
    {
    }
    else
    {
      HrefString = "";
      IsMalformed = true;
    }

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
    else
    {
      IsMalformed = true;
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

std::optional<std::string> Scraper::ParseDate(lxb_dom_element_t* DateElement, const std::string& Strategy, const std::string& DateParseString) const
{
  /*
  Convert DateElement into a generic Lexbor Node type and get
  inner text and text length.
  */
  lxb_dom_node_t* Node = lxb_dom_interface_node(DateElement);
  size_t TextLength = 0;
  lxb_char_t* Text = lxb_dom_node_text_content(Node, &TextLength);

  if (Text == nullptr)
  {
    return std::nullopt;  // No text or none found
  }

  constexpr std::string_view DOTW = "{DotW}";
  constexpr std::string_view DAY = "{Day}";
  constexpr std::string_view MONTH = "{Month}";
  constexpr std::string_view YEAR = "{Year}";

  std::string RC;
  std::string_view DateText(reinterpret_cast<const char*>(Text), TextLength);
  std::vector<std::string_view> DateTextSplit;
  size_t DotwLocation = DateText.find(DOTW);
  size_t DayLocation = DateText.find(DAY);
  size_t MonthLocation = DateText.find(MONTH);
  size_t YearLocation = DateText.find(YEAR);
  std::array DatePartsOrder = {
      std::pair{DotwLocation, DOTW},
      std::pair{DayLocation, DAY},
      std::pair{MonthLocation, MONTH},
      std::pair{YearLocation, YEAR}};

  std::sort(DatePartsOrder.begin(), DatePartsOrder.end());

  for (int i = 0; i < 4; i++)
  {
    size_t CurrentPosition = 0;
    if (DatePartsOrder[i].first == 0)
    {
    }
  }

  return RC;
}
