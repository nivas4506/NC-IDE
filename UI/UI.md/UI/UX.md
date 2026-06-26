# NC IDE — Liquid UI Design Spec v2 (Advanced)

Builds on `NC-IDE-Liquid-UI-Spec.md` (v1). v1 tokens still apply — this adds depth layering, micro-interaction choreography, specular/noise realism, adaptive surfaces, and state-driven morphing.

## 1. Depth Model (Z-Layer System)

Liquid UI needs a real depth hierarchy, not just "glass vs. not glass." Define explicit Z-layers so blur, shadow, and parallax stay consistent across every surface instead of being decided ad hoc per component.

```json
{
  "zLayers": {
    "z0_base": { "elevation": "level0", "blur": "none", "parallax": 0 },
    "z1_chrome": { "elevation": "level1", "blur": "blur.tooltip", "parallax": 0.02 },
    "z2_panel": { "elevation": "level2", "blur": "blur.panel", "parallax": 0.05 },
    "z3_overlay": { "elevation": "level3", "blur": "blur.overlay", "parallax": 0.08 },
    "z4_modal": { "elevation": "level3", "blur": "28px", "parallax": 0.12, "scrim": "rgba(0,0,0,0.55)" }
  }
}
```

- `parallax` = how far a surface shifts (in px, scaled to cursor or window-drag delta) relative to the base layer. Subtle — this is what makes glass feel like it has physical thickness rather than being a flat translucent paint job.
- Rule: a surface's blur radius and shadow elevation must always move together. Never increase blur without increasing elevation — that mismatch is what makes glassmorphism look fake.

## 2. Specular + Noise Layer (Realism Pass)

Flat translucency reads as "frosted plastic." Add two cheap passes on top of every glass surface:

- **Specular edge**: 1px top-edge highlight, `rgba(255,255,255,0.18)`, fading to 0 by 40% down the surface. Simulates light catching the top edge of glass.
- **Noise overlay**: static 2% opacity grain texture (pre-rendered 64×64 tile, repeated), blend mode `overlay`. Kills the "too clean/digital" look of pure blur. Render once at theme load, never regenerate per frame.

```json
{
  "specular": { "opacity": 0.18, "spread": "40%", "position": "top" },
  "noise": { "opacity": 0.02, "tileSize": "64px", "blendMode": "overlay" }
}
```

Qt6 note: bake specular + noise into the cached blur pixmap from v1 §6 — same single re-render trigger (resize/theme change), zero extra runtime cost.

## 3. State-Driven Morphing (Shared Element Transitions)

This is the actual "liquid" part — UI elements that morph between states rather than swap. Three required morph patterns:

| Trigger | From → To | Behavior |
|---|---|---|
| Command Palette open | Toolbar search icon → full palette | Icon bounds expand into palette bounds (shared geometry interpolation), not a fade-swap. Icon's circular shape morphs into the palette's rounded-rect. |
| File tab → Editor focus | Tab chrome → active tab underline | Underline width/position animates from previous active tab's position, not a hard jump. |
| Problems Panel item → Editor line | Click error in panel | Camera-style scroll-to-line in editor, paired with a brief glow pulse (`accentGlow`, 2 pulses, 400ms) on the target line — not a hard jump-cut. |

```json
{
  "morphMotion": {
    "sharedGeometry": { "duration": "slow", "easing": "easeMorph" },
    "underlineFollow": { "duration": "fast", "easing": "easeStandard" },
    "glowPulse": { "duration": "320ms", "repeat": 2, "color": "accentGlow" }
  }
}
```

Qt6 implementation: shared-geometry morphs need a custom `QVariantAnimation` interpolating both `geometry()` and `borderRadius` simultaneously (not two separate animations — they must stay locked in lockstep or the morph looks like two unrelated tweens).

## 4. Micro-Interaction Catalogue

Beyond panel-level motion (v1 §4), define feedback at the smallest interactive unit:

| Element | Idle → Hover | Hover → Press | Notes |
|---|---|---|---|
| Buttons | Surface lightens 6%, fast | Scale 0.97, fast | Release snaps back with slight overshoot (easeMorph) |
| File tree row | Background fade-in `borderGlass`, fast | — | Row height never changes, only background |
| Checkbox/toggle | — | Liquid fill animates left→right, base, easeMorph | Fill is a literal "liquid" sweep, not an instant color swap |
| Slider (e.g. zoom, font size) | Track glows on drag start | Thumb scales 1.15 while dragging | Glow fades 200ms after drag end |
| Git status dot | — | Pulses once on state change (added/modified/deleted) | Single pulse, not looping — looping idle animations are forbidden (battery/distraction) |

## 5. Adaptive Glass (Context-Aware Translucency)

Static blur values aren't enough at scale. Glass surfaces should adapt opacity based on what's behind them, since v1 §5 already requires contrast safety — this section makes that dynamic instead of a fixed worst-case fallback:

- Sample average luminance of the region directly behind a glass panel (cheap: downsample backing content to 8×8 before blur pass).
- If luminance > 0.6 (bright code theme, e.g. light syntax background bleeding through), auto-increase `surfaceGlass` alpha from 0.55 → 0.75 to protect contrast.
- This sampling runs only on theme load and on major layout changes (panel resize, file switch) — never per-frame.

```json
{
  "adaptiveGlass": {
    "luminanceThreshold": 0.6,
    "alphaLow": 0.55,
    "alphaHigh": 0.75,
    "sampleResolution": "8x8",
    "resampleTriggers": ["themeChange", "panelResize", "majorLayoutChange"]
  }
}
```

## 6. Typography & Grid Refinement

v1 set base sizes; v2 adds a real scale and baseline grid so glass panels don't feel like they're floating arbitrarily:

```json
{
  "typeScale": {
    "ratio": 1.125,
    "steps": ["11px", "12.4px", "14px", "15.7px", "17.7px"]
  },
  "grid": {
    "baseline": "4px",
    "panelPadding": "12px",
    "panelGap": "8px",
    "minTouchTarget": "28px"
  }
}
```

All glass panel internal spacing snaps to the 4px baseline — prevents the "soft blur + misaligned content" look that makes glassmorphism feel sloppy.

## 7. Caret, Minimap & Editor-Adjacent Chrome

Editor surface itself stays glass-free (v1 §5 is absolute), but its *immediate* chrome gets refined treatment:

- **Caret**: smooth position interpolation (not teleport) when moving via click or arrow-repeat, `fast`/`easeStandard`. Blink uses opacity fade, not hard on/off, to avoid the "strobe" feel.
- **Minimap**: rendered at `z1_chrome` — gets the lightest blur tier so it visually recedes from the editor without losing legibility of code shape.
- **Selection highlight**: fill fades in (`fast`) rather than appearing instantly; fixes the jarring flash on large multi-line selects.

## 8. Accessibility & Motion Safety (Extends v1 §5)

- `prefers-reduced-motion`-equivalent setting disables: shared-geometry morphs (§3), parallax (§1), specular/noise (§2), liquid fill sweep (§4). Reduced mode keeps simple opacity fades only — functionally identical to "Reduced Transparency" from v1 but covers motion, not just blur.
- All glow pulses (§3, §4) capped at 2 repetitions max — anything looping indefinitely is disallowed regardless of feature.
- Color is never the sole status signal: git status dots, error/warning/success states must pair color with a shape or icon difference for colorblind users.

## 9. Updated Deliverables Checklist (additive to v1 §7)

- [ ] Z-layer constants module (`zLayers` from §1) consumed by every glass widget base class
- [ ] Pre-baked specular+noise pixmap pipeline, merged into v1's cached blur render
- [ ] Shared-geometry morph animator (`QVariantAnimation`-based) for palette/tab/glow transitions
- [ ] Adaptive glass luminance sampler (8×8 downsample + alpha switch)
- [ ] Reduced Motion settings toggle, distinct from but paired with Reduced Transparency
- [ ] Type scale + 4px baseline grid applied across all panel padding/gap values

## 10. Still Out of Scope

- No physics-based (spring/momentum) dragging — morphs are eased, not simulated. Keeps Qt6 implementation deterministic and cheap.
- No per-user custom theme editor in this pass — token files are static JSON, hand-authored.
- No animated app icon / OS-level dock effects.