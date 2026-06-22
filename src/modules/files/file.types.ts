export interface ActiveFile {
  path: string | null
  content: string
  savedContent: string
  isDirty: boolean
}
