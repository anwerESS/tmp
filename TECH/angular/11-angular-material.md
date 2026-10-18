# Chapitre 11 : Angular Material

## Introduction

Angular Material est la bibliothèque officielle de composants UI pour Angular, basée sur Material Design de Google. Elle fournit des composants prêts à l'emploi, accessibles, et personnalisables pour créer des interfaces professionnelles rapidement.

## Pourquoi Angular Material ?

### Avantages

- ✅ **Composants pré-construits** : boutons, formulaires, tables, dialogues, etc.
- ✅ **Design cohérent** : Material Design de Google
- ✅ **Accessibilité (a11y)** : ARIA, navigation clavier intégrée
- ✅ **Responsive** : s'adapte aux différentes tailles d'écran
- ✅ **Thèmes** : personnalisation facile des couleurs
- ✅ **Animations** : transitions fluides intégrées
- ✅ **Documentation** : exemples complets et API claire

### Composants Principaux

- Formulaires : Input, Select, Checkbox, Radio, Slider, Datepicker
- Boutons : Button, FAB, Icon Button, Button Toggle
- Navigation : Toolbar, Menu, Sidenav, Tabs
- Layout : Card, Expansion Panel, Stepper, Divider
- Data : Table, Paginator, Sort, Tree
- Popups : Dialog, Snackbar, Tooltip, Bottom Sheet

## Installation

### Commande d'Installation

```bash
ng add @angular/material
```

Cette commande :
1. Installe `@angular/material`, `@angular/cdk`, `@angular/animations`
2. Configure le projet pour Material
3. Demande de choisir un thème
4. Configure les polices et icônes

### Questions lors de l'Installation

```
? Choose a prebuilt theme name, or "custom" for a custom theme:
  > Indigo/Pink        [ Preview: https://material.angular.io?theme=indigo-pink ]
    Deep Purple/Amber  [ Preview: https://material.angular.io?theme=deeppurple-amber ]
    Pink/Blue Grey     [ Preview: https://material.angular.io?theme=pink-bluegrey ]
    Purple/Green       [ Preview: https://material.angular.io?theme=purple-green ]
    Azure/Blue         [ Preview: https://material.angular.io?theme=azure-blue ]
    Custom

? Set up global Angular Material typography styles? (y/N) y
? Include the Angular animations module? (Y/n) Y
```

**Recommandation** : Choisir "Azure/Blue" pour un thème moderne et professionnel.

## Structure de la Documentation Material

La documentation officielle (https://material.angular.io) est organisée en 4 sections :

### 1. Overview (Vue d'ensemble)

Description générale du composant et exemples d'utilisation.

### 2. API

Liste complète des inputs, outputs, et méthodes du composant.

```typescript
// Exemple pour MatButton
@Input() color: 'primary' | 'accent' | 'warn'
@Input() disabled: boolean
@Input() disableRipple: boolean
```

### 3. Examples (Exemples)

Code complet avec démo interactive.

### 4. Accessibility (Accessibilité)

Guidance pour rendre les composants accessibles.

## Import des Modules

Angular Material utilise des modules standalone. Chaque composant doit être importé individuellement.

```typescript
import { Component } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';

@Component({
    selector: 'app-my-form',
    imports: [
        MatButtonModule,
        MatInputModule,
        MatSelectModule
    ],
    templateUrl: './my-form.html'
})
export class MyFormComponent {
}
```

## MatButton (Boutons)

### Variantes de Boutons

```html
<!-- Bouton plein (filled) -->
<button mat-flat-button>Bouton Filled</button>
<button mat-flat-button color="primary">Primary</button>
<button mat-flat-button color="accent">Accent</button>
<button mat-flat-button color="warn">Danger</button>

<!-- Bouton élevé -->
<button mat-raised-button>Bouton Élevé</button>
<button mat-raised-button color="primary">Primary</button>

<!-- Bouton contour -->
<button mat-stroked-button>Bouton Contour</button>

<!-- Bouton texte simple -->
<button mat-button>Bouton Texte</button>

<!-- Bouton désactivé -->
<button mat-flat-button [disabled]="true">Désactivé</button>

<!-- Bouton avec icône -->
<button mat-icon-button>
    <mat-icon>favorite</mat-icon>
</button>

<!-- FAB (Floating Action Button) -->
<button mat-fab color="primary">
    <mat-icon>add</mat-icon>
</button>

<!-- Mini FAB -->
<button mat-mini-fab color="accent">
    <mat-icon>edit</mat-icon>
</button>
```

### Import Nécessaire

```typescript
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';  // Pour les icônes
```

## MatFormField (Champs de Formulaire)

`mat-form-field` est le conteneur pour les inputs Material.

### Structure de Base

```html
<mat-form-field>
    <mat-label>Email</mat-label>
    <input matInput type="email" placeholder="votre@email.com">
    <mat-hint>Nous ne partagerons jamais votre email</mat-hint>
    <mat-error>Email invalide</mat-error>
</mat-form-field>
```

### Composants d'un mat-form-field

- `<mat-label>` : label du champ
- `matInput` : directive sur l'input
- `<mat-hint>` : texte d'aide sous le champ
- `<mat-error>` : message d'erreur
- `<mat-icon matPrefix>` : icône avant l'input
- `<mat-icon matSuffix>` : icône après l'input

### Exemples Complets

```html
<!-- Input simple -->
<mat-form-field>
    <mat-label>Nom</mat-label>
    <input matInput formControlName="name" required>
    @if (form.controls.name.hasError('required')) {
        <mat-error>Le nom est requis</mat-error>
    }
</mat-form-field>

<!-- Input avec icône -->
<mat-form-field>
    <mat-label>Email</mat-label>
    <mat-icon matPrefix>email</mat-icon>
    <input matInput type="email" formControlName="email">
    <mat-error>Format invalide</mat-error>
</mat-form-field>

<!-- Input mot de passe avec toggle -->
<mat-form-field>
    <mat-label>Mot de passe</mat-label>
    <input matInput [type]="hidePassword ? 'password' : 'text'" 
           formControlName="password">
    <button mat-icon-button matSuffix 
            (click)="hidePassword = !hidePassword"
            type="button">
        <mat-icon>{{ hidePassword ? 'visibility_off' : 'visibility' }}</mat-icon>
    </button>
</mat-form-field>

<!-- Textarea -->
<mat-form-field>
    <mat-label>Description</mat-label>
    <textarea matInput rows="5" formControlName="description"></textarea>
    <mat-hint align="end">{{ description.value.length }} / 500</mat-hint>
</mat-form-field>
```

### Appearance (Apparence)

```html
<!-- Fill (par défaut) -->
<mat-form-field appearance="fill">
    <mat-label>Nom</mat-label>
    <input matInput>
</mat-form-field>

<!-- Outline -->
<mat-form-field appearance="outline">
    <mat-label>Nom</mat-label>
    <input matInput>
</mat-form-field>
```

### Import Nécessaire

```typescript
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';  // Pour les icônes
```

## MatSelect (Liste Déroulante)

```html
<mat-form-field>
    <mat-label>Catégorie</mat-label>
    <mat-select formControlName="category" required>
        <mat-option value="stamps">Timbres</mat-option>
        <mat-option value="coins">Pièces</mat-option>
        <mat-option value="cards">Cartes</mat-option>
        <mat-option value="figures">Figurines</mat-option>
    </mat-select>
    @if (form.controls.category.hasError('required')) {
        <mat-error>La catégorie est requise</mat-error>
    }
</mat-form-field>

<!-- Avec boucle -->
<mat-form-field>
    <mat-label>Pays</mat-label>
    <mat-select formControlName="country">
        @for (country of countries; track country.code) {
            <mat-option [value]="country.code">
                {{ country.name }}
            </mat-option>
        }
    </mat-select>
</mat-form-field>

<!-- Multi-sélection -->
<mat-form-field>
    <mat-label>Tags</mat-label>
    <mat-select formControlName="tags" multiple>
        <mat-option value="rare">Rare</mat-option>
        <mat-option value="vintage">Vintage</mat-option>
        <mat-option value="collection">Collection</mat-option>
    </mat-select>
</mat-form-field>
```

### Import Nécessaire

```typescript
import { MatSelectModule } from '@angular/material/select';
```

## Upload de Fichiers

Material n'a pas de composant d'upload natif, mais on peut le créer facilement.

### Approche Recommandée

```html
<!-- Input file caché -->
<input 
    type="file" 
    #fileInput 
    (change)="onFileSelected($event)"
    accept="image/*"
    hidden>

<!-- Bouton Material qui déclenche l'input -->
<button mat-raised-button type="button" (click)="fileInput.click()">
    <mat-icon>cloud_upload</mat-icon>
    Choisir une image
</button>

<!-- Aperçu de l'image -->
@if (imagePreview) {
    <div class="image-preview">
        <img [src]="imagePreview" alt="Aperçu">
        <button mat-icon-button (click)="removeImage()">
            <mat-icon>close</mat-icon>
        </button>
    </div>
}
```

```typescript
imagePreview: string | null = null;

onFileSelected(event: Event) {
    const input = event.target as HTMLInputElement;
    const file = input.files?.[0];
    
    if (file) {
        const reader = new FileReader();
        reader.onload = () => {
            this.imagePreview = reader.result as string;
        };
        reader.readAsDataURL(file);
    }
}

removeImage() {
    this.imagePreview = null;
}
```

## Autres Composants Courants

### MatCheckbox

```html
<mat-checkbox formControlName="acceptTerms">
    J'accepte les conditions
</mat-checkbox>

<mat-checkbox [checked]="true" [disabled]="false">
    Option cochée
</mat-checkbox>
```

```typescript
import { MatCheckboxModule } from '@angular/material/checkbox';
```

### MatRadioButton

```html
<mat-radio-group formControlName="condition">
    <mat-radio-button value="mint">Neuf</mat-radio-button>
    <mat-radio-button value="good">Bon état</mat-radio-button>
    <mat-radio-button value="fair">Correct</mat-radio-button>
    <mat-radio-button value="poor">Mauvais état</mat-radio-button>
</mat-radio-group>
```

```typescript
import { MatRadioModule } from '@angular/material/radio';
```

### MatSlider

```html
<mat-slider min="0" max="100" step="1">
    <input matSliderThumb formControlName="rating">
</mat-slider>
```

```typescript
import { MatSliderModule } from '@angular/material/slider';
```

### MatDatepicker

```html
<mat-form-field>
    <mat-label>Date d'acquisition</mat-label>
    <input matInput [matDatepicker]="picker" formControlName="date">
    <mat-datepicker-toggle matSuffix [for]="picker"></mat-datepicker-toggle>
    <mat-datepicker #picker></mat-datepicker>
</mat-form-field>
```

```typescript
import { MatDatepickerModule } from '@angular/material/datepicker';
import { MatNativeDateModule } from '@angular/material/core';
```

### MatCard

```html
<mat-card>
    <mat-card-header>
        <mat-card-title>Titre de la Carte</mat-card-title>
        <mat-card-subtitle>Sous-titre</mat-card-subtitle>
    </mat-card-header>
    
    <mat-card-content>
        <p>Contenu de la carte...</p>
    </mat-card-content>
    
    <mat-card-actions>
        <button mat-button>ACTION 1</button>
        <button mat-button>ACTION 2</button>
    </mat-card-actions>
</mat-card>
```

```typescript
import { MatCardModule } from '@angular/material/card';
```

### MatDialog

```typescript
import { MatDialog } from '@angular/material/dialog';
import { inject } from '@angular/core';

export class MyComponent {
    private dialog = inject(MatDialog);
    
    openDialog() {
        const dialogRef = this.dialog.open(MyDialogComponent, {
            width: '400px',
            data: { name: 'John' }
        });
        
        dialogRef.afterClosed().subscribe(result => {
            console.log('Dialog fermé:', result);
        });
    }
}
```

```typescript
// my-dialog.component.ts
import { Component, inject } from '@angular/core';
import { MAT_DIALOG_DATA, MatDialogRef } from '@angular/material/dialog';

@Component({
    template: `
        <h2 mat-dialog-title>Confirmation</h2>
        <mat-dialog-content>
            Êtes-vous sûr de vouloir supprimer {{ data.name }} ?
        </mat-dialog-content>
        <mat-dialog-actions>
            <button mat-button (click)="onCancel()">Annuler</button>
            <button mat-flat-button color="warn" (click)="onConfirm()">
                Supprimer
            </button>
        </mat-dialog-actions>
    `
})
export class MyDialogComponent {
    data = inject(MAT_DIALOG_DATA);
    dialogRef = inject(MatDialogRef<MyDialogComponent>);
    
    onCancel() {
        this.dialogRef.close(false);
    }
    
    onConfirm() {
        this.dialogRef.close(true);
    }
}
```

```typescript
import { MatDialogModule } from '@angular/material/dialog';
```

### MatSnackBar (Toast/Notification)

```typescript
import { MatSnackBar } from '@angular/material/snack-bar';
import { inject } from '@angular/core';

export class MyComponent {
    private snackBar = inject(MatSnackBar);
    
    showNotification() {
        this.snackBar.open('Item sauvegardé !', 'Fermer', {
            duration: 3000,
            horizontalPosition: 'end',
            verticalPosition: 'top'
        });
    }
}
```

```typescript
import { MatSnackBarModule } from '@angular/material/snack-bar';
```

### MatTable

```html
<table mat-table [dataSource]="items">
    <!-- Colonne Nom -->
    <ng-container matColumnDef="name">
        <th mat-header-cell *matHeaderCellDef>Nom</th>
        <td mat-cell *matCellDef="let item">{{ item.name }}</td>
    </ng-container>
    
    <!-- Colonne Année -->
    <ng-container matColumnDef="year">
        <th mat-header-cell *matHeaderCellDef>Année</th>
        <td mat-cell *matCellDef="let item">{{ item.year }}</td>
    </ng-container>
    
    <!-- Colonne Actions -->
    <ng-container matColumnDef="actions">
        <th mat-header-cell *matHeaderCellDef>Actions</th>
        <td mat-cell *matCellDef="let item">
            <button mat-icon-button (click)="edit(item)">
                <mat-icon>edit</mat-icon>
            </button>
            <button mat-icon-button (click)="delete(item)">
                <mat-icon>delete</mat-icon>
            </button>
        </td>
    </ng-container>
    
    <tr mat-header-row *matHeaderRowDef="displayedColumns"></tr>
    <tr mat-row *matRowDef="let row; columns: displayedColumns;"></tr>
</table>
```

```typescript
displayedColumns: string[] = ['name', 'year', 'actions'];
items = [
    { name: 'Timbre rare', year: 1920 },
    { name: 'Pièce ancienne', year: 1850 }
];
```

```typescript
import { MatTableModule } from '@angular/material/table';
```

## Personnalisation avec Theming

### Component Tokens (Variables CSS)

Material utilise des variables CSS pour la personnalisation.

```scss
// styles.scss ou component.scss
@use '@angular/material' as mat;

.custom-button {
    // Couleurs système Material
    background-color: var(--mat-sys-primary);
    color: var(--mat-sys-on-primary);
    
    &.danger {
        background-color: var(--mat-sys-error);
        color: var(--mat-sys-on-error);
    }
}

// Personnaliser un composant spécifique
button.mat-mdc-raised-button {
    --mdc-protected-button-container-height: 48px;
    border-radius: 12px;
}
```

### Variables CSS Courantes

```scss
// Couleurs primaires
--mat-sys-primary
--mat-sys-on-primary
--mat-sys-primary-container

// Couleurs d'accent
--mat-sys-secondary
--mat-sys-on-secondary

// Couleurs d'erreur
--mat-sys-error
--mat-sys-on-error

// Surface et fond
--mat-sys-surface
--mat-sys-on-surface
--mat-sys-background
```

### Thème Personnalisé Global

```scss
// styles.scss
@use '@angular/material' as mat;

// Définir une palette personnalisée
$my-primary: mat.define-palette(mat.$indigo-palette);
$my-accent: mat.define-palette(mat.$pink-palette, A200, A100, A400);
$my-warn: mat.define-palette(mat.$red-palette);

// Créer le thème
$my-theme: mat.define-light-theme((
  color: (
    primary: $my-primary,
    accent: $my-accent,
    warn: $my-warn,
  ),
  typography: mat.define-typography-config(),
  density: 0,
));

// Appliquer le thème
@include mat.all-component-themes($my-theme);
```

## Exemple Complet : Formulaire Material

```typescript
// collection-item-form.component.ts
import { Component, inject } from '@angular/core';
import { FormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatSelectModule } from '@angular/material/select';
import { MatIconModule } from '@angular/material/icon';
import { MatCardModule } from '@angular/material/card';
import { MatCheckboxModule } from '@angular/material/checkbox';

@Component({
    selector: 'app-collection-item-form',
    imports: [
        ReactiveFormsModule,
        MatButtonModule,
        MatInputModule,
        MatFormFieldModule,
        MatSelectModule,
        MatIconModule,
        MatCardModule,
        MatCheckboxModule
    ],
    templateUrl: './collection-item-form.html',
    styleUrl: './collection-item-form.scss'
})
export class CollectionItemFormComponent {
    private formBuilder = inject(FormBuilder);
    
    itemForm = this.formBuilder.group({
        name: ['', [Validators.required, Validators.minLength(3)]],
        category: ['', Validators.required],
        year: [null as number | null, [Validators.min(1800)]],
        condition: ['good'],
        description: ['', Validators.maxLength(500)],
        forSale: [false]
    });
    
    categories = ['Timbres', 'Pièces', 'Cartes', 'Figurines'];
    conditions = [
        { value: 'mint', label: 'Neuf' },
        { value: 'excellent', label: 'Excellent' },
        { value: 'good', label: 'Bon' },
        { value: 'fair', label: 'Correct' },
        { value: 'poor', label: 'Mauvais' }
    ];
    
    onSubmit() {
        if (this.itemForm.valid) {
            console.log('Formulaire valide:', this.itemForm.value);
        } else {
            this.itemForm.markAllAsTouched();
        }
    }
}
```

```html
<!-- collection-item-form.html -->
<mat-card>
    <mat-card-header>
        <mat-card-title>Nouvel Item de Collection</mat-card-title>
    </mat-card-header>
    
    <mat-card-content>
        <form [formGroup]="itemForm" (submit)="onSubmit()">
            <!-- Nom -->
            <mat-form-field appearance="outline">
                <mat-label>Nom</mat-label>
                <mat-icon matPrefix>label</mat-icon>
                <input matInput formControlName="name" required>
                @if (itemForm.controls.name.invalid && 
                      itemForm.controls.name.touched) {
                    <mat-error>
                        @if (itemForm.controls.name.hasError('required')) {
                            Le nom est requis
                        }
                        @if (itemForm.controls.name.hasError('minlength')) {
                            Minimum 3 caractères
                        }
                    </mat-error>
                }
            </mat-form-field>
            
            <!-- Catégorie -->
            <mat-form-field appearance="outline">
                <mat-label>Catégorie</mat-label>
                <mat-select formControlName="category" required>
                    @for (cat of categories; track cat) {
                        <mat-option [value]="cat">{{ cat }}</mat-option>
                    }
                </mat-select>
                @if (itemForm.controls.category.hasError('required') && 
                      itemForm.controls.category.touched) {
                    <mat-error>La catégorie est requise</mat-error>
                }
            </mat-form-field>
            
            <!-- Année -->
            <mat-form-field appearance="outline">
                <mat-label>Année</mat-label>
                <mat-icon matPrefix>calendar_today</mat-icon>
                <input matInput type="number" formControlName="year">
                @if (itemForm.controls.year.hasError('min')) {
                    <mat-error>Année minimale: 1800</mat-error>
                }
            </mat-form-field>
            
            <!-- État -->
            <mat-form-field appearance="outline">
                <mat-label>État</mat-label>
                <mat-select formControlName="condition">
                    @for (cond of conditions; track cond.value) {
                        <mat-option [value]="cond.value">
                            {{ cond.label }}
                        </mat-option>
                    }
                </mat-select>
            </mat-form-field>
            
            <!-- Description -->
            <mat-form-field appearance="outline">
                <mat-label>Description</mat-label>
                <textarea matInput rows="4" formControlName="description">
                </textarea>
                <mat-hint align="end">
                    {{ itemForm.controls.description.value?.length || 0 }} / 500
                </mat-hint>
            </mat-form-field>
            
            <!-- À vendre -->
            <mat-checkbox formControlName="forSale">
                Item à vendre
            </mat-checkbox>
            
            <!-- Boutons -->
            <div class="form-actions">
                <button mat-flat-button color="primary" type="submit"
                        [disabled]="itemForm.invalid">
                    <mat-icon>save</mat-icon>
                    Enregistrer
                </button>
                <button mat-stroked-button type="button">
                    Annuler
                </button>
            </div>
        </form>
    </mat-card-content>
</mat-card>
```

```scss
// collection-item-form.scss
mat-card {
    max-width: 600px;
    margin: 2rem auto;
}

form {
    display: flex;
    flex-direction: column;
    gap: 1rem;
}

mat-form-field {
    width: 100%;
}

.form-actions {
    display: flex;
    gap: 1rem;
    margin-top: 1rem;
    
    button {
        flex: 1;
    }
}
```

## Icônes Material

### Configuration

```html
<!-- index.html -->
<link href="https://fonts.googleapis.com/icon?family=Material+Icons" 
      rel="stylesheet">
```

### Utilisation

```html
<!-- Icône simple -->
<mat-icon>home</mat-icon>

<!-- Icône avec couleur -->
<mat-icon color="primary">favorite</mat-icon>

<!-- Icône dans un bouton -->
<button mat-icon-button>
    <mat-icon>delete</mat-icon>
</button>

<!-- Icône avec taille personnalisée -->
<mat-icon style="font-size: 48px;">star</mat-icon>
```

### Icônes Courantes

- **Actions** : add, edit, delete, save, close, check, clear
- **Navigation** : home, menu, arrow_back, arrow_forward, expand_more
- **Fichiers** : folder, insert_drive_file, cloud_upload, cloud_download
- **Communication** : email, phone, chat, notifications
- **Média** : play_arrow, pause, stop, volume_up, image
- **Social** : favorite, share, person, group

Liste complète : https://fonts.google.com/icons

## Bonnes Pratiques

### 1. Import Groupé

```typescript
// material.imports.ts
export const MATERIAL_IMPORTS = [
    MatButtonModule,
    MatInputModule,
    MatFormFieldModule,
    MatSelectModule,
    MatIconModule,
    MatCardModule
];

// component.ts
import { MATERIAL_IMPORTS } from './material.imports';

@Component({
    imports: [ReactiveFormsModule, ...MATERIAL_IMPORTS]
})
```

### 2. Apparence Cohérente

Utiliser la même `appearance` pour tous les `mat-form-field` :

```html
<!-- Option 1: Définir globalement dans styles.scss -->
mat-form-field {
    &.mat-mdc-form-field {
        // Styles globaux
    }
}

<!-- Option 2: Utiliser un wrapper component -->
```

### 3. Gestion des Erreurs

Toujours afficher les erreurs de manière cohérente :

```html
@if (control.invalid && control.touched) {
    <mat-error>
        @if (control.hasError('required')) {
            Ce champ est requis
        }
        @if (control.hasError('email')) {
            Format email invalide
        }
    </mat-error>
}
```

### 4. Responsive Design

```scss
mat-form-field {
    width: 100%;
    
    @media (min-width: 768px) {
        width: 48%;
    }
}
```

## Questions d'Entretien Fréquentes

**Q1 : Quelle est la différence entre Material et Bootstrap ?**
- Material : composants Angular natifs, intégration profonde, animations
- Bootstrap : framework CSS générique, nécessite des wrappers Angular

**Q2 : Comment installer Angular Material ?**
```bash
ng add @angular/material
```

**Q3 : Qu'est-ce que CDK (Component Dev Kit) ?**
Le CDK fournit des comportements réutilisables (drag-drop, overlay, a11y) sans styles.

**Q4 : Comment personnaliser les couleurs Material ?**
Via les thèmes SCSS ou les variables CSS (`--mat-sys-primary`, etc.)

**Q5 : Pourquoi utiliser `mat-form-field` ?**
Encapsule label, hint, error, prefix/suffix de manière cohérente et accessible.

**Q6 : Comment créer un thème Material personnalisé ?**
Avec `define-palette()` et `define-light-theme()` dans un fichier SCSS.

**Q7 : Les composants Material sont-ils accessibles ?**
Oui, ils suivent les standards ARIA et sont testés pour l'accessibilité.

**Q8 : Comment déboguer les erreurs Material ?**
- Vérifier que tous les modules sont importés
- Vérifier que `@angular/animations` est installé
- Regarder les erreurs de console

## Résumé

Angular Material permet de :
- ✅ Créer des interfaces professionnelles rapidement
- ✅ Avoir un design cohérent basé sur Material Design
- ✅ Garantir l'accessibilité (a11y)
- ✅ Personnaliser facilement les thèmes et couleurs
- ✅ Utiliser des composants prêts et bien documentés

**Points clés à retenir** :
- Installer avec `ng add @angular/material`
- Importer chaque module Material individuellement
- Utiliser `mat-form-field` pour tous les champs de formulaire
- Choisir l'`appearance` ("fill" ou "outline") de manière cohérente
- Consulter la documentation officielle pour les API complètes
- Personnaliser avec les variables CSS ou les thèmes SCSS

Dans le prochain chapitre, nous verrons comment implémenter l'authentification avec login, interceptors et guards pour sécuriser l'application.
