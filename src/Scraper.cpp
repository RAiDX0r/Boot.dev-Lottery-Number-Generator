// Takes raw HTML code and feeds it into the HtmlParser library and extracts necessary data.

#include "Scraper.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <iostream>
#include <sstream>
#include <string_view>
#include <unordered_map>
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

std::optional<std::string> Scraper::ParseDate(lxb_dom_element_t* DateElement, const std::string& Strategy, std::string_view DateParseString) const
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
  const std::array<std::string_view, 4> Identifiers = {DOTW, DAY, MONTH, YEAR};

  std::string RC;
  std::string_view DateText(reinterpret_cast<const char*>(Text), TextLength);
  std::vector<std::string_view> DateTextSplit;
  // Find Identifier locations in DateParseString
  size_t DotwLocation = DateParseString.find(DOTW);
  size_t DayLocation = DateParseString.find(DAY);
  size_t MonthLocation = DateParseString.find(MONTH);
  size_t YearLocation = DateParseString.find(YEAR);
  std::vector<std::string_view> PatternTokens = TokenisePattern(DateParseString, Identifiers);
  std::unordered_map<std::string_view, std::string_view> PartStage;  // Map to hold sliced data to build RC
  std::string_view PendingIdentifier = {};
  size_t Cursor = 0;
  size_t ValueStart = 0;
  bool FoundIdentifier = false;

  // Build PartStage to help find parts in DateElement
  for (const auto& Token : PatternTokens)
  {
    // Find Identifier
    if (std::find(Identifiers.begin(), Identifiers.end(), Token) != Identifiers.end())
    {
      PendingIdentifier = Token;
      ValueStart = Cursor;
      // Cursor does not advance. The value's end is determined by the next literal
    }
    else
    {
      size_t FoundAt = DateText.find(Token, Cursor);

      if (FoundAt == std::string_view::npos)
      {
        return std::nullopt;  // Malformed somehow
      }

      // Found data for Identifier
      if (PendingIdentifier.empty() == false)
      {
        PartStage[PendingIdentifier] = DateText.substr(ValueStart, FoundAt - ValueStart);
        PendingIdentifier = {};
      }

      Cursor = FoundAt + Token.length();
    }
  }

  // Pattern ends with an identifier
  if (PendingIdentifier.empty() == false)
  {
    PartStage[PendingIdentifier] = DateText.substr(ValueStart, DateText.length() - ValueStart);
  }

  // Start processing parts and building RC
  RC.reserve(10);               // Guarunteed one heap allocation | YYYY-MM-DD is 10 characters
  std::array<char, 12> Buffer;  // Local buffer to safetly append components

  
  // TODO: Parse year to ensure a proper year
  // Append Year
  // if (auto [DataPointer, ErrorCode] = std::to_chars(Buffer.data(), Buffer.data() + Buffer.size(), PartStage[YEAR]); ErrorCode == std::errc{})
  // {
  // }



  return RC;
}

std::vector<std::string_view> Scraper::TokenisePattern(std::string_view DateParseString, const std::array<std::string_view, 4> Identifiers) const
{
  std::array<std::pair<size_t, std::string_view>, 4> OrderedIdentifierLocations = {
      std::pair<size_t, std::string_view>{DateParseString.find(Identifiers[0]), Identifiers[0]},
      std::pair<size_t, std::string_view>{DateParseString.find(Identifiers[1]), Identifiers[1]},
      std::pair<size_t, std::string_view>{DateParseString.find(Identifiers[2]), Identifiers[2]},
      std::pair<size_t, std::string_view>{DateParseString.find(Identifiers[3]), Identifiers[3]}};
  std::vector<std::string_view> RC;
  size_t Start = 0;

  // Inherently uses the pair's first property to sort
  std::sort(OrderedIdentifierLocations.begin(), OrderedIdentifierLocations.end());

  for (const auto& [Position, Identifier] : OrderedIdentifierLocations)
  {
    // Identifier not found so skip
    if (Position == std::string_view::npos)
    {
      continue;
    }

    // Literal is before Identifier and not position 0, slice it
    if (Position > Start)
    {
      RC.push_back(DateParseString.substr(Start, Position - Start));
    }

    // Get Identifier next
    RC.push_back(DateParseString.substr(Position, Identifier.length()));

    Start = Position + Identifier.length();
  }

  // Get trailing literal if it exists
  if (Start < DateParseString.length())
  {
    RC.push_back(DateParseString.substr(Start, DateParseString.length() - Start));
  }

  return RC;
}

std::string Scraper::ProcessMonthToMM(const std::string_view Month) const
{
  constexpr std::string_view FALLBACK = "00";
  // Performant lookup table for two-digit return strings
  static constexpr std::array<std::string_view, 13> TWO_DIGIT_MONTHS = {
      "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};

  if (Month.empty() == true)
  {
    return std::string(FALLBACK);
  }

  std::string RC = std::string(FALLBACK);

  // Check if Month is a number
  if (std::isdigit(Month[0] == true))
  {
    int MonthNumber = 0;
    /**
     * We need a starting and ending pointer because we are not garunteed a \0 because a string_view
     * is a slice of the original string.
     * DataPointer is a raw pointer to the underlying data and is used by the function to read the data.
     * In HD scenario, it is the same as the end of the string_view.
     * If conversion succeeds partially: ptr will point to the first invalid character it encountered.
     * For example, if you pass "12a34", ptr will point to the character 'a'.
     * If conversion fails completely: ptr will point right back to the beginning of your string (DataPointer.data()).
     *
     * ErrorCode type is std::errc, which is a standard C++ scoped enum used for system error tracking.
     * Can be one of three
     * std::errc{} (Default/Success): The string was parsed into a number without hitting any hardware limitations.
     *  NOTE: A partial conversion is a success, e.g. "12a34" returns "12"
     * std::errc::invalid_argument: The string didn't start with a number at all (e.g., trying to parse "abc" or "").
     * std::errc::result_out_of_range: The string didn't start with a number at all (e.g., trying to parse "abc" or "").
     */
    auto [DataPointer, ErrorCode] = std::from_chars(Month.data(), Month.data() + Month.size(), MonthNumber);

    if (ErrorCode == std::errc{} && DataPointer == (Month.data() + Month.size()) && MonthNumber >= 1 && MonthNumber <= 12)
    {
      RC = std::string(TWO_DIGIT_MONTHS[MonthNumber]);
    }
  }
  // Month is February or Feb
  else
  {
    // Hardcoded struct array mapping keys to month integers
    struct MonthMap
    {
      std::string_view Name;
      int Number;
    };
    static constexpr std::array TextMappings = {
        MonthMap{"jan", 1}, MonthMap{"january", 1},
        MonthMap{"feb", 2}, MonthMap{"february", 2},
        MonthMap{"mar", 3}, MonthMap{"march", 3},
        MonthMap{"apr", 4}, MonthMap{"april", 4},
        MonthMap{"may", 5},
        MonthMap{"jun", 6}, MonthMap{"june", 6},
        MonthMap{"jul", 7}, MonthMap{"july", 7},
        MonthMap{"aug", 8}, MonthMap{"august", 8},
        MonthMap{"sep", 9}, MonthMap{"september", 9},
        MonthMap{"oct", 10}, MonthMap{"october", 10},
        MonthMap{"nov", 11}, MonthMap{"november", 11},
        MonthMap{"dec", 12}, MonthMap{"december", 12}};

    for (const auto& Mapping : TextMappings)
    {
      if (Equals(Month, Mapping.Name) == true)
      {
        RC = std::string(TWO_DIGIT_MONTHS[Mapping.Number]);
      }
    }
  }

  return RC;
}

bool Scraper::Equals(std::string_view a, std::string_view b) const
{
  if (a.size() != b.size())
  {
    return false;
  }

  // Compare character-by-character using std::tolower
  return std::equal(a.begin(), a.end(), b.begin(), [](char ca, char cb)
      { return std::tolower(static_cast<unsigned char>(ca)) ==
               std::tolower(static_cast<unsigned char>(cb)); });
}
