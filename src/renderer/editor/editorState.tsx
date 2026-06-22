import React, { createContext, useContext, useState } from 'react'

interface EditorContextType {
  cursorLine: number
  cursorCol: number
  languageMode: string
  setCursorPos: (line: number, col: number) => void
  setLanguageMode: (mode: string) => void
}

const EditorContext = createContext<EditorContextType | null>(null)

export const useEditorState = () => {
  const context = useContext(EditorContext)
  if (!context) {
    throw new Error('useEditorState must be used within an EditorStateProvider')
  }
  return context
}

export const EditorStateProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [cursorLine, setCursorLine] = useState(1)
  const [cursorCol, setCursorCol] = useState(1)
  const [languageMode, setLanguageMode] = useState('Plain Text')

  const setCursorPos = (line: number, col: number) => {
    setCursorLine(line)
    setCursorCol(col)
  }

  return (
    <EditorContext.Provider
      value={{
        cursorLine,
        cursorCol,
        languageMode,
        setCursorPos,
        setLanguageMode
      }}
    >
      {children}
    </EditorContext.Provider>
  )
}
