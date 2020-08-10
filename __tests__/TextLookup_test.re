open Jest;

describe("Text lookup functions", () => {
  open Expect;

  describe("characterTypeAfter", () => {
    let testInput = [
      (("hello world!", 0), Ok(CharacterType.Character)),
      (("hello world!", 5), Ok(CharacterType.Whitespace)),
      (("hello world!", 11), Ok(CharacterType.Punctuation)),
      (("hello world!", 12), Ok(CharacterType.Whitespace)),
      (("hello world!", (-1)), Error(TextLookup.LookupError.Underflow)),
      (("hello world!", 13), Error(Overflow)),
    ];

    testAll(
      "characterTypeAfter", testInput, (((text, startIndex), expected)) =>
      text
      |> TextLookup.characterTypeAfter(~startIndex)
      |> expect
      |> toEqual(expected)
    );
  });

  describe("findNextNotOfType", () => {
    let testInput = [
      (("hello world!", 0, CharacterType.Character), Ok(5)),
      (("hello world!", 5, CharacterType.Whitespace), Ok(6)),
      (("hello world!", 11, CharacterType.Punctuation), Ok(12)),
      (
        ("hello world!", 12, CharacterType.Whitespace),
        Error(TextLookup.LookupError.Overflow),
      ),
      (("hello world!", (-1), CharacterType.Punctuation), Error(Underflow)),
    ];

    testAll(
      "findNextNotOfType",
      testInput,
      (((text, startIndex, characterType), expected)) =>
      text
      |> TextLookup.findNextNotOfType(~characterType, ~startIndex)
      |> expect
      |> toEqual(expected)
    );
  });

  describe("findPreviousNotOfType", () => {
    let testInput = [
      (("hello world!", 5, CharacterType.Whitespace), Ok(5)),
      (("hello world!", 6, CharacterType.Whitespace), Ok(5)),
      (("hello world!", 10, CharacterType.Character), Ok(6)),
      (("hello world!", 11, CharacterType.Punctuation), Ok(11)),
      (("hello world!", 12, CharacterType.Punctuation), Ok(11)),
      (("hello world!", 12, CharacterType.Punctuation), Ok(11)),
      (("hello world!", 12, CharacterType.Whitespace), Ok(12)),
      (
        ("hello world!", 0, CharacterType.Character),
        Error(TextLookup.LookupError.Underflow),
      ),
      (("hello world!", 13, CharacterType.Whitespace), Error(Overflow)),
    ];

    testAll(
      "findPreviousNotOfType",
      testInput,
      (((text, startIndex, characterType), expected)) =>
      text
      |> TextLookup.findPreviousNotOfType(~characterType, ~startIndex)
      |> expect
      |> toEqual(expected)
    );
  });

  describe("findNextWordStart", () => {
    let testInput = [
      (("hello world!", 0), Ok(6)),
      (("hello world!", 7), Ok(11)),
      (("#include <iostream>", 17), Ok(18)),
      (("\treturn 0;", 8), Ok(9)),
      (("    return 0;", 11), Ok(12)),
      (("hello world!", 11), Error(TextLookup.LookupError.Overflow)),
      (("hello world!", 12), Error(Overflow)),
      (("hello world!", (-1)), Error(Underflow)),
    ];

    testAll(
      "findNextWordStart", testInput, (((text, startIndex), expected)) =>
      text
      |> TextLookup.findNextWordStart(~startIndex)
      |> expect
      |> toEqual(expected)
    );
  });

  describe("findNextWordEnd", () => {
    let testInput = [
      (("hello world!", 0), Ok(11)),
      (("hello world!", 7), Ok(12)),
      (("hello world!", 11), Error(TextLookup.LookupError.Overflow)),
      (("hello world!", 12), Error(Overflow)),
      (("hello world!", (-1)), Error(Underflow)),
    ];

    testAll("findNextWordEnd", testInput, (((text, startIndex), expected)) =>
      text
      |> TextLookup.findNextWordEnd(~startIndex)
      |> expect
      |> toEqual(expected)
    );
  });

  describe("findPreviousWordStart", () => {
    let testInput = [
      (("hello world!", 9), Ok(0)),
      (("hello world!", 11), Ok(0)),
      (("hello world!", 12), Ok(6)),
      (("hello world!", 2), Error(TextLookup.LookupError.Underflow)),
      (("hello world!", 0), Error(Underflow)),
      (("hello world!", 13), Error(Overflow)),
    ];

    testAll(
      "findPreviousWordStart", testInput, (((text, startIndex), expected)) =>
      text
      |> TextLookup.findPreviousWordStart(~startIndex)
      |> expect
      |> toEqual(expected)
    );
  });

  describe("findPreviousWordEnd", () => {
    let testInput = [
      (("hello world!", 9), Ok(5)),
      (("hello world!", 11), Ok(5)),
      (("hello world!", 12), Ok(11)),
      (("hello world!", 2), Ok(0)),
      (("hello world!", 0), Ok(0)),
      (("hello world!", 13), Error(TextLookup.LookupError.Overflow)),
    ];

    testAll(
      "findPreviousWordEnd", testInput, (((text, startIndex), expected)) =>
      text
      |> TextLookup.findPreviousWordEnd(~startIndex)
      |> expect
      |> toEqual(expected)
    );
  });

  describe("findWordEnd", () => {
    let testInput = [
      (("hello world!", 0), Ok(5)),
      (("hello world!", 7), Ok(11)),
      (("hello", 2), Ok(5)),
      (("#include <iostream>", 17), Ok(18)),
      (("#include <iostream>", 18), Ok(19)),
      (("#include <iostream>", 19), Ok(19)),
      (("#include <iostream>", 20), Ok(19)),
      (
        ("#include <iostream>", (-1)),
        Error(TextLookup.LookupError.Underflow),
      ),
    ];

    testAll("findWordEnd", testInput, (((text, startIndex), expected)) => {
      text
      |> TextLookup.findWordEnd(~startIndex)
      |> expect
      |> toEqual(expected)
    });
  });

  describe("findWordStart", () => {
    let testInput = [
      (("hello world!", 0), Ok(0)),
      (("hello world!", 2), Ok(0)),
      (("hello world!", 6), Ok(5)),
      (("hello world!", 8), Ok(6)),
      (("hello world!", 11), Ok(6)),
      (("hello world!", 12), Ok(11)),
      (("hello world!", (-1)), Ok(0)),
      (("hello world!", 13), Error(TextLookup.LookupError.Overflow)),
      (("hello", 2), Ok(0)),
      (("hello", 0), Ok(0)),
      (("hello", 5), Ok(0)),
      (("hello", (-1)), Ok(0)),
      (("hello", 6), Error(Overflow)),
      (("#include <iostream>", 1), Ok(0)),
      (("#include <iostream>", 8), Ok(1)),
      (("#include <iostream>", 9), Ok(8)),
      (("#include <iostream>", 10), Ok(9)),
      (("#include <iostream>", 17), Ok(10)),
      (("#include <iostream>", 18), Ok(10)),
      (("#include <iostream>", 19), Ok(18)),
      (("#include <iostream>", (-1)), Ok(0)),
      (("#include <iostream>", 20), Error(Overflow)),
    ];

    testAll("findWordStart", testInput, (((text, startIndex), expected)) =>
      text
      |> TextLookup.findWordStart(~startIndex)
      |> expect
      |> toEqual(expected)
    );
  });
});
