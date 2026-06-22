export interface EditorPosition {
  lineNumber: number
  column: number
}

export interface EditorSelection {
  startLineNumber: number
  startColumn: number
  endLineNumber: number
  endColumn: number
}

export interface EditorConfig {
  fontSize: number
  lineNumbers: 'on' | 'off'
  wordWrap: 'on' | 'off'
}
