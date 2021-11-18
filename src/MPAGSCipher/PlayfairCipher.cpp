#include "PlayfairCipher.hpp"


#include <string>
#include <iostream>

PlayfairCipher::PlayfairCipher(const std::string& key)
{
  setKey(key);
}

void PlayfairCipher::setKey(const std::string& key)
{

  // store original key
  key_ = key;

  // Append the alphabet
  key_ += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  // Make sure the key is upper case
  std::transform(std::begin(key_), std::end(key_), std::begin(key_), ::toupper);

  // Remove non-alpha characters
  key_.erase(std::remove_if(std::begin(key_), std::end(key_), [](char c){return !std::isalpha(c); }  ), std::end(key_));
  // Change J -> I
  std::transform(std::begin(key_), std::end(key_), std::begin(key_), [](char c) {return (c == 'J') ? 'I' : c; });
  // Remove duplicated letters
  std::string lettersFound("");
  auto detectDuplicates = [&] (char c)
  {
    if(lettersFound.find(c) == std::string::npos)
    {
      lettersFound += c;
      return false;
    }
    else
    {
      return true;
    }
  };

  key_.erase(std::remove_if(std::begin(key_), std::end(key_), detectDuplicates), std::end(key_));
  // Store the coords of each letter
  // Store the playfair cihper key map
  for (std::size_t i{0}; i < keyLength_; ++i)
  {
    std::size_t row{i/gridSize_}; //size_t -> discards remainder
    std::size_t column{i % gridSize_};

    PlayfairCoords coords = std::make_pair(row,column);

    charLookup_[key_[i]] = coords;
    coordLookup_[coords] = key_[i];

  }




}

std::string PlayfairCipher::applyCipher(const std::string& inputText, const CipherMode cipherMode)
{
  //create output string, initally a copy of input text
  std::string outputText(inputText);

  // Change J → I (performed in PlayfairCipher::setKey)

  // If repeated chars in a digraph add an X or Q if XX
  if(cipherMode == CipherMode::Encrypt)
  {
    std::string cipherText{""};
    for (std::size_t i{0}; i < inputText.size(); ++i)
    {
      // check if digraph is same
      if(inputText[i] == inputText[i+1])
      {
        if(inputText[i] == 'X')
        {
          // add 'Q' if 'XX'
          cipherText += inputText[i];
          cipherText += 'Q';
        }
        else
        {
          // add 'X' for any other duplicate letters
          cipherText += inputText[i] ;
          cipherText +='X';
        }
      }
      else // don't add anything new
      {
        cipherText += inputText[i];
      }
    }

    // if the size of input is odd, add a trailing Z
    if(inputText.size() % 2 != 0) //odd
    {
      cipherText += 'Z';
    }



    std::string finalText{""};
    // Loop over the input in Digraphs
    for (std::size_t i{0}; i < cipherText.size(); i+=2)
    {
      //   - Find the coords in the grid for each digraph
      PlayfairCoords coord1{ charLookup_[cipherText[i]]};
      PlayfairCoords coord2{ charLookup_[cipherText[i+1]]};

      // define new coordinates
      PlayfairCoords newcoord1;
      PlayfairCoords newcoord2;

      //   - Apply the rules to these coords to get 'new' coords

      // 1. If letters are on same row, replace with letter to the right
      if (coord1.first == coord2.first)
      {
        if(coord1.second + 1 >= gridSize_) // need to wrap to next row (letter 1 on edge)
        {
          newcoord1 = std::make_pair(coord1.first + 1, 0);
          newcoord2 = std::make_pair(coord2.first, coord2.second + 1);
        }
        else if(coord2.second + 1 >= gridSize_) // need to wrap to next row (letter 2 on edge)
        {
          newcoord1 = std::make_pair(coord1.first, coord1.second + 1);
          newcoord2 = std::make_pair(coord2.first + 1, 0);
        }
        else
        {
          newcoord1 = std::make_pair(coord1.first , coord1.second + 1);
          newcoord2 = std::make_pair(coord2.first, coord2.second + 1);
        }

      }

      // 2. If in same column, replace with letters directly below
      if (coord1.second == coord2.second)
      {
        if(coord1.first + 1 >= gridSize_) // need to wrap to top (letter 1 on edge)
        {
          newcoord1 = std::make_pair(0, coord1.second);
          newcoord2 = std::make_pair(coord2.first + 1, coord2.second);
        }
        else if(coord2.first + 1 >= gridSize_) // need to wrap to top (letter 2 on edge)
        {
          newcoord1 = std::make_pair(coord1.first + 1, coord1.second);
          newcoord2 = std::make_pair(0, coord2.second);
        }
        else
        {
          newcoord1 = std::make_pair(coord1.first , coord1.second + 1);
          newcoord2 = std::make_pair(coord2.first, coord2.second + 1);
        }

      }

      // 3. If they form a rectangle, replace with ones from corner on the same row

      // this condition ensures that there is a rectangle
      if ( (abs(coord1.first - coord2.first) == 1)  && (abs(coord1.second - coord2.second) == gridSize_ -1)   )
      {
        // consider:
        // case where letter 1 is upper corner (letter 2 is lower corner) of rectangle
        if(coord1.first < coord2.first)
        {
          newcoord1 = std::make_pair(coord1.first + 1, coord1.second);
          newcoord2 = std::make_pair(coord2.first - 1, coord2.second);
        }
        else   // case where letter 2 is upper corner (letter 1 is lower corner) of rectangle
        {
          newcoord1 = std::make_pair(coord1.first - 1, coord1.second);
          newcoord2 = std::make_pair(coord2.first + 1, coord2.second);
        }

      }

      //   - Find the letter associated with the new coords
      char letter1{ coordLookup_[newcoord1]};
      char letter2{ coordLookup_[newcoord2]};

      finalText += letter1;
      finalText += letter2;
    }

    // return the text
    return finalText;
  }
  else
  {

    std::string decryptText{""};
    for (std::size_t i{0}; i < outputText.size(); i+=2)
    {
      //   - Find the coords in the grid for each digraph
      PlayfairCoords coord1{ charLookup_[outputText[i]]};
      PlayfairCoords coord2{ charLookup_[outputText[i+1]]};

      // define new coordinates
      PlayfairCoords newcoord1;
      PlayfairCoords newcoord2;

      //   - Apply the rules to these coords to get 'new' coords

      // 3. If they form a rectangle, replace with ones from corner on the same row

      // this condition ensures that there is a rectangle
      if ( (abs(coord1.first - coord2.first) == 1)  && (abs(coord1.second - coord2.second) == gridSize_ -1)   )
      {
        // consider:
        // case where letter 1 is upper corner (letter 2 is lower corner) of rectangle
        if(coord1.first < coord2.first)
        {
          newcoord1 = std::make_pair(coord1.first - 1, coord1.second);
          newcoord2 = std::make_pair(coord2.first + 1, coord2.second);
        }
        else   // case where letter 2 is upper corner (letter 1 is lower corner) of rectangle
        {
          newcoord1 = std::make_pair(coord1.first + 1, coord1.second);
          newcoord2 = std::make_pair(coord2.first - 1, coord2.second);
        }

      }

      // 2. If in same column, replace with letters directly below
      if (coord1.second == coord2.second)
      {
        if(coord1.first  == 0) // need to wrap to bottom (letter 1 on edge)
        {
          newcoord1 = std::make_pair(gridSize_ - 1, coord1.second);
          newcoord2 = std::make_pair(coord2.first - 1, coord2.second);
        }
        else if(coord2.first == 0) // need to wrap to bottom (letter 2 on edge)
        {
          newcoord1 = std::make_pair(coord1.first - 1, coord1.second);
          newcoord2 = std::make_pair(gridSize_ - 1, coord2.second);
        }
        else
        {
          newcoord1 = std::make_pair(coord1.first , coord1.second - 1);
          newcoord2 = std::make_pair(coord2.first, coord2.second - 1);
        }

      }


      // 1. If letters are on same row, replace with letter to the right
      if (coord1.first == coord2.first)
      {
        if(coord1.second == 0 ) // need to wrap to next row (letter 1 on edge)
        {
          newcoord1 = std::make_pair(coord1.first - 1, gridSize_ - 1);
          newcoord2 = std::make_pair(coord2.first, coord2.second - 1);
        }
        else if(coord2.second == 0) // need to wrap to next row (letter 2 on edge)
        {
          newcoord1 = std::make_pair(coord1.first, coord1.second - 1);
          newcoord2 = std::make_pair(coord2.first - 1, gridSize_ - 1);
        }
        else
        {
          newcoord1 = std::make_pair(coord1.first , coord1.second - 1);
          newcoord2 = std::make_pair(coord2.first, coord2.second - 1);
        }

      }


      //   - Find the letter associated with the new coords
      char letter1{ coordLookup_[newcoord1]};
      char letter2{ coordLookup_[newcoord2]};

      decryptText += letter1;
      decryptText += letter2;
    }

    // remove trailing Z
    if(decryptText.back() == 'Z')
    {
      decryptText.pop_back();
    }

    std::string finalText{""};

    for (std::size_t i{0}; i < decryptText.size()-1; ++i)
    {
      // check duplicates
      if(decryptText[i] ==  decryptText[i+2] && (decryptText[i+1] == 'Q' || decryptText[i+1] == 'X'))
      {
        finalText += decryptText[i];
        finalText += decryptText[i];
      }
      else
      {
        finalText +=  decryptText[i];
      }

    }
  // return the text
  return finalText;

  }




}
