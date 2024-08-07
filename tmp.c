{
    "$schema": "vscode://schemas/color-theme",
    "name": "Monokai Charcoal (red)",
    "tokenColors": [
        {
            "settings": {
                "foreground": "#FFFFFF",
                "background": "#000c18",
                "caret": "#F8F8F0",
                "invisibles": "#3B3A32",
                "lineHighlight": "#2c120b",
                "selection": "#2c120b",
                "findHighlight": "#FFE792",
                "findHighlightForeground": "#000000",
                "selectionBorder": "#222218",
                "activeGuide": "#9D550FB0",
                "bracketsForeground": "#F8F8F2A5",
                "bracketsOptions": "underline",
                "bracketContentsForeground": "#F8F8F2A5",
                "bracketContentsOptions": "underline",
                "tagsOptions": "stippled_underline"
            }
        },
        {
            "name": "Comment",
            "scope": ["comment"],
            "settings": { "foreground": "#636363" }
        },
        {
            "name": "String",
            "scope": "string",
            // "settings": { "foreground": "#E6DB74" }
            "settings": { "foreground": "#f0ff64" }
        },
        {
            "name": "Number",
            "scope": "constant.numeric",
            "settings": { "foreground": "#AE81FF" }
        },
        {
            "name": "Built-in constant",
            "scope": "constant.language",
            "settings": { "foreground": "#AE81FF" }
        },
        {
            "name": "User-defined constant",
            "scope": ["constant.character", "constant.other"],
            "settings": { "foreground": "#AE81FF" }
        },
        {
            "name": "Variable",
            "scope": "variable",
            "settings": { "fontStyle": "" }
        },
        {
            "name": "Keyword",
            "scope": "keyword",
            "settings": { "fontStyle": "bold", "foreground": "#F92672" }
        },
        {
            "name": "Storage",
            "scope": "storage",
            "settings": { "fontStyle": "bold", "foreground": "#F92672" }
        },
        {
            "name": "Storage type",
            "scope": "storage.type",
            "settings": { "fontStyle": "italic", "foreground": "#66D9EF" }
        },
        {
            "name": "Class name",
            "scope": "entity.name.class",
            "settings": { "fontStyle": "underline", "foreground": "#ff42bd" }
        },
        {
            "name": "Inherited class",
            "scope": "entity.other.inherited-class",
            "settings": {
                "fontStyle": "italic underline",
                "foreground": "#ff42bd"
            }
        },
        {
            "name": "Function name",
            "scope": "entity.name.function",
            "settings": { "fontStyle": "", "foreground": "#00ccb4" }
        },
        {
            "name": "Function argument",
            "scope": "variable.parameter",
            "settings": { "fontStyle": "italic", "foreground": "#e8ce9f" }
        },
        {
            "name": "Tag name",
            "scope": "entity.name.tag",
            "settings": { "fontStyle": "bold", "foreground": "#F92672" }
        },
        {
            "name": "Tag attribute",
            "scope": "entity.other.attribute-name",
            "settings": { "fontStyle": "", "foreground": "#ff42bd" }
        },
        {
            "name": "Library function",
            "scope": "support.function",
            "settings": { "fontStyle": "", "foreground": "#66D9EF" }
        },
        {
            "name": "Library constant",
            "scope": "support.constant",
            "settings": { "fontStyle": "", "foreground": "#66D9EF" }
        },
        {
            "name": "Library class/type",
            "scope": ["support.type", "support.class"],
            "settings": { "fontStyle": "italic", "foreground": "#66D9EF" }
        },
        {
            "name": "Library variable",
            "scope": "support.other.variable",
            "settings": { "fontStyle": "" }
        },
        {
            "name": "Invalid",
            "scope": "invalid",
            "settings": {
                "background": "#F92672",
                "fontStyle": "bold",
                "foreground": "#F8F8F0"
            }
        },
        {
            "name": "Invalid deprecated",
            "scope": "invalid.deprecated",
            "settings": { "background": "#AE81FF", "foreground": "#F8F8F0" }
        },
        {
            "name": "JSON String",
            "scope": [
                "meta.structure.dictionary.json",
                "string.quoted.double.json"
            ],
            "settings": { "foreground": "#CFCFC2" }
        },
        {
            "name": "diff.header",
            "scope": "meta.diff, meta.diff.header",
            "settings": { "foreground": "#75715E" }
        },
        {
            "name": "diff.deleted",
            "scope": "markup.deleted",
            "settings": { "background": "#F92672", "foreground": "#F92672" }
        },
        {
            "name": "diff.inserted",
            "scope": "markup.inserted",
            "settings": { "foreground": "#ff42bd" }
        },
        {
            "name": "diff.changed",
            "scope": "markup.changed",
            "settings": { "foreground": "#E6DB74" }
        },
        {
            "scope": "constant.numeric.line-number.find-in-files - match",
            "settings": { "foreground": "#AE81FFA0" }
        },
        {
            "scope": "entity.name.filename.find-in-files",
            "settings": { "foreground": "#E6DB74" }
        },
        {
            "name": "Language methods",
            "scope": ["variable.language"],
            "settings": { "foreground": "#ff42bd" }
        },
        {
            "scope": "heading.1.markdown",
            "settings": { "foreground": "#ff42bd", "fontStyle": "bold" }
        },
        {
            "scope": "punctuation.definition.heading.markdown",
            "settings": { "foreground": "#ff42bd" }
        },
        {
            "scope": "entity.name.section.markdown",
            "settings": { "foreground": "#ff42bd" }
        },
        {
            "scope": "punctuation.definition.list.begin.markdown",
            "settings": { "foreground": "#AE81FF" }
        },
        {
            "scope": "meta.image.inline.markdown",
            "settings": { "foreground": "#E6DB74" }
        },
        {
            "scope": "markup.bold.markdown",
            "settings": { "foreground": "#FFFFFF", "fontStyle": "bold" }
        },
        {
            "scope": "markup.italic.markdown",
            "settings": { "foreground": "#FFFFFF", "fontStyle": "italic" }
        },
        {
            "scope": "markup.inline.raw.string.markdown",
            "settings": { "foreground": "#66D9EF" }
        },
        {
            "scope": "meta.separator.markdown",
            "settings": { "foreground": "#8f8f8f" }
        },
        {
            "scope": [
                "punctuation.definition.template-expression.begin",
                "punctuation.definition.template-expression.end"
            ],
            "settings": { "foreground": "#AE81FF" }
        },
        {
            "scope": [
                "variable.other.object",
                "punctuation.accessor",
                "meta.brace.round",
                "variable.other.readwrite",
                "punctuation.separator.comma"
            ],
            "settings": { "foreground": "#FFFFFF" }
        }
    ],
    "colors": {
        "foreground": "#FFFFFF",
        "icon.foreground": "#ffffff",
        "contrastBorder": "#f82a5d75",
        "contrastActiveBorder": "#00000000",
        "focusBorder": "#f82a5d75",
        "editor.foreground": "#FFFFFF",
        "editor.background": "#000000",
        "editor.inactiveSelectionBackground": "#6688cc",
        "editor.findMatchBackground": "#6688cc",
        "editor.findMatchBorder": "#f82a5d75",
        "editor.selectionBackground": "#6688cc",
        "editor.selectionForeground": "#000c18",
        "editor.lineHighlightBorder": "#f82a5d75",
        "editorLineNumber.foreground": "#b2b2b2",
        "editorLineNumber.activeForeground": "#b2b2b2",
        // "editorLineNumber.foreground": "#f82a5d",
        // "editorLineNumber.activeForeground": "#f82a5d",
        "editorMarkerNavigation.background": "#060621",
        "editorMarkerNavigationError.background": "#AB395B",
        "editorMarkerNavigationWarning.background": "#5B7E7A",
        "editorLink.activeForeground": "#0063a5",
        "editor.findMatchHighlightBackground": "#eeeeee44",
        "terminal.ansiBlack": "#8f8f8f",
        "terminal.ansiBrightBlack": "#8f8f8f",
        "terminal.ansiRed": "#f82a5d",
        "terminal.ansiBrightRed": "#f82a5d",
        "terminal.ansiGreen": "#98d800",
        "terminal.ansiBrightGreen": "#98d800",
        "terminal.ansiYellow": "#e7dc60",
        "terminal.ansiBrightYellow": "#e7dc60",
        "terminal.ansiBlue": "#5ccaef",
        "terminal.ansiBrightBlue": "#5ccaef",
        "terminal.ansiMagenta": "#f82a5d",
        "terminal.ansiBrightMagenta": "#f82a5d",
        "terminal.ansiCyan": "#a57fff",
        "terminal.ansiBrightCyan": "#a57fff",
        "terminal.ansiWhite": "#f1f1f1",
        "terminal.ansiBrightWhite": "#f1f1f1",
        "titleBar.activeForeground": "#f82a5d",
        "editorSuggestWidget.background": "#000000",
        "editorSuggestWidget.foreground": "#FFFFFF",
        "editorSuggestWidget.border": "#f82a5d",
        "editorSuggestWidget.highlightForeground": "#e7dc60",
        "editorSuggestWidget.selectedBackground": "#f82a5d75",
        "editorHoverWidget.background": "#000000",
        "editorHoverWidget.border": "#f82a5d75",
        "editorWidget.background": "#000000",
        "editorWidget.border": "#000000",
        "editorIndentGuide.activeBackground": "#505050",
        "breadcrumb.foreground": "#f82a5d",
        "menu.background": "#000000",
        "menu.border": "#f82a5d75",
        "panel.background": "#000000",
        "panel.dropBorder": "#f82a5d",
        "panel.border": "#f82a5d75",
        "panelTitle.activeForeground": "#f82a5d",
        "panelTitle.inactiveForeground": "#f82a5d75",
        "panelSection.border": "#f82a5d75",
        "sideBar.background": "#000",
        "sideBarSectionHeader.border": "#f82a5d75",
        "sideBar.border": "#f82a5d75",
        "sideBarTitle.foreground": "#f82a5d",
        "sideBar.foreground": "#FFFFFF",
        "sideBarSectionHeader.background": "#000000",
        "sideBarSectionHeader.foreground": "#f82a5d",
        "badge.background": "#000000",
        "badge.foreground": "#f82a5d",
        "activityBar.background": "#000",
        "activityBar.foreground": "#f82a5d",
        "activityBar.border": "#f82a5d75",
        "activityBar.inactiveForeground": "#f82a5d75",
        "activityBarBadge.background": "#000",
        "activityBarBadge.foreground": "#f82a5d",
        "titleBar.activeBackground": "#000000",
        "titleBar.border": "#f82a5d75",
        "titleBar.inactiveBackground": "#000000",
        "titleBar.inactiveForeground": "#f82a5d",
        "tab.activeForeground": "#FFFFFF",
        "tab.inactiveForeground": "#FFFFFF",
        "tab.unfocusedActiveForeground": "#FFFFFF",
        "tab.unfocusedInactiveForeground": "#FFFFFF",
        "tab.activeBackground": "#000000",
        "tab.inactiveBackground": "#000000",
        "tab.unfocusedActiveBackground": "#000000",
        "tab.unfocusedInactiveBackground": "#000000",
        "tab.hoverBackground": "#f82a5d25",
        "tab.unfocusedHoverBackground": "#f82a5d25",
        "tab.border": "#f82a5d75",
        "tab.activeBorder": "#FFFFFF",
        "tab.unfocusedActiveBorder": "#f82a5d",
        "tab.activeModifiedBorder": "#f82a5d75",
        "tab.inactiveModifiedBorder": "#f82a5d75",
        "tab.hoverBorder": "#f82a5d75",
        "editorGroupHeader.tabsBackground": "#000000",
        "editorGroup.border": "#f82a5d75",
        "editorGroupHeader.tabsBorder": "#000000",
        "editorGroupHeader.border": "#000000",
        "editorGroupHeader.noTabsBackground": "#000000",
        "statusBar.background": "#000000",
        "statusBar.foreground": "#f82a5d",
        "statusBar.border": "#f82a5d75",
        "statusBar.noFolderBackground": "#000000",
        "statusBar.noFolderForeground": "#f82a5d",
        "statusBar.debuggingBackground": "#43B9D815",
        "statusBar.debuggingForeground": "#f82a5d",
        "statusBarItem.hoverBackground": "#101010",
        "notificationCenter.border": "#f82a5d",
        "notifications.foreground": "#FFFFFF",
        "notifications.background": "#000000",
        "notifications.border": "#f82a5d75",
        "notificationCenterHeader.background": "#000000",
        "notificationCenterHeader.foreground": "#FFFFFF",
        "editorHint.foreground": "#e7dc60",
        "diffEditor.insertedTextBackground": "#31958A55",
        "diffEditor.insertedTextBorder": "#31958A55",
        "diffEditor.removedTextBackground": "#892F4688",
        "diffEditor.removedTextBorder": "#31958A55",
        "merge.border": "#f82a5d75",
        "peekView.border": "#f82a5d75",
        "peekViewTitle.background": "#000000",
        "peekViewTitleLabel.foreground": "#ffffff",
        "debugExceptionWidget.background": "#000000",
        "debugExceptionWidget.border": "#f82a5d",
        "debugToolBar.background": "#000000",
        "debugToolBar.border": "#f82a5d",
        "button.background": "#000000",
        "button.foreground": "#FFFFFF",
        "button.hoverBackground": "#000000",
        "dropdown.border": "#f82a5d75",
        "dropdown.background": "#000000",
        "dropdown.foreground": "#FFFFFF",
        "input.background": "#000000",
        "input.foreground": "#FFFFFF",
        "input.border": "#f82a5d75",
        "input.placeholderForeground": "#f82a5d75",
        "checkbox.border": "#f82a5d75",
        "quickInput.background": "#000000",
        "list.activeSelectionBackground": "#f82a5d75",
        "list.inactiveSelectionBackground": "#f82a5d15",
        "list.hoverBackground": "#f82a5d75",
        "list.inactiveFocusOutline": "#f82a5d",
        "gitlens.trailingLineBackgroundColor": "#000000",
        "gitlens.trailingLineForegroundColor": "#f82a5d",
        "terminal.border": "#f82a5d75"
    }
}
