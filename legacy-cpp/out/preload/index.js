"use strict";
const electron = require("electron");
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
electron.contextBridge.exposeInMainWorld("api", {
  file: {
    open: () => electron.ipcRenderer.invoke(IPC_CHANNELS.FILE_OPEN),
    save: (filePath, content) => electron.ipcRenderer.invoke(IPC_CHANNELS.FILE_SAVE, filePath, content),
    saveAs: (content) => electron.ipcRenderer.invoke(IPC_CHANNELS.FILE_SAVE_AS, content),
    showUnsavedChangesDialog: () => electron.ipcRenderer.invoke(IPC_CHANNELS.FILE_UNSAVED_DIALOG),
    onMenuNew: (callback) => {
      const listener = () => callback();
      electron.ipcRenderer.on(IPC_CHANNELS.MENU_NEW, listener);
      return () => {
        electron.ipcRenderer.removeListener(IPC_CHANNELS.MENU_NEW, listener);
      };
    },
    onMenuOpen: (callback) => {
      const listener = () => callback();
      electron.ipcRenderer.on(IPC_CHANNELS.MENU_OPEN, listener);
      return () => {
        electron.ipcRenderer.removeListener(IPC_CHANNELS.MENU_OPEN, listener);
      };
    },
    onMenuSave: (callback) => {
      const listener = () => callback();
      electron.ipcRenderer.on(IPC_CHANNELS.MENU_SAVE, listener);
      return () => {
        electron.ipcRenderer.removeListener(IPC_CHANNELS.MENU_SAVE, listener);
      };
    },
    onMenuSaveAs: (callback) => {
      const listener = () => callback();
      electron.ipcRenderer.on(IPC_CHANNELS.MENU_SAVE_AS, listener);
      return () => {
        electron.ipcRenderer.removeListener(IPC_CHANNELS.MENU_SAVE_AS, listener);
      };
    }
  },
  app: {
    onCloseRequest: (callback) => {
      const listener = () => callback();
      electron.ipcRenderer.on(IPC_CHANNELS.APP_CLOSE_REQUEST, listener);
      return () => {
        electron.ipcRenderer.removeListener(IPC_CHANNELS.APP_CLOSE_REQUEST, listener);
      };
    },
    confirmClose: () => electron.ipcRenderer.send(IPC_CHANNELS.APP_CLOSE_CONFIRMED)
  }
});
