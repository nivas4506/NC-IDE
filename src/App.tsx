import React, { useEffect } from 'react'
import { Workspace } from './modules/ui/Workspace'
import { useNativeMenuShortcuts } from './modules/ui/shortcuts'
import { fileService } from './modules/files/fileService'
import './modules/theme/tokens.css'

export const App: React.FC = () => {
  // Bind native menu actions (New, Open, Save, Save As)
  useNativeMenuShortcuts()

  // Handle close confirmation from Electron main process
  useEffect(() => {
    const unbind = window.api.app.onCloseRequest(async () => {
      const shouldClose = await fileService.confirmDiscardChanges()
      if (shouldClose) {
        window.api.app.confirmClose()
      }
    })

    return () => {
      unbind()
    }
  }, [])

  return <Workspace />
}
export default App
