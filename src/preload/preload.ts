import { contextBridge, ipcRenderer } from 'electron'
import { IPC_CHANNELS } from '../shared/ipcChannels'

contextBridge.exposeInMainWorld('api', {
  file: {
    open: () => ipcRenderer.invoke(IPC_CHANNELS.FILE_OPEN),
    save: (filePath: string, content: string) => ipcRenderer.invoke(IPC_CHANNELS.FILE_SAVE, filePath, content),
    saveAs: (content: string) => ipcRenderer.invoke(IPC_CHANNELS.FILE_SAVE_AS, content),
    showUnsavedChangesDialog: () => ipcRenderer.invoke(IPC_CHANNELS.FILE_UNSAVED_DIALOG),
    onMenuNew: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on(IPC_CHANNELS.MENU_NEW, listener)
      return () => {
        ipcRenderer.removeListener(IPC_CHANNELS.MENU_NEW, listener)
      }
    },
    onMenuOpen: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on(IPC_CHANNELS.MENU_OPEN, listener)
      return () => {
        ipcRenderer.removeListener(IPC_CHANNELS.MENU_OPEN, listener)
      }
    },
    onMenuSave: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on(IPC_CHANNELS.MENU_SAVE, listener)
      return () => {
        ipcRenderer.removeListener(IPC_CHANNELS.MENU_SAVE, listener)
      }
    },
    onMenuSaveAs: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on(IPC_CHANNELS.MENU_SAVE_AS, listener)
      return () => {
        ipcRenderer.removeListener(IPC_CHANNELS.MENU_SAVE_AS, listener)
      }
    }
  },
  app: {
    onCloseRequest: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on(IPC_CHANNELS.APP_CLOSE_REQUEST, listener)
      return () => {
        ipcRenderer.removeListener(IPC_CHANNELS.APP_CLOSE_REQUEST, listener)
      }
    },
    confirmClose: () => ipcRenderer.send(IPC_CHANNELS.APP_CLOSE_CONFIRMED)
  }
})
