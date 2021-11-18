#ifndef MPAGSCIPHER_PLAYFAIRCIPHER_HPP
#define MPAGSCIPHER_PLAYFAIRCIPHER_HPP

#include "CipherMode.hpp"
#include <map>
#include <string>
#include <algorithm>

class PlayfairCipher {
  public:

    explicit PlayfairCipher(const std::string& key);
    void setKey(const std::string& key);
    std::string applyCipher(const std::string& inputText,
                            const CipherMode cipherMode);

  private:
    std::string key_{""};

    // grid size
    const std::string::size_type gridSize_{5};

    // key length = grid size^2
    const std::string::size_type keyLength_{gridSize_*gridSize_};

    // lookup tables generated from the key

    // type definition for the coordinates in the 5x5 table
    using PlayfairCoords = std::pair<std::size_t, std::size_t>;

    // lookup table to go from the character to coordinates
    std::map<char, PlayfairCoords> charLookup_;

    // lookup table to go from the coordinates to the character
    std::map<PlayfairCoords, char> coordLookup_;

};

#endif
