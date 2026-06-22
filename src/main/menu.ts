import { Menu, BrowserWindow, app, dialog } from 'electron'

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
            if (win) win.webContents.send('menu:new')
          }
        },
        {
          label: 'Open File...',
          accelerator: 'CmdOrCtrl+O',
          click: () => {
            const win = BrowserWindow.getFocusedWindow()
            if (win) win.webContents.send('menu:open')
          }
        },
        {
          label: 'Save',
          accelerator: 'CmdOrCtrl+S',
          click: () => {
            const win = BrowserWindow.getFocusedWindow()
            if (win) win.webContents.send('menu:save')
          }
        },
        {
          label: 'Save As...',
          accelerator: 'CmdOrCtrl+Shift+S',
          click: () => {
            const win = BrowserWindow.getFocusedWindow()
            if (win) win.webContents.send('menu:save-as')
          }
        },
        { type: 'separator' },
        {
          label: 'Exit',
          click: () => {
            app.quit()
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
        { type: 'separator' },
        { role: 'selectAll' }
      ]
    },
    {
      label: 'View',
      submenu: [
        {
          label: 'Toggle Status Bar',
          click: () => {
            const win = BrowserWindow.getFocusedWindow()
            if (win) win.webContents.send('menu:toggle-statusbar')
          }
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
              dialog.showMessageBox(browserWindow, {
                type: 'info',
                title: 'About NC IDE',
                message: 'NC IDE',
                detail: 'NC IDE — Week 1 Foundation\n\nA lightweight, developer-first IDE built on Electron, React, and CodeMirror 6.'
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
