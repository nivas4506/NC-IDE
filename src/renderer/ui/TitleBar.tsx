import React from 'react'
import { useFileManager } from '../files/fileManager'

export const TitleBar: React.FC = () => {
  const { fileState } = useFileManager()
  const { filePath, isDirty } = fileState

  const fileName = filePath ? filePath.split(/[/\\]/).pop() : 'Untitled'
  const displayPath = filePath || 'New File'

  return (
    <div className="title-bar">
      <div className="title-bar-brand">
        <div className="brand-logo">N</div>
        <span className="brand-text">NC IDE</span>
      </div>
      <div className="title-bar-file">
        <span className="file-name">{fileName}</span>
        {isDirty && <span className="dirty-dot" title="Unsaved changes" />}
        <span className="file-path-separator">—</span>
        <span className="file-path">{displayPath}</span>
      </div>
      <div className="title-bar-actions">
        <span className="badge">Prototype v0.1</span>
      </div>
    </div>
  )
}
