export const IPC_CHANNELS = {
  FILE_OPEN: 'file:open',
  FILE_SAVE: 'file:save',
  FILE_SAVE_AS: 'file:save-as',
  FILE_UNSAVED_DIALOG: 'file:unsaved-dialog',
  MENU_NEW: 'menu:new',
  MENU_OPEN: 'menu:open',
  MENU_SAVE: 'menu:save',
  MENU_SAVE_AS: 'menu:save-as',
  MENU_TOGGLE_SIDEBAR: 'menu:toggle-sidebar',
  APP_CLOSE_REQUEST: 'app:close-request',
  APP_CLOSE_CONFIRMED: 'app:close-confirmed'
} as const
