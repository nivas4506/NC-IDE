import { ipcMain, dialog, BrowserWindow } from 'electron'
import * as fs from 'fs/promises'
import { IPC_CHANNELS } from '../../shared/ipcChannels'

export function registerFileHandlers() {
  ipcMain.handle(IPC_CHANNELS.FILE_OPEN, async (event) => {
    const window = BrowserWindow.fromWebContents(event.sender)
    if (!window) return null

    const result = await dialog.showOpenDialog(window, {
      title: 'Open File',
      properties: ['openFile'],
      filters: [
        { name: 'All Files', extensions: ['*'] },
        { name: 'Text Files', extensions: ['txt', 'md', 'js', 'jsx', 'ts', 'tsx', 'html', 'css', 'json', 'py', 'yaml', 'yml'] }
      ]
    })

    if (result.canceled || result.filePaths.length === 0) {
      return null
    }

    const filePath = result.filePaths[0]
    try {
      const content = await fs.readFile(filePath, 'utf-8')
      return { filePath, content }
    } catch (error) {
      console.error(`Failed to read file: ${filePath}`, error)
      throw error
    }
  })

  ipcMain.handle(IPC_CHANNELS.FILE_SAVE, async (_event, filePath: string, content: string) => {
    try {
      await fs.writeFile(filePath, content, 'utf-8')
      return true
    } catch (error) {
      console.error(`Failed to save file: ${filePath}`, error)
      throw error
    }
  })

  ipcMain.handle(IPC_CHANNELS.FILE_SAVE_AS, async (event, content: string) => {
    const window = BrowserWindow.fromWebContents(event.sender)
    if (!window) return null

    const result = await dialog.showSaveDialog(window, {
      title: 'Save File As',
      filters: [
        { name: 'All Files', extensions: ['*'] },
        { name: 'Text Files', extensions: ['txt', 'md', 'js', 'jsx', 'ts', 'tsx', 'html', 'css', 'json', 'py', 'yaml', 'yml'] }
      ]
    })

    if (result.canceled || !result.filePath) {
      return null
    }

    const filePath = result.filePath
    try {
      await fs.writeFile(filePath, content, 'utf-8')
      return filePath
    } catch (error) {
      console.error(`Failed to save-as file: ${filePath}`, error)
      throw error
    }
  })

  ipcMain.handle(IPC_CHANNELS.FILE_UNSAVED_DIALOG, async (event) => {
    const window = BrowserWindow.fromWebContents(event.sender)
    if (!window) return 'cancel'

    const result = await dialog.showMessageBox(window, {
      type: 'warning',
      buttons: ['Save', "Don't Save", 'Cancel'],
      defaultId: 0,
      cancelId: 2,
      title: 'Unsaved Changes',
      message: 'Do you want to save the changes you made to the file?',
      detail: "Your changes will be lost if you don't save them."
    })

    if (result.response === 0) return 'save'
    if (result.response === 1) return 'dont-save'
    return 'cancel'
  })
}
