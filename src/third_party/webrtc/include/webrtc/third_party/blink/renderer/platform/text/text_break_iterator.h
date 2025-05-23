/*
 * Copyright (C) 2006 Lars Knoll <lars@trolltech.com>
 * Copyright (C) 2007, 2011, 2012 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_TEXT_BREAK_ITERATOR_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_TEXT_BREAK_ITERATOR_H_

#include "base/macros.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"
#include "third_party/blink/renderer/platform/wtf/text/character_names.h"
#include "third_party/blink/renderer/platform/wtf/text/unicode.h"

#include <unicode/brkiter.h>

namespace blink {

typedef icu::BreakIterator TextBreakIterator;

// Note: The returned iterator is good only until you get another iterator, with
// the exception of acquireLineBreakIterator.

// This is similar to character break iterator in most cases, but is subject to
// platform UI conventions. One notable example where this can be different
// from character break iterator is Thai prepend characters, see bug 24342.
// Use this for insertion point and selection manipulations.
PLATFORM_EXPORT TextBreakIterator* CursorMovementIterator(const UChar*,
                                                          int length);

PLATFORM_EXPORT TextBreakIterator* WordBreakIterator(const String&,
                                                     int start,
                                                     int length);
PLATFORM_EXPORT TextBreakIterator* WordBreakIterator(const UChar*, int length);
PLATFORM_EXPORT TextBreakIterator* AcquireLineBreakIterator(
    const LChar*,
    int length,
    const AtomicString& locale,
    const UChar* prior_context,
    unsigned prior_context_length);
PLATFORM_EXPORT TextBreakIterator* AcquireLineBreakIterator(
    const UChar*,
    int length,
    const AtomicString& locale,
    const UChar* prior_context,
    unsigned prior_context_length);
PLATFORM_EXPORT void ReleaseLineBreakIterator(TextBreakIterator*);
PLATFORM_EXPORT TextBreakIterator* SentenceBreakIterator(const UChar*,
                                                         int length);

// Before calling this, check if the iterator is not at the end. Otherwise,
// it may not work as expected.
// See https://ssl.icu-project.org/trac/ticket/13447 .
PLATFORM_EXPORT bool IsWordTextBreak(TextBreakIterator*);

const int kTextBreakDone = -1;

enum class LineBreakType {
  kNormal,

  // word-break:break-all allows breaks between letters/numbers, but prohibits
  // break before/after certain punctuation.
  kBreakAll,

  // Allows breaks at every grapheme cluster boundary.
  // Terminal style line breaks described in UAX#14: Examples of Customization
  // http://unicode.org/reports/tr14/#Examples
  // CSS is discussing to add this feature crbug.com/720205
  // Used internally for word-break:break-word.
  kBreakCharacter,

  // word-break:keep-all doesn't allow breaks between all kind of
  // letters/numbers except some south east asians'.
  kKeepAll,
};

// Determines break opportunities around collapsible space characters (space,
// newline, and tabulation characters.)
enum class BreakSpaceType {
  // Break before every collapsible space character.
  // This is a specialized optimization for CSS, where leading/trailing spaces
  // in each line are removed, and thus breaking before spaces can save
  // computing hanging spaces.
  // Callers are expected to handle spaces by themselves. Because a run of
  // spaces can include different types of spaces, break opportunity is given
  // for every space character.
  // Pre-LayoutNG line breaker uses this type.
  kBeforeEverySpace,

  // Break before a run of white space characters.
  // This is for CSS line breaking as in |kBeforeEverySpace|, but when
  // whitespace collapsing is already applied to the target string. In this
  // case, a run of white spaces are preserved spaces. There should not be break
  // opportunities between white spaces.
  // LayoutNG line breaker uses this type.
  kBeforeSpaceRun,
};

PLATFORM_EXPORT std::ostream& operator<<(std::ostream&, LineBreakType);
PLATFORM_EXPORT std::ostream& operator<<(std::ostream&, BreakSpaceType);

class PLATFORM_EXPORT LazyLineBreakIterator final {
  STACK_ALLOCATED();

 public:
  LazyLineBreakIterator()
      : iterator_(nullptr),
        cached_prior_context_(nullptr),
        cached_prior_context_length_(0),
        break_type_(LineBreakType::kNormal) {
    ResetPriorContext();
  }

  LazyLineBreakIterator(String string,
                        const AtomicString& locale = AtomicString(),
                        LineBreakType break_type = LineBreakType::kNormal)
      : string_(string),
        locale_(locale),
        iterator_(nullptr),
        cached_prior_context_(nullptr),
        cached_prior_context_length_(0),
        break_type_(break_type) {
    ResetPriorContext();
  }

  ~LazyLineBreakIterator() {
    if (iterator_)
      ReleaseLineBreakIterator(iterator_);
  }

  const String& GetString() const { return string_; }

  UChar LastCharacter() const {
    static_assert(arraysize(prior_context_) == 2,
                  "TextBreakIterator has unexpected prior context length");
    return prior_context_[1];
  }

  UChar SecondToLastCharacter() const {
    static_assert(arraysize(prior_context_) == 2,
                  "TextBreakIterator has unexpected prior context length");
    return prior_context_[0];
  }

  void SetPriorContext(UChar last, UChar second_to_last) {
    static_assert(arraysize(prior_context_) == 2,
                  "TextBreakIterator has unexpected prior context length");
    prior_context_[0] = second_to_last;
    prior_context_[1] = last;
  }

  void UpdatePriorContext(UChar last) {
    static_assert(arraysize(prior_context_) == 2,
                  "TextBreakIterator has unexpected prior context length");
    prior_context_[0] = prior_context_[1];
    prior_context_[1] = last;
  }

  void ResetPriorContext() {
    static_assert(arraysize(prior_context_) == 2,
                  "TextBreakIterator has unexpected prior context length");
    prior_context_[0] = 0;
    prior_context_[1] = 0;
  }

  unsigned PriorContextLength() const {
    unsigned prior_context_length = 0;
    static_assert(arraysize(prior_context_) == 2,
                  "TextBreakIterator has unexpected prior context length");
    if (prior_context_[1]) {
      ++prior_context_length;
      if (prior_context_[0])
        ++prior_context_length;
    }
    return prior_context_length;
  }

  // Obtain text break iterator, possibly previously cached, where this iterator
  // is (or has been) initialized to use the previously stored string as the
  // primary breaking context and using previously stored prior context if
  // non-empty.
  TextBreakIterator* Get(unsigned prior_context_length) const {
    DCHECK(prior_context_length <= kPriorContextCapacity);
    const UChar* prior_context =
        prior_context_length
            ? &prior_context_[kPriorContextCapacity - prior_context_length]
            : nullptr;
    if (!iterator_) {
      if (string_.Is8Bit())
        iterator_ = AcquireLineBreakIterator(
            string_.Characters8(), string_.length(), locale_, prior_context,
            prior_context_length);
      else
        iterator_ = AcquireLineBreakIterator(
            string_.Characters16(), string_.length(), locale_, prior_context,
            prior_context_length);
      cached_prior_context_ = prior_context;
      cached_prior_context_length_ = prior_context_length;
    } else if (prior_context != cached_prior_context_ ||
               prior_context_length != cached_prior_context_length_) {
      ReleaseIterator();
      return Get(prior_context_length);
    }
    return iterator_;
  }

  void ResetStringAndReleaseIterator(String string,
                                     const AtomicString& locale) {
    string_ = string;
    locale_ = locale;

    ReleaseIterator();
  }

  void SetLocale(const AtomicString& locale) {
    if (locale == locale_)
      return;
    locale_ = locale;
    ReleaseIterator();
  }

  LineBreakType BreakType() const { return break_type_; }
  void SetBreakType(LineBreakType break_type) { break_type_ = break_type; }
  BreakSpaceType BreakSpace() const { return break_space_; }
  void SetBreakSpace(BreakSpaceType break_space) { break_space_ = break_space; }

  inline bool IsBreakable(int pos,
                          int& next_breakable,
                          LineBreakType line_break_type) const {
    if (pos > next_breakable) {
      next_breakable = NextBreakablePosition(pos, line_break_type);
    }
    return pos == next_breakable;
  }

  inline bool IsBreakable(int pos, int& next_breakable) const {
    return IsBreakable(pos, next_breakable, break_type_);
  }

  inline bool IsBreakable(int pos) const {
    int next_breakable = -1;
    return IsBreakable(pos, next_breakable, break_type_);
  }

  // Returns the break opportunity at or after |offset|.
  unsigned NextBreakOpportunity(unsigned offset) const;

  // Returns the break opportunity at or before |offset|.
  unsigned PreviousBreakOpportunity(unsigned offset, unsigned min = 0) const;

  static bool IsBreakableSpace(UChar ch) {
    return ch == kSpaceCharacter || ch == kTabulationCharacter ||
           ch == kNewlineCharacter;
  }

 private:
  void ReleaseIterator() const {
    if (iterator_)
      ReleaseLineBreakIterator(iterator_);
    iterator_ = nullptr;
    cached_prior_context_ = nullptr;
    cached_prior_context_length_ = 0;
  }

  template <typename CharacterType, LineBreakType, BreakSpaceType>
  int NextBreakablePosition(int pos, const CharacterType* str, int len) const;
  template <typename CharacterType, LineBreakType>
  int NextBreakablePosition(int pos, const CharacterType* str, int len) const;
  template <LineBreakType>
  int NextBreakablePosition(int pos, int len) const;
  int NextBreakablePositionBreakCharacter(int pos) const;
  int NextBreakablePosition(int pos, LineBreakType, int len) const;
  int NextBreakablePosition(int pos, LineBreakType) const;

  static const unsigned kPriorContextCapacity = 2;
  String string_;
  AtomicString locale_;
  mutable TextBreakIterator* iterator_;
  UChar prior_context_[kPriorContextCapacity];
  mutable const UChar* cached_prior_context_;
  mutable unsigned cached_prior_context_length_;
  LineBreakType break_type_;
  BreakSpaceType break_space_ = BreakSpaceType::kBeforeEverySpace;
};

// Iterates over "extended grapheme clusters", as defined in UAX #29.
// Note that platform implementations may be less sophisticated - e.g. ICU prior
// to version 4.0 only supports "legacy grapheme clusters".  Use this for
// general text processing, e.g. string truncation.

class PLATFORM_EXPORT NonSharedCharacterBreakIterator final {
  STACK_ALLOCATED();

 public:
  explicit NonSharedCharacterBreakIterator(const String&);
  NonSharedCharacterBreakIterator(const UChar*, unsigned length);
  ~NonSharedCharacterBreakIterator();

  int Next();
  int Current();

  bool IsBreak(int offset) const;
  int Preceding(int offset) const;
  int Following(int offset) const;

  bool operator!() const { return !is8_bit_ && !iterator_; }

 private:
  void CreateIteratorForBuffer(const UChar*, unsigned length);

  unsigned ClusterLengthStartingAt(unsigned offset) const {
    DCHECK(is8_bit_);
    // The only Latin-1 Extended Grapheme Cluster is CR LF
    return IsCRBeforeLF(offset) ? 2 : 1;
  }

  bool IsCRBeforeLF(unsigned offset) const {
    DCHECK(is8_bit_);
    return charaters8_[offset] == '\r' && offset + 1 < length_ &&
           charaters8_[offset + 1] == '\n';
  }

  bool IsLFAfterCR(unsigned offset) const {
    DCHECK(is8_bit_);
    return charaters8_[offset] == '\n' && offset >= 1 &&
           charaters8_[offset - 1] == '\r';
  }

  bool is8_bit_;

  // For 8 bit strings, we implement the iterator ourselves.
  const LChar* charaters8_;
  unsigned offset_;
  unsigned length_;

  // For 16 bit strings, we use a TextBreakIterator.
  TextBreakIterator* iterator_;

  DISALLOW_COPY_AND_ASSIGN(NonSharedCharacterBreakIterator);
};

// Counts the number of grapheme clusters. A surrogate pair or a sequence
// of a non-combining character and following combining characters is
// counted as 1 grapheme cluster.
PLATFORM_EXPORT unsigned NumGraphemeClusters(const String&);

// Returns the number of code units that the next grapheme cluster is made of.
PLATFORM_EXPORT unsigned LengthOfGraphemeCluster(const String&, unsigned = 0);

// Returns a list of graphemes cluster at each character using character break
// rules.
PLATFORM_EXPORT void GraphemesClusterList(const UChar* text,
                                          unsigned text_length,
                                          unsigned start,
                                          unsigned length,
                                          Vector<unsigned>* graphemes);

}  // namespace blink

#endif
