export const monacoThemeName = 'nc-dark-theme'

export const ncDarkThemeConfig = {
  base: 'vs-dark' as const,
  inherit: true,
  rules: [
    { token: 'comment', foreground: '6272a4', fontStyle: 'italic' },
    { token: 'keyword', foreground: 'ff79c6', fontStyle: 'bold' },
    { token: 'string', foreground: 'f1fa8c' },
    { token: 'number', foreground: 'bd93f9' },
    { token: 'regexp', foreground: 'f1fa8c' },
    { token: 'type', foreground: '8be9fd', fontStyle: 'italic' },
    { token: 'class', foreground: '50fa7b' },
    { token: 'function', foreground: '50fa7b' },
    { token: 'variable', foreground: 'f8f8f2' }
  ],
  colors: {
    'editor.background': '#121212',
    'editor.foreground': '#FFFFFF',
    'editorCursor.foreground': '#1313EC',
    'editor.lineHighlightBackground': '#1E1E1E',
    'editorLineNumber.foreground': '#A0A0A0',
    'editorLineNumber.activeForeground': '#FFFFFF',
    'editor.selectionBackground': '#2A2A2A',
    'editor.inactiveSelectionBackground': '#222222',
    'editor.border': '#2A2A2A',
    'editorGutter.background': '#121212'
  }
}
