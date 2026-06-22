import React from 'react'
import { MenuBar } from './MenuBar'
import { Toolbar } from './Toolbar'
import { EditorView } from '../editor/EditorView'
import { StatusBar } from './StatusBar'

export const Workspace: React.FC = () => {
  return (
    <div className="app-container">
      <MenuBar />
      <Toolbar />
      <div className="workspace-area">
        <EditorView />
      </div>
      <StatusBar />
    </div>
  )
}
