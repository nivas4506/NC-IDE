import { useEffect } from 'react'
import { fileService } from '../files/fileService'

export function useNativeMenuShortcuts() {
  useEffect(() => {
    const unbindNew = window.api.file.onMenuNew(() => {
      fileService.newFile()
    })
    const unbindOpen = window.api.file.onMenuOpen(() => {
      fileService.open()
    })
    const unbindSave = window.api.file.onMenuSave(() => {
      fileService.save()
    })
    const unbindSaveAs = window.api.file.onMenuSaveAs(() => {
      fileService.saveAs()
    })

    return () => {
      unbindNew()
      unbindOpen()
      unbindSave()
      unbindSaveAs()
    }
  }, [])
}
