import { app, BrowserWindow, ipcMain } from 'electron'
import { join } from 'path'
import { registerFileHandlers } from './ipc/fileHandlers'
import { createApplicationMenu } from './menu'
import { IPC_CHANNELS } from '../shared/ipcChannels'

let isQuitting = false

function createWindow() {
  const mainWindow = new BrowserWindow({
    width: 1280,
    height: 800,
    minWidth: 800,
    minHeight: 600,
    title: 'NC IDE',
    backgroundColor: '#121212',
    webPreferences: {
      preload: join(__dirname, '../preload/index.js'),
      nodeIntegration: false,
      contextIsolation: true,
      sandbox: true
    }
  })

  // Register IPC handlers
  registerFileHandlers()

  // Set up native application menu
  createApplicationMenu()

  // Load app (development vs production)
  if (process.env.ELECTRON_RENDERER_URL) {
    mainWindow.loadURL(process.env.ELECTRON_RENDERER_URL)
  } else {
    mainWindow.loadFile(join(__dirname, '../renderer/index.html'))
  }

  // Handle window close request
  mainWindow.on('close', (e) => {
    if (isQuitting) return
    e.preventDefault()
    mainWindow.webContents.send(IPC_CHANNELS.APP_CLOSE_REQUEST)
  })
}

// Listen to confirmed exit
ipcMain.on(IPC_CHANNELS.APP_CLOSE_CONFIRMED, () => {
  isQuitting = true
  app.quit()
})

app.whenReady().then(() => {
  createWindow()

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow()
    }
  })
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})
