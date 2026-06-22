import { create } from 'zustand'

export interface EditorState {
  cursorLine: number
  cursorCol: number
  languageMode: string
  fontSize: number
  wordWrap: 'on' | 'off'
  editorInstance: any | null

  // Actions
  setCursorPos: (line: number, col: number) => void
  setLanguageMode: (mode: string) => void
  setFontSize: (size: number) => void
  toggleWordWrap: () => void
  setEditorInstance: (instance: any) => void
  triggerUndo: () => void
  triggerRedo: () => void
}

export const useEditorStore = create<EditorState>((set, get) => ({
  cursorLine: 1,
  cursorCol: 1,
  languageMode: 'Plain Text',
  fontSize: 14,
  wordWrap: 'on',
  editorInstance: null,

  setCursorPos: (line, col) => set({ cursorLine: line, cursorCol: col }),
  setLanguageMode: (mode) => set({ languageMode: mode }),
  setFontSize: (size) => set({ fontSize: size }),
  toggleWordWrap: () => set((state) => ({ wordWrap: state.wordWrap === 'on' ? 'off' : 'on' })),
  setEditorInstance: (instance) => set({ editorInstance: instance }),
  triggerUndo: () => {
    const editor = get().editorInstance
    if (editor) {
      editor.trigger('keyboard', 'undo', null)
    }
  },
  triggerRedo: () => {
    const editor = get().editorInstance
    if (editor) {
      editor.trigger('keyboard', 'redo', null)
    }
  }
}))
