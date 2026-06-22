import React, { createContext, useContext, useState, useEffect, useRef } from 'react'

export interface FileState {
  filePath: string | null
  content: string
  savedContent: string
  isDirty: boolean
}

interface FileContextType {
  fileState: FileState
  newFile: () => void
  openFile: () => Promise<void>
  saveFile: () => Promise<void>
  saveFileAs: () => Promise<void>
  updateContent: (content: string) => void
}

const FileContext = createContext<FileContextType | null>(null)

export const useFileManager = () => {
  const context = useContext(FileContext)
  if (!context) {
    throw new Error('useFileManager must be used within a FileProvider')
  }
  return context
}

// Global API type declaration for TS
declare global {
  interface Window {
    api: {
      file: {
        open: () => Promise<{ filePath: string; content: string } | null>
        save: (filePath: string, content: string) => Promise<boolean>
        saveAs: (content: string) => Promise<string | null>
        onMenuNew: (callback: () => void) => () => void
        onMenuOpen: (callback: () => void) => () => void
        onMenuSave: (callback: () => void) => () => void
        onMenuSaveAs: (callback: () => void) => () => void
        onMenuToggleStatusBar: (callback: () => void) => () => void
      }
    }
  }
}

export const FileProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [fileState, setFileState] = useState<FileState>({
    filePath: null,
    content: '',
    savedContent: '',
    isDirty: false
  })

  // We use refs to avoid re-binding Electron IPC listeners when content changes
  const stateRef = useRef(fileState)
  useEffect(() => {
    stateRef.current = fileState
  }, [fileState])

  const newFile = () => {
    setFileState({
      filePath: null,
      content: '',
      savedContent: '',
      isDirty: false
    })
  }

  const openFile = async () => {
    try {
      const result = await window.api.file.open()
      if (result) {
        setFileState({
          filePath: result.filePath,
          content: result.content,
          savedContent: result.content,
          isDirty: false
        })
      }
    } catch (err) {
      console.error('Failed to open file:', err)
    }
  }

  const saveFile = async () => {
    const { filePath, content } = stateRef.current
    if (filePath) {
      try {
        const success = await window.api.file.save(filePath, content)
        if (success) {
          setFileState((prev) => ({
            ...prev,
            savedContent: content,
            isDirty: false
          }))
        }
      } catch (err) {
        console.error('Failed to save file:', err)
      }
    } else {
      await saveFileAs()
    }
  }

  const saveFileAs = async () => {
    const { content } = stateRef.current
    try {
      const newPath = await window.api.file.saveAs(content)
      if (newPath) {
        setFileState({
          filePath: newPath,
          content,
          savedContent: content,
          isDirty: false
        })
      }
    } catch (err) {
      console.error('Failed to save file as:', err)
    }
  }

  const updateContent = (newContent: string) => {
    setFileState((prev) => ({
      ...prev,
      content: newContent,
      isDirty: newContent !== prev.savedContent
    }))
  }

  // Bind native menu events
  useEffect(() => {
    const unbindNew = window.api.file.onMenuNew(() => {
      newFile()
    })
    const unbindOpen = window.api.file.onMenuOpen(() => {
      openFile()
    })
    const unbindSave = window.api.file.onMenuSave(() => {
      saveFile()
    })
    const unbindSaveAs = window.api.file.onMenuSaveAs(() => {
      saveFileAs()
    })

    return () => {
      unbindNew()
      unbindOpen()
      unbindSave()
      unbindSaveAs()
    }
  }, [])

  return (
    <FileContext.Provider value={{ fileState, newFile, openFile, saveFile, saveFileAs, updateContent }}>
      {children}
    </FileContext.Provider>
  )
}
