export interface ThemeColors {
  background: string
  surface: string
  accent: string
  text: string
  textMuted: string
  border: string
}

export interface ThemeConfig {
  id: string
  name: string
  dark: boolean
  colors: ThemeColors
}
