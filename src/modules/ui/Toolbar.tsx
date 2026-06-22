import React from 'react'
import { useActiveFileStore } from '../files/activeFile.store'
import { useEditorStore } from '../editor/editor.store'
import { fileService } from '../files/fileService'

export const Toolbar: React.FC = () => {
  const { isDirty } = useActiveFileStore()
  const { editorInstance, triggerUndo, triggerRedo } = useEditorStore()

  const isUndoRedoDisabled = !editorInstance

  return (
    <div className="tool-bar">
      <button className="tool-btn" onClick={fileService.newFile} title="New File (Ctrl+N)">
        <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
          <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z" />
          <polyline points="14 2 14 8 20 8" />
          <line x1="12" y1="18" x2="12" y2="12" />
          <line x1="9" y1="15" x2="15" y2="15" />
        </svg>
        <span>New</span>
      </button>

      <button className="tool-btn" onClick={fileService.open} title="Open File (Ctrl+O)">
        <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
          <path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z" />
        </svg>
        <span>Open</span>
      </button>

      <button
        className={`tool-btn ${isDirty ? 'accented' : 'disabled'}`}
        onClick={isDirty ? fileService.save : undefined}
        disabled={!isDirty}
        title={isDirty ? "Save File (Ctrl+S)" : "No unsaved changes"}
      >
        <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
          <path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z" />
          <polyline points="17 21 17 13 7 13 7 21" />
          <polyline points="7 3 7 8 15 8" />
        </svg>
        <span>Save</span>
      </button>

      <button
        className={`tool-btn ${isUndoRedoDisabled ? 'disabled' : ''}`}
        onClick={triggerUndo}
        disabled={isUndoRedoDisabled}
        title="Undo (Ctrl+Z)"
      >
        <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
          <polyline points="9 14 4 9 9 4" />
          <path d="M20 20v-7a4 4 0 0 0-4-4H4" />
        </svg>
        <span>Undo</span>
      </button>

      <button
        className={`tool-btn ${isUndoRedoDisabled ? 'disabled' : ''}`}
        onClick={triggerRedo}
        disabled={isUndoRedoDisabled}
        title="Redo (Ctrl+Y)"
      >
        <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
          <polyline points="15 14 20 9 15 4" />
          <path d="M4 20v-7a4 4 0 0 1 4-4h12" />
        </svg>
        <span>Redo</span>
      </button>
    </div>
  )
}
