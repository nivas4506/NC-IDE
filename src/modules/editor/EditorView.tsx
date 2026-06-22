import React, { useEffect } from 'react'
import MonacoEditor from '@monaco-editor/react'
import { useActiveFileStore } from '../files/activeFile.store'
import { useEditorStore } from './editor.store'
import { ncDarkThemeConfig, monacoThemeName } from '../theme/monacoTheme'

export const EditorView: React.FC = () => {
  const { filePath, content, setContent } = useActiveFileStore()
  const { setCursorPos, setLanguageMode, fontSize, wordWrap, setEditorInstance } = useEditorStore()

  const language = detectLanguage(filePath)

  useEffect(() => {
    setLanguageMode(getLanguageLabel(language))
  }, [language, setLanguageMode])

  const handleEditorMount = (editorInstance: any) => {
    setEditorInstance(editorInstance)
    // Register cursor change listener
    editorInstance.onDidChangeCursorPosition((e: any) => {
      setCursorPos(e.position.lineNumber, e.position.column)
    })

    // Focus editor immediately
    editorInstance.focus()
  }

  const handleBeforeMount = (monaco: any) => {
    // Register custom dark theme
    monaco.editor.defineTheme(monacoThemeName, ncDarkThemeConfig)
  }

  return (
    <div style={{ height: '100%', width: '100%', overflow: 'hidden' }}>
      <MonacoEditor
        height="100%"
        width="100%"
        language={language}
        theme={monacoThemeName}
        value={content}
        onChange={(val) => setContent(val || '')}
        onMount={handleEditorMount}
        beforeMount={handleBeforeMount}
        options={{
          fontSize,
          wordWrap,
          fontFamily: 'var(--font-mono)',
          lineNumbers: 'on',
          minimap: { enabled: false },
          automaticLayout: true,
          scrollbar: {
            verticalScrollbarSize: 8,
            horizontalScrollbarSize: 8
          },
          cursorBlinking: 'smooth',
          cursorSmoothCaretAnimation: 'on',
          padding: { top: 12, bottom: 12 }
        }}
      />
    </div>
  )
}

function detectLanguage(filePath: string | null): string {
  if (!filePath) return 'plaintext'
  const ext = filePath.split('.').pop()?.toLowerCase()
  switch (ext) {
    case 'js':
    case 'jsx':
      return 'javascript'
    case 'ts':
    case 'tsx':
      return 'typescript'
    case 'json':
      return 'json'
    case 'py':
      return 'python'
    case 'html':
      return 'html'
    case 'css':
      return 'css'
    case 'md':
      return 'markdown'
    default:
      return 'plaintext'
  }
}

function getLanguageLabel(langId: string): string {
  switch (langId) {
    case 'javascript': return 'JavaScript'
    case 'typescript': return 'TypeScript'
    case 'json': return 'JSON'
    case 'python': return 'Python'
    case 'html': return 'HTML'
    case 'css': return 'CSS'
    case 'markdown': return 'Markdown'
    default: return 'Plain Text'
  }
}
