import React, { useEffect, useRef } from 'react'
import { EditorState, Compartment } from '@codemirror/state'
import { EditorView, keymap, drawSelection } from '@codemirror/view'
import { basicSetup } from 'codemirror'
import { javascript } from '@codemirror/lang-javascript'
import { defaultKeymap, history, historyKeymap } from '@codemirror/commands'
import { useFileManager } from '../files/fileManager'
import { useEditorState } from './editorState'

const languageCompartment = new Compartment()

const customDarkTheme = EditorView.theme({
  "&": {
    color: "var(--nc-text)",
    backgroundColor: "var(--nc-bg)",
    fontFamily: "var(--nc-font-mono)",
    fontSize: "14px",
    height: "100%"
  },
  ".cm-content": {
    caretColor: "var(--nc-text)",
    padding: "10px 0"
  },
  "&.cm-focused": {
    outline: "none"
  },
  "&.cm-focused .cm-cursor": {
    borderLeftColor: "var(--nc-text)",
    borderLeftWidth: "2px"
  },
  "&.cm-focused .cm-selectionBackground, .cm-selectionBackground, ::selection": {
    backgroundColor: "#2B2D3A"
  },
  ".cm-gutters": {
    backgroundColor: "var(--nc-bg)",
    color: "#5C6370",
    borderRight: "1px solid var(--nc-border)",
    paddingLeft: "10px"
  },
  ".cm-activeLineGutter": {
    backgroundColor: "var(--nc-surface)",
    color: "var(--nc-text)"
  },
  ".cm-activeLine": {
    backgroundColor: "var(--nc-surface)"
  }
}, { dark: true })

export const Editor: React.FC = () => {
  const containerRef = useRef<HTMLDivElement>(null)
  const viewRef = useRef<EditorView | null>(null)
  const { fileState, updateContent } = useFileManager()
  const { setCursorPos, setLanguageMode } = useEditorState()

  // Track file content changes from outside (e.g. Open file)
  const contentRef = useRef(fileState.content)
  useEffect(() => {
    contentRef.current = fileState.content
  }, [fileState.content])

  // Initialize CodeMirror instance
  useEffect(() => {
    if (!containerRef.current) return

    const updateListener = EditorView.updateListener.of((update) => {
      if (update.docChanged) {
        const docText = update.state.doc.toString()
        if (docText !== contentRef.current) {
          updateContent(docText)
        }
      }

      if (update.selectionSet || update.docChanged) {
        const head = update.state.selection.main.head
        const line = update.state.doc.lineAt(head)
        const col = head - line.from + 1
        setCursorPos(line.number, col)
      }
    })

    const ext = fileState.filePath ? fileState.filePath.split('.').pop()?.toLowerCase() : null
    let langExt = []
    let modeName = 'Plain Text'
    if (ext === 'js' || ext === 'jsx') {
      langExt = javascript()
      modeName = 'JavaScript'
    } else if (ext === 'ts' || ext === 'tsx') {
      langExt = javascript({ typescript: true })
      modeName = 'TypeScript'
    } else if (ext === 'json') {
      langExt = javascript({ json: true })
      modeName = 'JSON'
    }
    setLanguageMode(modeName)

    const state = EditorState.create({
      doc: fileState.content,
      extensions: [
        basicSetup,
        history(),
        drawSelection(),
        keymap.of([...defaultKeymap, ...historyKeymap]),
        languageCompartment.of(langExt),
        customDarkTheme,
        updateListener
      ]
    })

    const view = new EditorView({
      state,
      parent: containerRef.current
    })

    viewRef.current = view

    return () => {
      view.destroy()
    }
  }, [])

  // Sync editor content and language when fileState changes from outside (like loading a file or clearing it)
  useEffect(() => {
    const view = viewRef.current
    if (!view) return

    // If document content is different, replace it in the editor
    if (view.state.doc.toString() !== fileState.content) {
      view.dispatch({
        changes: { from: 0, to: view.state.doc.length, insert: fileState.content }
      })
    }

    // Update dynamic language extension
    const ext = fileState.filePath ? fileState.filePath.split('.').pop()?.toLowerCase() : null
    let langExt = []
    let modeName = 'Plain Text'
    if (ext === 'js' || ext === 'jsx') {
      langExt = javascript()
      modeName = 'JavaScript'
    } else if (ext === 'ts' || ext === 'tsx') {
      langExt = javascript({ typescript: true })
      modeName = 'TypeScript'
    } else if (ext === 'json') {
      langExt = javascript({ json: true })
      modeName = 'JSON'
    }
    setLanguageMode(modeName)

    view.dispatch({
      effects: languageCompartment.reconfigure(langExt)
    })
  }, [fileState.filePath, fileState.savedContent])

  return <div ref={containerRef} style={{ height: '100%', width: '100%', outline: 'none' }} />
}
