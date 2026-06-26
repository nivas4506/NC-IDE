"use strict";
const electron = require("electron");
const path = require("path");
const fs = require("fs/promises");
function _interopNamespaceDefault(e) {
  const n = Object.create(null, { [Symbol.toStringTag]: { value: "Module" } });
  if (e) {
    for (const k in e) {
      if (k !== "default") {
        const d = Object.getOwnPropertyDescriptor(e, k);
        Object.defineProperty(n, k, d.get ? d : {
          enumerable: true,
          get: () => e[k]
        });
      }
    }
  }
  n.default = e;
  return Object.freeze(n);
}
const fs__namespace = /* @__PURE__ */ _interopNamespaceDefault(fs);
const IPC_CHANNELS = {
  FILE_OPEN: "file:open",
  FILE_SAVE: "file:save",
  FILE_SAVE_AS: "file:save-as",
  FILE_UNSAVED_DIALOG: "file:unsaved-dialog",
  MENU_NEW: "menu:new",
  MENU_OPEN: "menu:open",
  MENU_SAVE: "menu:save",
  MENU_SAVE_AS: "menu:save-as",
  APP_CLOSE_REQUEST: "app:close-request",
  APP_CLOSE_CONFIRMED: "app:close-confirmed"
};
function registerFileHandlers() {
  electron.ipcMain.handle(IPC_CHANNELS.FILE_OPEN, async (event) => {
    const window = electron.BrowserWindow.fromWebContents(event.sender);
    if (!window) return null;
    const result = await electron.dialog.showOpenDialog(window, {
      title: "Open File",
      properties: ["openFile"],
      filters: [
        { name: "All Files", extensions: ["*"] },
        { name: "Text Files", extensions: ["txt", "md", "js", "jsx", "ts", "tsx", "html", "css", "json", "py", "yaml", "yml"] }
      ]
    });
    if (result.canceled || result.filePaths.length === 0) {
      return null;
    }
    const filePath = result.filePaths[0];
    try {
      const content = await fs__namespace.readFile(filePath, "utf-8");
      return { filePath, content };
    } catch (error) {
      console.error(`Failed to read file: ${filePath}`, error);
      throw error;
    }
  });
  electron.ipcMain.handle(IPC_CHANNELS.FILE_SAVE, async (_event, filePath, content) => {
    try {
      await fs__namespace.writeFile(filePath, content, "utf-8");
      return true;
    } catch (error) {
      console.error(`Failed to save file: ${filePath}`, error);
      throw error;
    }
  });
  electron.ipcMain.handle(IPC_CHANNELS.FILE_SAVE_AS, async (event, content) => {
    const window = electron.BrowserWindow.fromWebContents(event.sender);
    if (!window) return null;
    const result = await electron.dialog.showSaveDialog(window, {
      title: "Save File As",
      filters: [
        { name: "All Files", extensions: ["*"] },
        { name: "Text Files", extensions: ["txt", "md", "js", "jsx", "ts", "tsx", "html", "css", "json", "py", "yaml", "yml"] }
      ]
    });
    if (result.canceled || !result.filePath) {
      return null;
    }
    const filePath = result.filePath;
    try {
      await fs__namespace.writeFile(filePath, content, "utf-8");
      return filePath;
    } catch (error) {
      console.error(`Failed to save-as file: ${filePath}`, error);
      throw error;
    }
  });
  electron.ipcMain.handle(IPC_CHANNELS.FILE_UNSAVED_DIALOG, async (event) => {
    const window = electron.BrowserWindow.fromWebContents(event.sender);
    if (!window) return "cancel";
    const result = await electron.dialog.showMessageBox(window, {
      type: "warning",
      buttons: ["Save", "Don't Save", "Cancel"],
      defaultId: 0,
      cancelId: 2,
      title: "Unsaved Changes",
      message: "Do you want to save the changes you made to the file?",
      detail: "Your changes will be lost if you don't save them."
    });
    if (result.response === 0) return "save";
    if (result.response === 1) return "dont-save";
    return "cancel";
  });
}
function createApplicationMenu() {
  const template = [
    {
      label: "File",
      submenu: [
        {
          label: "New File",
          accelerator: "CmdOrCtrl+N",
          click: () => {
            const win = electron.BrowserWindow.getFocusedWindow();
            if (win) win.webContents.send(IPC_CHANNELS.MENU_NEW);
          }
        },
        {
          label: "Open File...",
          accelerator: "CmdOrCtrl+O",
          click: () => {
            const win = electron.BrowserWindow.getFocusedWindow();
            if (win) win.webContents.send(IPC_CHANNELS.MENU_OPEN);
          }
        },
        {
          label: "Save",
          accelerator: "CmdOrCtrl+S",
          click: () => {
            const win = electron.BrowserWindow.getFocusedWindow();
            if (win) win.webContents.send(IPC_CHANNELS.MENU_SAVE);
          }
        },
        {
          label: "Save As...",
          accelerator: "CmdOrCtrl+Shift+S",
          click: () => {
            const win = electron.BrowserWindow.getFocusedWindow();
            if (win) win.webContents.send(IPC_CHANNELS.MENU_SAVE_AS);
          }
        },
        { type: "separator" },
        {
          label: "Exit",
          accelerator: "CmdOrCtrl+Q",
          click: () => {
            const win = electron.BrowserWindow.getFocusedWindow();
            if (win) {
              win.close();
            } else {
              electron.app.quit();
            }
          }
        }
      ]
    },
    {
      label: "Edit",
      submenu: [
        { role: "undo" },
        { role: "redo" },
        { type: "separator" },
        { role: "cut" },
        { role: "copy" },
        { role: "paste" },
        { role: "selectAll" }
      ]
    },
    {
      label: "View",
      submenu: [
        {
          label: "Toggle Sidebar (Stub)",
          enabled: false
        },
        { type: "separator" },
        { role: "reload" },
        { role: "forceReload" },
        { role: "toggleDevTools" },
        { type: "separator" },
        { role: "resetZoom" },
        { role: "zoomIn" },
        { role: "zoomOut" },
        { type: "separator" },
        { role: "togglefullscreen" }
      ]
    },
    {
      label: "Help",
      submenu: [
        {
          label: "About NC IDE",
          click: (menuItem, browserWindow) => {
            if (browserWindow) {
              electron.dialog.showMessageBox(browserWindow, {
                type: "info",
                title: "About NC IDE",
                message: "NC IDE",
                detail: "NC IDE — Week 1 Foundation (Monaco)\n\nA lightweight, developer-first IDE built on Electron, React, and Monaco Editor."
              });
            }
          }
        }
      ]
    }
  ];
  const menu = electron.Menu.buildFromTemplate(template);
  electron.Menu.setApplicationMenu(menu);
}
let isQuitting = false;
function createWindow() {
  const mainWindow = new electron.BrowserWindow({
    width: 1280,
    height: 800,
    minWidth: 800,
    minHeight: 600,
    title: "NC IDE",
    backgroundColor: "#121212",
    webPreferences: {
      preload: path.join(__dirname, "../preload/index.js"),
      nodeIntegration: false,
      contextIsolation: true,
      sandbox: true
    }
  });
  registerFileHandlers();
  createApplicationMenu();
  if (process.env.ELECTRON_RENDERER_URL) {
    mainWindow.loadURL(process.env.ELECTRON_RENDERER_URL);
  } else {
    mainWindow.loadFile(path.join(__dirname, "../renderer/index.html"));
  }
  mainWindow.on("close", (e) => {
    if (isQuitting) return;
    e.preventDefault();
    mainWindow.webContents.send(IPC_CHANNELS.APP_CLOSE_REQUEST);
  });
}
electron.ipcMain.on(IPC_CHANNELS.APP_CLOSE_CONFIRMED, () => {
  isQuitting = true;
  electron.app.quit();
});
electron.app.whenReady().then(() => {
  createWindow();
  electron.app.on("activate", () => {
    if (electron.BrowserWindow.getAllWindows().length === 0) {
      createWindow();
    }
  });
});
electron.app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    electron.app.quit();
  }
});
