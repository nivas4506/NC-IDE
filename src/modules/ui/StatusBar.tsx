import React from 'react'
import { useActiveFileStore } from '../files/activeFile.store'
import { useEditorStore } from '../editor/editor.store'

export const StatusBar: React.FC = () => {
  const { filePath } = useActiveFileStore()
  const { cursorLine, cursorCol, languageMode } = useEditorStore()

  const fileName = filePath ? filePath.split(/[/\\]/).pop() : 'No File'

  return (
    <div className="status-bar">
      <div className="status-left">
        <span className="status-icon">
          <svg viewBox="0 0 24 24" width="12" height="12" fill="none" stroke="currentColor" strokeWidth="2">
            <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z" />
            <polyline points="14 2 14 8 20 8" />
          </svg>
        </span>
        <span className="status-path" title={filePath || 'No File Opened'}>
          {fileName}
        </span>
      </div>
      <div className="status-right">
        <div className="status-elem">
          <span>{languageMode}</span>
        </div>
        <div className="status-elem divider">|</div>
        <div className="status-elem">
          <span>UTF-8</span>
        </div>
        <div className="status-elem divider">|</div>
        <div className="status-elem cursor-pos">
          <span>Ln {cursorLine}, Col {cursorCol}</span>
        </div>
      </div>
    </div>
  )
}
