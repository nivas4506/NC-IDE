import React, { useEffect } from 'react'
import { useActiveFileStore } from '../files/activeFile.store'

export const MenuBar: React.FC = () => {
  const { filePath, isDirty } = useActiveFileStore()

  const fileName = filePath ? filePath.split(/[/\\]/).pop() : 'Untitled'
  const displayPath = filePath || 'New File'

  // Update window title dynamically
  const titlePrefix = isDirty ? '● ' : ''
  const documentTitle = `${titlePrefix}${fileName} — NC IDE`

  useEffect(() => {
    document.title = documentTitle
  }, [documentTitle])

  return (
    <div className="title-bar">
      <div className="title-bar-brand">
        <div className="brand-logo">N</div>
        <span className="brand-text">NC IDE</span>
      </div>
      <div className="title-bar-file">
        <span className="file-name">{fileName}</span>
        {isDirty && <div className="dirty-dot" title="Unsaved changes" />}
        <span className="file-path-separator">—</span>
        <span className="file-path">{displayPath}</span>
      </div>
      <div className="title-bar-actions">
        <span className="badge">v0.1.0</span>
      </div>
    </div>
  )
}
