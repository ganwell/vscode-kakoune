type lookupData = ((int, string), int);

let makeLookupData = (~lineNumber, ~textLine, ~offset): lookupData => (
  (lineNumber, textLine),
  offset,
);

let rec matchAll = (regex, str) =>
  switch (str |> Js.Re.exec_(regex)) {
  | Some(m) => m |> List.cons(str |> matchAll(regex))
  | None => []
  };

let findAllOccurrances = (regex, textLinesWithOffset: list(lookupData)) =>
  textLinesWithOffset
  |> List.map(~f=(((i, l), o)) => (i, o, l |> matchAll(regex)))
  |> List.map(~f=((lineIndex, o, matches)) =>
       matches |> List.map(~f=r => (lineIndex, o, r))
     )
  |> List.flatten
  |> List.map(~f=((line, characterOffset, result)) => {
       let anchorCharacter = (result |> Js.Re.index) + characterOffset;
       (
         Vscode.Position.make(~character=anchorCharacter, ~line),
         Vscode.Position.make(
           ~character=
             (
               (result |> Js.Re.captures)[0]
               |> Js.Nullable.toOption
               |> Option.unwrap(~default="")
               |> String.length
             )
             + anchorCharacter,
           ~line,
         ),
       );
     })
  |> List.map(~f=((anchor, active)) =>
       Vscode.Selection.make(~anchor, ~active)
     );

let searchAll = (editor: Vscode.TextEditor.t) => {
  let searchBox = Vscode.Window.createInputBox();

  Vscode.InputBox.onDidAccept(searchBox, () =>
    searchBox |> Vscode.InputBox.dispose
  );

  Vscode.InputBox.onDidChangeValue(searchBox, value =>
    editor.document
    |> Vscode.TextDocument.getAllTextLines
    |> List.map(~f=((lineNumber, textLine)) =>
         makeLookupData(~lineNumber, ~textLine, ~offset=0)
       )
    |> findAllOccurrances(value |> Js.Re.fromStringWithFlags(~flags="g"))
    |> Array.fromList
    |> Vscode.TextEditor.setSelections(editor)
  );
  searchBox |> Vscode.InputBox.show;
};

let searchSelections = (~editor: Vscode.TextEditor.t, originalSelections) => {
  let searchBox = Vscode.Window.createInputBox();

  Vscode.InputBox.onDidAccept(searchBox, () =>
    searchBox |> Vscode.InputBox.dispose
  );

  Vscode.InputBox.onDidChangeValue(searchBox, value =>
    originalSelections
    |> Array.map(~f=(s: Vscode.Selection.t) =>
         makeLookupData(
           ~lineNumber=s.start.line,
           ~textLine=
             editor.document
             |> Vscode.TextDocument.getTextInRange(
                  _,
                  s |> Vscode.Range.fromSelection,
                ),
           ~offset=s.start.character,
         )
       )
    |> List.fromArray
    |> findAllOccurrances(value |> Js.Re.fromStringWithFlags(~flags="g"))
    |> Array.fromList
    |> Vscode.TextEditor.setSelections(editor)
  );

  searchBox |> Vscode.InputBox.show;
};
