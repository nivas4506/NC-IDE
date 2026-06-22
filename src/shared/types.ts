export interface UserConfig {
  theme: string
  fontSize: number
}

declare global {
  interface Window {
    api: {
      file: {
        open: () => Promise<{ filePath: string; content: string } | null>
        save: (filePath: string, content: string) => Promise<boolean>
        saveAs: (content: string) => Promise<string | null>
        showUnsavedChangesDialog: () => Promise<'save' | 'dont-save' | 'cancel'>
        onMenuNew: (callback: () => void) => () => void
        onMenuOpen: (callback: () => void) => () => void
        onMenuSave: (callback: () => void) => () => void
        onMenuSaveAs: (callback: () => void) => () => void
      }
      app: {
        onCloseRequest: (callback: () => void) => () => void
        confirmClose: () => void
      }
    }
  }
}
export {}
