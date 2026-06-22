import React, { useState, useEffect } from 'react'
import { FileProvider } from './files/fileManager'
import { EditorStateProvider } from './editor/editorState'
import { TitleBar } from './ui/TitleBar'
import { ToolBar } from './ui/ToolBar'
import { StatusBar } from './ui/StatusBar'
import { Editor } from './editor/Editor'
import './theme/theme.css'

const AppContent: React.FC = () => {
  const [showStatusBar, setShowStatusBar] = useState(true)

  useEffect(() => {
    const unbind = window.api.file.onMenuToggleStatusBar(() => {
      setShowStatusBar((prev) => !prev)
    })
    return () => {
      unbind()
    }
  }, [])

  return (
    <div className="app-container">
      <TitleBar />
      <ToolBar />
      <div className="workspace-area">
        <Editor />
      </div>
      {showStatusBar && <StatusBar />}
    </div>
  )
}

export const App: React.FC = () => {
  return (
    <FileProvider>
      <EditorStateProvider>
        <AppContent />
      </EditorStateProvider>
    </FileProvider>
  )
}
