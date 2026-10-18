# Chapitre 10 : Formulaires Réactifs (Reactive Forms)

## Introduction

Angular propose trois approches pour gérer les formulaires :
1. **Template-driven forms** : logique dans le template HTML
2. **Reactive forms** : logique dans le code TypeScript (recommandé)
3. **Signal forms** : nouvelle approche expérimentale avec signals

Ce chapitre se concentre sur les **formulaires réactifs**, l'approche la plus puissante et recommandée pour les applications professionnelles.

## Pourquoi les Formulaires Réactifs ?

### Avantages

- ✅ **Testabilité** : logique dans le TypeScript, facile à tester
- ✅ **Validation complexe** : validateurs synchrones et asynchrones
- ✅ **Typage fort** : détection d'erreurs à la compilation
- ✅ **Réactivité** : Observable pour suivre les changements
- ✅ **Dynamisme** : ajouter/supprimer des contrôles facilement
- ✅ **Performances** : meilleur contrôle du change detection

### Comparaison avec Template-Driven

| Aspect | Template-Driven | Reactive Forms |
|--------|----------------|----------------|
| Logique | Dans le HTML | Dans le TypeScript |
| Validation | Attributs HTML | Validators programmatiques |
| Testabilité | Difficile | Facile |
| Complexité | Simple | Avancé |
| Performances | Moins optimisé | Optimisé |

## Concepts Fondamentaux

### FormControl

Un `FormControl` représente un champ de formulaire individuel.

```typescript
import { FormControl } from '@angular/forms';

// Création basique
const nameControl = new FormControl('');

// Avec valeur initiale
const ageControl = new FormControl(25);

// Avec validateurs
const emailControl = new FormControl('', [Validators.required, Validators.email]);

// Avec état disabled
const disabledControl = new FormControl({value: 'test', disabled: true});
```

### FormGroup

Un `FormGroup` regroupe plusieurs `FormControl` en un objet.

```typescript
import { FormGroup, FormControl, Validators } from '@angular/forms';

const userForm = new FormGroup({
    firstname: new FormControl('', Validators.required),
    lastname: new FormControl('', Validators.required),
    email: new FormControl('', [Validators.required, Validators.email]),
    age: new FormControl(null, [Validators.min(18), Validators.max(100)])
});
```

### FormBuilder

`FormBuilder` simplifie la création de formulaires.

```typescript
import { FormBuilder, Validators } from '@angular/forms';
import { inject } from '@angular/core';

export class MyComponent {
    private formBuilder = inject(FormBuilder);
    
    userForm = this.formBuilder.group({
        firstname: ['', Validators.required],
        lastname: ['', Validators.required],
        email: ['', [Validators.required, Validators.email]],
        age: [null, [Validators.min(18), Validators.max(100)]]
    });
}
```

## Configuration de Base

### 1. Import des Modules

```typescript
import { Component, inject } from '@angular/core';
import { FormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';

@Component({
    selector: 'app-user-form',
    imports: [ReactiveFormsModule],  // Important !
    templateUrl: './user-form.html'
})
export class UserFormComponent {
    private formBuilder = inject(FormBuilder);
    
    userForm = this.formBuilder.group({
        firstname: ['', Validators.required],
        email: ['', [Validators.required, Validators.email]]
    });
}
```

### 2. Liaison dans le Template

```html
<form [formGroup]="userForm" (submit)="onSubmit()">
    <div>
        <label>Prénom</label>
        <input formControlName="firstname">
    </div>
    
    <div>
        <label>Email</label>
        <input formControlName="email" type="email">
    </div>
    
    <button type="submit" [disabled]="userForm.invalid">
        Enregistrer
    </button>
</form>
```

## Validation

### Validateurs Intégrés

Angular fournit des validateurs courants :

```typescript
import { Validators } from '@angular/forms';

userForm = this.formBuilder.group({
    // Champ requis
    name: ['', Validators.required],
    
    // Longueur minimale/maximale
    username: ['', [Validators.minLength(3), Validators.maxLength(20)]],
    
    // Valeur minimale/maximale (nombres)
    age: [null, [Validators.min(18), Validators.max(100)]],
    
    // Email
    email: ['', [Validators.required, Validators.email]],
    
    // Pattern (regex)
    phone: ['', Validators.pattern(/^[0-9]{10}$/)],
    
    // Multiple validateurs
    password: ['', [
        Validators.required,
        Validators.minLength(8),
        Validators.pattern(/(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])/)
    ]]
});
```

### Afficher les Erreurs

```html
<div>
    <label>Email</label>
    <input formControlName="email" type="email">
    
    <!-- Affichage des erreurs -->
    @if (userForm.controls.email.invalid && userForm.controls.email.touched) {
        <div class="error">
            @if (userForm.controls.email.hasError('required')) {
                <span>L'email est requis</span>
            }
            @if (userForm.controls.email.hasError('email')) {
                <span>Format d'email invalide</span>
            }
        </div>
    }
</div>
```

### États de Validation

Chaque contrôle a plusieurs états :

```typescript
const emailControl = userForm.get('email');

// États de validité
emailControl.valid       // true si valide
emailControl.invalid     // true si invalide
emailControl.errors      // objet contenant les erreurs

// États d'interaction
emailControl.pristine    // true si jamais modifié
emailControl.dirty       // true si modifié
emailControl.touched     // true si focus puis blur
emailControl.untouched   // true si jamais focus

// État du contrôle
emailControl.disabled    // true si désactivé
emailControl.enabled     // true si activé
```

### Méthode hasError()

```typescript
// Dans le template
@if (userForm.controls.email.hasError('required')) {
    <span>L'email est requis</span>
}

// Dans le TypeScript
onSubmit() {
    if (this.userForm.get('email')?.hasError('required')) {
        console.log('Email manquant');
    }
}
```

## Validateurs Personnalisés

### Validateur Synchrone Simple

```typescript
import { AbstractControl, ValidationErrors, ValidatorFn } from '@angular/forms';

// Fonction qui retourne un validateur
function forbiddenNameValidator(forbiddenName: string): ValidatorFn {
    return (control: AbstractControl): ValidationErrors | null => {
        const forbidden = control.value?.toLowerCase() === forbiddenName.toLowerCase();
        return forbidden ? { forbiddenName: { value: control.value } } : null;
    };
}

// Utilisation
userForm = this.formBuilder.group({
    username: ['', [
        Validators.required,
        forbiddenNameValidator('admin')
    ]]
});
```

### Validateur de Confirmation de Mot de Passe

```typescript
function passwordMatchValidator(): ValidatorFn {
    return (control: AbstractControl): ValidationErrors | null => {
        const password = control.get('password');
        const confirmPassword = control.get('confirmPassword');
        
        if (!password || !confirmPassword) {
            return null;
        }
        
        return password.value === confirmPassword.value 
            ? null 
            : { passwordMismatch: true };
    };
}

// Utilisation sur le FormGroup
registrationForm = this.formBuilder.group({
    password: ['', [Validators.required, Validators.minLength(8)]],
    confirmPassword: ['', Validators.required]
}, {
    validators: passwordMatchValidator()  // Validateur au niveau du groupe
});
```

```html
@if (registrationForm.hasError('passwordMismatch')) {
    <div class="error">Les mots de passe ne correspondent pas</div>
}
```

### Validateur Asynchrone (Vérification Serveur)

```typescript
import { AsyncValidatorFn } from '@angular/forms';
import { Observable, of, delay } from 'rxjs';
import { map } from 'rxjs/operators';

function usernameAvailableValidator(userService: UserService): AsyncValidatorFn {
    return (control: AbstractControl): Observable<ValidationErrors | null> => {
        if (!control.value) {
            return of(null);
        }
        
        return userService.checkUsernameAvailable(control.value).pipe(
            delay(500),  // Délai pour simuler appel réseau
            map(isAvailable => isAvailable ? null : { usernameTaken: true })
        );
    };
}

// Utilisation
username: ['', 
    [Validators.required],  // Validateurs synchrones
    [usernameAvailableValidator(this.userService)]  // Validateur asynchrone
]
```

## Manipulation des Formulaires

### Lire les Valeurs

```typescript
// Valeur complète du formulaire
const formValue = this.userForm.value;
console.log(formValue);  // { firstname: 'John', email: 'john@example.com' }

// Valeur d'un contrôle spécifique
const email = this.userForm.get('email')?.value;

// Valeur brute (inclut les champs disabled)
const rawValue = this.userForm.getRawValue();
```

### Modifier les Valeurs

```typescript
// patchValue : met à jour certains champs
this.userForm.patchValue({
    firstname: 'John'
    // Les autres champs restent inchangés
});

// setValue : met à jour TOUS les champs (erreur si un champ manque)
this.userForm.setValue({
    firstname: 'John',
    lastname: 'Doe',
    email: 'john@example.com',
    age: 30
});

// Modifier un contrôle spécifique
this.userForm.get('email')?.setValue('new@example.com');
```

### Réinitialiser

```typescript
// Réinitialiser tout le formulaire
this.userForm.reset();

// Réinitialiser avec des valeurs par défaut
this.userForm.reset({
    firstname: '',
    email: ''
});

// Réinitialiser un contrôle spécifique
this.userForm.get('email')?.reset();
```

### Activer/Désactiver

```typescript
// Désactiver tout le formulaire
this.userForm.disable();

// Activer tout le formulaire
this.userForm.enable();

// Désactiver un contrôle spécifique
this.userForm.get('email')?.disable();

// Activer un contrôle spécifique
this.userForm.get('email')?.enable();
```

## Réactivité avec valueChanges

### Écouter les Changements

```typescript
import { Subscription } from 'rxjs';

export class MyComponent implements OnDestroy {
    private valueChangesSubscription: Subscription | null = null;
    
    ngOnInit() {
        // Écouter tous les changements du formulaire
        this.valueChangesSubscription = this.userForm.valueChanges.subscribe(value => {
            console.log('Formulaire modifié:', value);
        });
        
        // Écouter un contrôle spécifique
        this.userForm.get('email')?.valueChanges.subscribe(email => {
            console.log('Email modifié:', email);
        });
    }
    
    ngOnDestroy() {
        this.valueChangesSubscription?.unsubscribe();
    }
}
```

### Opérateurs RxJS Utiles

```typescript
import { debounceTime, distinctUntilChanged } from 'rxjs/operators';

ngOnInit() {
    // Attendre 500ms après la dernière frappe
    this.userForm.get('search')?.valueChanges.pipe(
        debounceTime(500),
        distinctUntilChanged()
    ).subscribe(searchTerm => {
        this.search(searchTerm);
    });
}
```

## Exemple Complet : Formulaire CRUD

### Modèle de Données

```typescript
// models/collection-item.ts
export class CollectionItem {
    id: number | null = null;
    name: string = '';
    description: string = '';
    year: number | null = null;
    category: string = '';
    condition: string = 'good';
    imageBase64: string = '';
    
    constructor(data?: Partial<CollectionItem>) {
        Object.assign(this, data);
    }
}
```

### Composant de Formulaire

```typescript
// collection-item-detail.ts
import { Component, inject, input, OnDestroy } from '@angular/core';
import { FormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';
import { Router } from '@angular/router';
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';
import { CollectionService } from '../../services/collection/collection-service';
import { CollectionItem } from '../../models/collection-item';
import { Subscription } from 'rxjs';

@Component({
    selector: 'app-collection-item-detail',
    imports: [
        ReactiveFormsModule,
        MatButtonModule,
        MatInputModule,
        MatSelectModule
    ],
    templateUrl: './collection-item-detail.html',
    styleUrl: './collection-item-detail.scss'
})
export class CollectionItemDetail implements OnDestroy {
    private formBuilder = inject(FormBuilder);
    private collectionService = inject(CollectionService);
    private router = inject(Router);
    
    id = input<string>();
    item: CollectionItem | null = null;
    imagePreview: string | null = null;
    
    private subscriptions = new Subscription();
    
    // Définition du formulaire
    itemFormGroup = this.formBuilder.group({
        name: ['', [Validators.required, Validators.minLength(3)]],
        description: ['', Validators.maxLength(500)],
        year: [null as number | null, [Validators.min(1800), Validators.max(2024)]],
        category: ['', Validators.required],
        condition: ['good', Validators.required]
    });
    
    categories = ['Timbres', 'Pièces', 'Cartes', 'Figurines', 'Autre'];
    conditions = [
        { value: 'mint', label: 'Neuf' },
        { value: 'excellent', label: 'Excellent' },
        { value: 'good', label: 'Bon' },
        { value: 'fair', label: 'Correct' },
        { value: 'poor', label: 'Mauvais' }
    ];
    
    ngOnInit() {
        const itemId = this.id();
        
        if (itemId) {
            // Mode édition : charger l'item existant
            this.item = this.collectionService.getItemById(parseInt(itemId));
            if (this.item) {
                this.itemFormGroup.patchValue({
                    name: this.item.name,
                    description: this.item.description,
                    year: this.item.year,
                    category: this.item.category,
                    condition: this.item.condition
                });
                this.imagePreview = this.item.imageBase64;
            }
        } else {
            // Mode création : nouvel item
            this.item = new CollectionItem();
        }
    }
    
    onFileSelected(event: Event) {
        const input = event.target as HTMLInputElement;
        const file = input.files?.[0];
        
        if (file) {
            // Conversion en Base64
            const reader = new FileReader();
            reader.onload = () => {
                const base64 = reader.result as string;
                this.imagePreview = base64;
                if (this.item) {
                    this.item.imageBase64 = base64;
                }
            };
            reader.readAsDataURL(file);
        }
    }
    
    save() {
        if (this.itemFormGroup.invalid) {
            // Marquer tous les champs comme touched pour afficher les erreurs
            this.itemFormGroup.markAllAsTouched();
            return;
        }
        
        if (this.item) {
            // Mettre à jour l'item avec les valeurs du formulaire
            Object.assign(this.item, this.itemFormGroup.value);
            
            // Sauvegarder
            this.collectionService.saveItem(this.item);
            
            // Rediriger
            this.router.navigate(['/home']);
        }
    }
    
    cancel() {
        this.router.navigate(['/home']);
    }
    
    ngOnDestroy() {
        this.subscriptions.unsubscribe();
    }
}
```

### Template du Formulaire

```html
<!-- collection-item-detail.html -->
<div class="item-detail-container">
    <h2>{{ id() ? 'Modifier' : 'Nouveau' }} Item</h2>
    
    <form [formGroup]="itemFormGroup" (submit)="save()">
        <!-- Nom -->
        <mat-form-field>
            <mat-label>Nom</mat-label>
            <input matInput formControlName="name" required>
            @if (itemFormGroup.controls.name.invalid && 
                  itemFormGroup.controls.name.touched) {
                <mat-error>
                    @if (itemFormGroup.controls.name.hasError('required')) {
                        Le nom est requis
                    }
                    @if (itemFormGroup.controls.name.hasError('minlength')) {
                        Minimum 3 caractères
                    }
                </mat-error>
            }
        </mat-form-field>
        
        <!-- Description -->
        <mat-form-field>
            <mat-label>Description</mat-label>
            <textarea matInput formControlName="description" rows="4"></textarea>
            @if (itemFormGroup.controls.description.hasError('maxlength')) {
                <mat-error>Maximum 500 caractères</mat-error>
            }
        </mat-form-field>
        
        <!-- Année -->
        <mat-form-field>
            <mat-label>Année</mat-label>
            <input matInput type="number" formControlName="year">
            @if (itemFormGroup.controls.year.invalid && 
                  itemFormGroup.controls.year.touched) {
                <mat-error>
                    @if (itemFormGroup.controls.year.hasError('min')) {
                        Année minimale: 1800
                    }
                    @if (itemFormGroup.controls.year.hasError('max')) {
                        Année maximale: 2024
                    }
                </mat-error>
            }
        </mat-form-field>
        
        <!-- Catégorie -->
        <mat-form-field>
            <mat-label>Catégorie</mat-label>
            <mat-select formControlName="category" required>
                @for (cat of categories; track cat) {
                    <mat-option [value]="cat">{{ cat }}</mat-option>
                }
            </mat-select>
            @if (itemFormGroup.controls.category.hasError('required') && 
                  itemFormGroup.controls.category.touched) {
                <mat-error>La catégorie est requise</mat-error>
            }
        </mat-form-field>
        
        <!-- État -->
        <mat-form-field>
            <mat-label>État</mat-label>
            <mat-select formControlName="condition" required>
                @for (cond of conditions; track cond.value) {
                    <mat-option [value]="cond.value">
                        {{ cond.label }}
                    </mat-option>
                }
            </mat-select>
        </mat-form-field>
        
        <!-- Upload d'image -->
        <div class="image-upload">
            <input 
                type="file" 
                #fileInput 
                (change)="onFileSelected($event)"
                accept="image/*"
                hidden>
            <button mat-raised-button type="button" 
                    (click)="fileInput.click()">
                Choisir une image
            </button>
            
            @if (imagePreview) {
                <div class="image-preview">
                    <img [src]="imagePreview" alt="Aperçu">
                </div>
            }
        </div>
        
        <!-- Boutons d'action -->
        <div class="form-actions">
            <button mat-raised-button type="submit" 
                    [disabled]="itemFormGroup.invalid"
                    color="primary">
                Enregistrer
            </button>
            <button mat-button type="button" (click)="cancel()">
                Annuler
            </button>
        </div>
        
        <!-- Debug (à retirer en production) -->
        <div class="debug-info">
            <p>Formulaire valide: {{ itemFormGroup.valid }}</p>
            <p>Formulaire dirty: {{ itemFormGroup.dirty }}</p>
            <p>Valeurs: {{ itemFormGroup.value | json }}</p>
        </div>
    </form>
</div>
```

### Styles

```scss
// collection-item-detail.scss
.item-detail-container {
    max-width: 600px;
    margin: 2rem auto;
    padding: 2rem;
}

form {
    display: flex;
    flex-direction: column;
    gap: 1rem;
}

mat-form-field {
    width: 100%;
}

.image-upload {
    margin: 1rem 0;
}

.image-preview {
    margin-top: 1rem;
    
    img {
        max-width: 300px;
        max-height: 300px;
        border-radius: 8px;
        box-shadow: 0 2px 8px rgba(0,0,0,0.1);
    }
}

.form-actions {
    display: flex;
    gap: 1rem;
    margin-top: 1rem;
}

.debug-info {
    margin-top: 2rem;
    padding: 1rem;
    background: #f5f5f5;
    border-radius: 4px;
    font-size: 0.9rem;
}
```

## FormArray (Formulaires Dynamiques)

Pour gérer une liste dynamique de contrôles :

```typescript
import { FormArray } from '@angular/forms';

skillsForm = this.formBuilder.group({
    skills: this.formBuilder.array([])
});

get skills() {
    return this.skillsForm.get('skills') as FormArray;
}

addSkill() {
    const skillGroup = this.formBuilder.group({
        name: ['', Validators.required],
        level: ['', Validators.required]
    });
    this.skills.push(skillGroup);
}

removeSkill(index: number) {
    this.skills.removeAt(index);
}
```

```html
<div formArrayName="skills">
    @for (skill of skills.controls; track $index) {
        <div [formGroupName]="$index">
            <input formControlName="name" placeholder="Compétence">
            <select formControlName="level">
                <option value="beginner">Débutant</option>
                <option value="intermediate">Intermédiaire</option>
                <option value="expert">Expert</option>
            </select>
            <button type="button" (click)="removeSkill($index)">
                Supprimer
            </button>
        </div>
    }
</div>
<button type="button" (click)="addSkill()">Ajouter une compétence</button>
```

## Bonnes Pratiques

### 1. Grouper les Subscriptions

```typescript
import { Subscription } from 'rxjs';

export class MyComponent implements OnDestroy {
    private subscriptions = new Subscription();
    
    ngOnInit() {
        this.subscriptions.add(
            this.form.valueChanges.subscribe(...)
        );
        
        this.subscriptions.add(
            this.form.statusChanges.subscribe(...)
        );
    }
    
    ngOnDestroy() {
        this.subscriptions.unsubscribe();  // Unsubscribe de tout
    }
}
```

### 2. Typage Fort

```typescript
interface UserFormValue {
    firstname: string;
    lastname: string;
    email: string;
    age: number | null;
}

userForm = this.formBuilder.group<UserFormValue>({
    firstname: ['', Validators.required],
    lastname: ['', Validators.required],
    email: ['', [Validators.required, Validators.email]],
    age: [null, Validators.min(18)]
});
```

### 3. Extraction de Validateurs

```typescript
// validators/custom-validators.ts
export class CustomValidators {
    static positiveNumber(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            return control.value > 0 ? null : { notPositive: true };
        };
    }
    
    static frenchPhoneNumber(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const pattern = /^(?:(?:\+|00)33|0)\s*[1-9](?:[\s.-]*\d{2}){4}$/;
            return pattern.test(control.value) ? null : { invalidPhone: true };
        };
    }
}

// Utilisation
phoneNumber: ['', CustomValidators.frenchPhoneNumber()]
```

### 4. Formulaires Réutilisables

```typescript
// forms/user-form.ts
export function createUserForm(fb: FormBuilder): FormGroup {
    return fb.group({
        firstname: ['', Validators.required],
        lastname: ['', Validators.required],
        email: ['', [Validators.required, Validators.email]]
    });
}

// Utilisation dans plusieurs composants
export class ComponentA {
    form = createUserForm(inject(FormBuilder));
}
```

## Questions d'Entretien Fréquentes

**Q1 : Quelle est la différence entre `patchValue()` et `setValue()` ?**
- `patchValue()` : met à jour certains champs, ignore les champs manquants
- `setValue()` : met à jour TOUS les champs, erreur si un champ manque

**Q2 : Comment créer un validateur personnalisé ?**
Créer une fonction qui retourne `ValidationErrors | null` :
```typescript
function myValidator(): ValidatorFn {
    return (control: AbstractControl): ValidationErrors | null => {
        return isValid ? null : { myError: true };
    };
}
```

**Q3 : Quelle est la différence entre `dirty` et `touched` ?**
- `dirty` : true si la valeur a été modifiée
- `touched` : true si le champ a reçu puis perdu le focus

**Q4 : Comment désactiver la validation d'un formulaire ?**
```typescript
this.form.clearValidators();
this.form.updateValueAndValidity();
```

**Q5 : Comment réinitialiser uniquement les erreurs sans toucher aux valeurs ?**
```typescript
this.form.markAsPristine();
this.form.markAsUntouched();
```

**Q6 : Pourquoi utiliser FormBuilder au lieu de FormGroup directement ?**
FormBuilder simplifie la syntaxe et réduit le boilerplate, rendant le code plus lisible.

**Q7 : Comment gérer les validateurs asynchrones ?**
Passer les validateurs asynchrones en 3ème argument :
```typescript
['', [syncValidators], [asyncValidators]]
```

**Q8 : Comment déboguer un formulaire qui ne se soumet pas ?**
- Vérifier `form.valid`
- Vérifier `form.errors`
- Marquer tous les champs comme touched : `form.markAllAsTouched()`
- Afficher `form.value | json` dans le template

## Résumé

Les formulaires réactifs Angular permettent de :
- ✅ Gérer la validation de manière robuste et testable
- ✅ Créer des formulaires dynamiques complexes
- ✅ Réagir aux changements en temps réel avec RxJS
- ✅ Maintenir un code TypeScript propre et typé
- ✅ Réutiliser des validateurs et des configurations

**Points clés à retenir** :
- Utiliser `FormBuilder` pour simplifier la création
- Importer `ReactiveFormsModule` dans le composant
- Lier avec `[formGroup]` et `formControlName`
- Valider avec `Validators` intégrés ou personnalisés
- Gérer les erreurs avec `hasError()` et les états `touched`/`dirty`
- Toujours faire `unsubscribe()` des Observables dans `ngOnDestroy()`

Dans le prochain chapitre, nous verrons comment intégrer Angular Material pour créer des formulaires avec un design professionnel et cohérent.
