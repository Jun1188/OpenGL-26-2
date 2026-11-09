# Standard workspace redesign

- Approved: 2026-11-09, including nonuniform scale, world-authored object attachments, object-owned local layers, and command [result] - argument descriptions.
- Baseline: e6af6deb4b2c8acc5028307592b654bcaeebbf4d.
- Existing untracked docs.zip is user-owned and remains untouched.
- Step 1: mode-0-only runtime and concise independent console; old practice/controller sources remain reference-only.
- Step 2: affine attachments, nonuniform scale, ordered local layers, picking, placement and SRT shortcuts; deterministic persistence.
- Step 3: pickable lights/cameras and point-light rendering.
- Step 4: generic acceleration/force hooks and moving-scale application.
- Step 5: regenerate current documentation and architecture, report all code changes and user-run checks.
- Each step is a separate commit. No builds, tests, validators, app launches, or browser checks are executed.

- Additional user direction: all previous modes are reference documents only. Full legacy source is preserved in standard/docs/reference/modes/*.md; Standard explicitly enumerates only current workspace translation units.
