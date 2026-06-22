import { contextBridge, ipcRenderer } from 'electron'

contextBridge.exposeInMainWorld('api', {
  file: {
    open: () => ipcRenderer.invoke('file:open'),
    save: (path: string, content: string) => ipcRenderer.invoke('file:save', path, content),
    saveAs: (content: string) => ipcRenderer.invoke('file:save-as', content),
    onMenuNew: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on('menu:new', listener)
      return () => {
        ipcRenderer.removeListener('menu:new', listener)
      }
    },
    onMenuOpen: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on('menu:open', listener)
      return () => {
        ipcRenderer.removeListener('menu:open', listener)
      }
    },
    onMenuSave: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on('menu:save', listener)
      return () => {
        ipcRenderer.removeListener('menu:save', listener)
      }
    },
    onMenuSaveAs: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on('menu:save-as', listener)
      return () => {
        ipcRenderer.removeListener('menu:save-as', listener)
      }
    },
    onMenuToggleStatusBar: (callback: () => void) => {
      const listener = () => callback()
      ipcRenderer.on('menu:toggle-statusbar', listener)
      return () => {
        ipcRenderer.removeListener('menu:toggle-statusbar', listener)
      }
    }
  }
})
