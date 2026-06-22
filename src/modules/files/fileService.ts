import { useActiveFileStore } from './activeFile.store'

export const fileService = {
  confirmDiscardChanges: async (): Promise<boolean> => {
    const { isDirty } = useActiveFileStore.getState()
    if (!isDirty) return true

    // Show unsaved changes dialog
    const choice = await window.api.file.showUnsavedChangesDialog()
    if (choice === 'save') {
      return await fileService.save()
    } else if (choice === 'dont-save') {
      return true
    }
    return false // cancel
  },

  newFile: async () => {
    if (await fileService.confirmDiscardChanges()) {
      useActiveFileStore.getState().resetFile()
    }
  },

  open: async () => {
    if (await fileService.confirmDiscardChanges()) {
      try {
        const result = await window.api.file.open()
        if (result) {
          useActiveFileStore.getState().openFile(result.filePath, result.content)
        }
      } catch (err) {
        console.error('Failed to open file:', err)
      }
    }
  },

  save: async (): Promise<boolean> => {
    const { filePath, content } = useActiveFileStore.getState()
    if (filePath) {
      try {
        const success = await window.api.file.save(filePath, content)
        if (success) {
          useActiveFileStore.getState().setSavedContent(content)
          return true
        }
      } catch (err) {
        console.error('Failed to save file:', err)
      }
      return false
    } else {
      return await fileService.saveAs()
    }
  },

  saveAs: async (): Promise<boolean> => {
    const { content } = useActiveFileStore.getState()
    try {
      const newPath = await window.api.file.saveAs(content)
      if (newPath) {
        useActiveFileStore.getState().saveFile(newPath, content)
        return true
      }
    } catch (err) {
      console.error('Failed to save file as:', err)
    }
    return false
  }
}
