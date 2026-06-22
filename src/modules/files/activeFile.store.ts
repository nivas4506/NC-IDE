import { create } from 'zustand'

export interface ActiveFileState {
  filePath: string | null
  content: string
  savedContent: string
  isDirty: boolean
  recentFiles: string[]

  // Actions
  setFilePath: (path: string | null) => void
  setContent: (content: string) => void
  setSavedContent: (content: string) => void
  setDirty: (isDirty: boolean) => void
  resetFile: () => void
  openFile: (path: string, content: string) => void
  saveFile: (path: string, content: string) => void
  addRecentFile: (path: string) => void
}

export const useActiveFileStore = create<ActiveFileState>((set) => ({
  filePath: null,
  content: '',
  savedContent: '',
  isDirty: false,
  recentFiles: [],

  setFilePath: (path) => set({ filePath: path }),
  setContent: (content) => set((state) => ({
    content,
    isDirty: content !== state.savedContent
  })),
  setSavedContent: (content) => set({ savedContent: content, isDirty: false }),
  setDirty: (isDirty) => set({ isDirty }),
  resetFile: () => set({
    filePath: null,
    content: '',
    savedContent: '',
    isDirty: false
  }),
  openFile: (path, content) => set((state) => {
    const updatedRecents = state.recentFiles.filter((p) => p !== path)
    updatedRecents.unshift(path)
    return {
      filePath: path,
      content,
      savedContent: content,
      isDirty: false,
      recentFiles: updatedRecents.slice(0, 10)
    }
  }),
  saveFile: (path, content) => set((state) => {
    const updatedRecents = state.recentFiles.filter((p) => p !== path)
    updatedRecents.unshift(path)
    return {
      filePath: path,
      content,
      savedContent: content,
      isDirty: false,
      recentFiles: updatedRecents.slice(0, 10)
    }
  }),
  addRecentFile: (path) => set((state) => {
    const updatedRecents = state.recentFiles.filter((p) => p !== path)
    updatedRecents.unshift(path)
    return {
      recentFiles: updatedRecents.slice(0, 10)
    }
  })
}))
