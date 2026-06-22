import { ipcMain, dialog, BrowserWindow } from 'electron'
import * as fs from 'fs/promises'

export function registerFileHandlers() {
  ipcMain.handle('file:open', async (event) => {
    const window = BrowserWindow.fromWebContents(event.sender)
    if (!window) return null

    const result = await dialog.showOpenDialog(window, {
      title: 'Open File',
      properties: ['openFile'],
      filters: [
        { name: 'All Files', extensions: ['*'] },
        { name: 'Text Files', extensions: ['txt', 'md', 'js', 'ts', 'tsx', 'html', 'css', 'json', 'yaml', 'yml'] }
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

  ipcMain.handle('file:save', async (_event, filePath: string, content: string) => {
    try {
      await fs.writeFile(filePath, content, 'utf-8')
      return true
    } catch (error) {
      console.error(`Failed to save file: ${filePath}`, error)
      throw error
    }
  })

  ipcMain.handle('file:save-as', async (event, content: string) => {
    const window = BrowserWindow.fromWebContents(event.sender)
    if (!window) return null

    const result = await dialog.showSaveDialog(window, {
      title: 'Save File As',
      filters: [
        { name: 'All Files', extensions: ['*'] },
        { name: 'Text Files', extensions: ['txt', 'md', 'js', 'ts', 'tsx', 'html', 'css', 'json', 'yaml', 'yml'] }
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
}
