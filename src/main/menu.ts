import { Menu, BrowserWindow, app, dialog } from 'electron'
import { IPC_CHANNELS } from '../shared/ipcChannels'

export function createApplicationMenu() {
  const template: Electron.MenuItemConstructorOptions[] = [
    {
      label: 'File',
      submenu: [
        {
          label: 'New File',
          accelerator: 'CmdOrCtrl+N',
          click: () => {
            const win = BrowserWindow.getFocusedWindow()
            if (win) win.webContents.send(IPC_CHANNELS.MENU_NEW)
          }
        },
        {
          label: 'Open File...',
          accelerator: 'CmdOrCtrl+O',
          click: () => {
            const win = BrowserWindow.getFocusedWindow()
            if (win) win.webContents.send(IPC_CHANNELS.MENU_OPEN)
          }
        },
        {
          label: 'Save',
          accelerator: 'CmdOrCtrl+S',
          click: () => {
            const win = BrowserWindow.getFocusedWindow()
            if (win) win.webContents.send(IPC_CHANNELS.MENU_SAVE)
          }
        },
        {
          label: 'Save As...',
          accelerator: 'CmdOrCtrl+Shift+S',
          click: () => {
            const win = BrowserWindow.getFocusedWindow()
            if (win) win.webContents.send(IPC_CHANNELS.MENU_SAVE_AS)
          }
        },
        { type: 'separator' },
        {
          label: 'Exit',
          accelerator: 'CmdOrCtrl+Q',
          click: () => {
            const win = BrowserWindow.getFocusedWindow()
            if (win) {
              win.close() // goes through the close confirmation guard
            } else {
              app.quit()
            }
          }
        }
      ]
    },
    {
      label: 'Edit',
      submenu: [
        { role: 'undo' },
        { role: 'redo' },
        { type: 'separator' },
        { role: 'cut' },
        { role: 'copy' },
        { role: 'paste' },
        { role: 'selectAll' }
      ]
    },
    {
      label: 'View',
      submenu: [
        {
          label: 'Toggle Sidebar (Stub)',
          enabled: false
        },
        { type: 'separator' },
        { role: 'reload' },
        { role: 'forceReload' },
        { role: 'toggleDevTools' },
        { type: 'separator' },
        { role: 'resetZoom' },
        { role: 'zoomIn' },
        { role: 'zoomOut' },
        { type: 'separator' },
        { role: 'togglefullscreen' }
      ]
    },
    {
      label: 'Help',
      submenu: [
        {
          label: 'About NC IDE',
          click: (menuItem, browserWindow) => {
            if (browserWindow) {
              dialog.showMessageBox(browserWindow as BrowserWindow, {
                type: 'info',
                title: 'About NC IDE',
                message: 'NC IDE',
                detail: 'NC IDE — Week 1 Foundation (Monaco)\n\nA lightweight, developer-first IDE built on Electron, React, and Monaco Editor.'
              })
            }
          }
        }
      ]
    }
  ]

  const menu = Menu.buildFromTemplate(template)
  Menu.setApplicationMenu(menu)
}
