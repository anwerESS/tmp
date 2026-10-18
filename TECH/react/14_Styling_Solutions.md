## [**..**](./00_index.md)

# 🎨 CHAPITRE 14 : Solutions de Styling

## 🎯 INTRODUCTION : POURQUOI TANT DE SOLUTIONS ?

### Le Problème du Styling dans React

**CSS traditionnel :**
```css
/* styles.css */
.button {
  background: blue;
  color: white;
}
```

```javascript
import './styles.css';

function Button() {
  return <button className="button">Click</button>;
}
```

**Problèmes :**
- ❌ **Global scope** : Classes conflictuelles
- ❌ **Pas de co-location** : CSS séparé du component
- ❌ **Pas de type-safety** : Erreurs silencieuses
- ❌ **Dead code** : CSS inutilisé difficile à détecter
- ❌ **Dynamic styling** : Difficile avec inline styles

---

### Les 3 Grandes Approches

| Approche | Exemple | Philosophie |
|----------|---------|-------------|
| **CSS-in-CSS** | CSS Modules | CSS traditionnel avec scoping |
| **CSS-in-JS** | Styled Components | CSS dans JavaScript |
| **Utility-first** | Tailwind CSS | Classes utilitaires composables |

---

## 📦 CSS MODULES

### Concept

**CSS Modules = CSS traditionnel avec scoping automatique**

```css
/* Button.module.css */
.button {
  background: blue;
  color: white;
  padding: 10px 20px;
  border: none;
  border-radius: 4px;
}

.button:hover {
  background: darkblue;
}

.primary {
  background: green;
}

.secondary {
  background: gray;
}
```

```javascript
// Button.jsx
import styles from './Button.module.css';

function Button({ variant = 'primary', children }) {
  return (
    <button className={styles.button + ' ' + styles[variant]}>
      {children}
    </button>
  );
}

// Render :
// <button class="Button_button__1a2b3 Button_primary__4c5d6">
//   Click
// </button>
```

---

### Setup (Vite/CRA)

**Vite :** Fonctionne out-of-the-box avec `.module.css`

**Create React App :** Fonctionne out-of-the-box

**Webpack manuel :**
```javascript
// webpack.config.js
{
  test: /\.module\.css$/,
  use: [
    'style-loader',
    {
      loader: 'css-loader',
      options: {
        modules: true,
      },
    },
  ],
}
```

---

### Exemples Pratiques

#### Exemple 1 : Component Simple

```css
/* Card.module.css */
.card {
  border: 1px solid #ddd;
  border-radius: 8px;
  padding: 20px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.title {
  font-size: 24px;
  font-weight: bold;
  margin-bottom: 10px;
}

.content {
  color: #666;
  line-height: 1.6;
}
```

```javascript
import styles from './Card.module.css';

function Card({ title, content }) {
  return (
    <div className={styles.card}>
      <h2 className={styles.title}>{title}</h2>
      <p className={styles.content}>{content}</p>
    </div>
  );
}
```

---

#### Exemple 2 : Classes Conditionnelles

```javascript
import styles from './Button.module.css';

function Button({ variant, disabled, children }) {
  // ❌ Verbeux
  const className = `${styles.button} ${styles[variant]} ${disabled ? styles.disabled : ''}`;
  
  // ✅ Avec classnames library
  import clsx from 'clsx';
  
  const className = clsx(
    styles.button,
    styles[variant],
    { [styles.disabled]: disabled }
  );
  
  return <button className={className}>{children}</button>;
}
```

---

#### Exemple 3 : Composition de Classes

```css
/* ProductCard.module.css */
.card {
  border: 1px solid #ddd;
  padding: 20px;
}

.image {
  width: 100%;
  height: 200px;
  object-fit: cover;
}

.title {
  font-size: 18px;
  margin: 10px 0;
}

.price {
  font-size: 24px;
  font-weight: bold;
  color: green;
}

.oldPrice {
  text-decoration: line-through;
  color: gray;
  font-size: 16px;
}
```

```javascript
import styles from './ProductCard.module.css';

function ProductCard({ product }) {
  return (
    <div className={styles.card}>
      <img src={product.image} className={styles.image} />
      <h3 className={styles.title}>{product.name}</h3>
      <div>
        <span className={styles.price}>${product.price}</span>
        {product.oldPrice && (
          <span className={styles.oldPrice}>${product.oldPrice}</span>
        )}
      </div>
    </div>
  );
}
```

---

#### Exemple 4 : Global Classes

```css
/* App.module.css */
:global(.container) {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 20px;
}

.header {
  background: blue;
  color: white;
}

/* Combine local et global */
.header :global(.logo) {
  font-size: 24px;
}
```

```javascript
import styles from './App.module.css';

function App() {
  return (
    <div className="container"> {/* Global */}
      <header className={styles.header}> {/* Local */}
        <span className="logo">MyApp</span> {/* Global */}
      </header>
    </div>
  );
}
```

---

### ✅ Avantages CSS Modules

- ✅ **Scoping automatique** : Pas de conflits
- ✅ **Syntax CSS familier** : Pas de nouvelle syntaxe
- ✅ **Performance** : CSS extrait en fichier
- ✅ **Composition** : composes keyword
- ✅ **Works with preprocessors** : SCSS, Less

---

### ❌ Inconvénients CSS Modules

- ❌ **Pas de dynamic styling** : Difficile avec props
- ❌ **Verbosité** : Importer styles partout
- ❌ **No type-safety** : Erreurs de classe silencieuses
- ❌ **className strings** : Pas de autocompletion

---

## 💅 STYLED COMPONENTS

### Concept

**Styled Components = CSS-in-JS avec tagged templates**

```javascript
import styled from 'styled-components';

const Button = styled.button`
  background: blue;
  color: white;
  padding: 10px 20px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  
  &:hover {
    background: darkblue;
  }
`;

function App() {
  return <Button>Click me</Button>;
}
```

---

### Installation

```bash
npm install styled-components
```

**Avec TypeScript :**
```bash
npm install styled-components @types/styled-components
```

---

### Exemples Pratiques

#### Exemple 1 : Component de Base

```javascript
import styled from 'styled-components';

const Card = styled.div`
  border: 1px solid #ddd;
  border-radius: 8px;
  padding: 20px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
`;

const Title = styled.h2`
  font-size: 24px;
  font-weight: bold;
  margin-bottom: 10px;
  color: #333;
`;

const Content = styled.p`
  color: #666;
  line-height: 1.6;
`;

function ProductCard({ title, content }) {
  return (
    <Card>
      <Title>{title}</Title>
      <Content>{content}</Content>
    </Card>
  );
}
```

---

#### Exemple 2 : Props-based Styling

```javascript
const Button = styled.button`
  background: ${props => props.$primary ? 'blue' : 'gray'};
  color: white;
  padding: ${props => props.$size === 'large' ? '15px 30px' : '10px 20px'};
  border: none;
  border-radius: 4px;
  opacity: ${props => props.disabled ? 0.5 : 1};
  cursor: ${props => props.disabled ? 'not-allowed' : 'pointer'};
  
  &:hover {
    background: ${props => props.$primary ? 'darkblue' : 'darkgray'};
  }
`;

// Usage
<Button $primary>Primary</Button>
<Button $size="large">Large</Button>
<Button disabled>Disabled</Button>

// Note : $ prefix = transient props (not passed to DOM)
```

---

#### Exemple 3 : Extending Styles

```javascript
const Button = styled.button`
  padding: 10px 20px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
`;

// Extend Button
const PrimaryButton = styled(Button)`
  background: blue;
  color: white;
`;

const DangerButton = styled(Button)`
  background: red;
  color: white;
`;

const OutlineButton = styled(Button)`
  background: transparent;
  border: 2px solid blue;
  color: blue;
`;
```

---

#### Exemple 4 : Theming

```javascript
import { ThemeProvider } from 'styled-components';

// Theme object
const theme = {
  colors: {
    primary: '#007bff',
    secondary: '#6c757d',
    success: '#28a745',
    danger: '#dc3545',
  },
  spacing: {
    small: '8px',
    medium: '16px',
    large: '24px',
  },
  breakpoints: {
    mobile: '576px',
    tablet: '768px',
    desktop: '992px',
  },
};

// App with theme
function App() {
  return (
    <ThemeProvider theme={theme}>
      <Button>Themed Button</Button>
    </ThemeProvider>
  );
}

// Button using theme
const Button = styled.button`
  background: ${props => props.theme.colors.primary};
  padding: ${props => props.theme.spacing.medium};
  
  @media (max-width: ${props => props.theme.breakpoints.mobile}) {
    padding: ${props => props.theme.spacing.small};
  }
`;
```

---

#### Exemple 5 : Global Styles

```javascript
import { createGlobalStyle } from 'styled-components';

const GlobalStyle = createGlobalStyle`
  * {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
  }
  
  body {
    font-family: 'Arial', sans-serif;
    background: ${props => props.theme.colors.background};
    color: ${props => props.theme.colors.text};
  }
  
  a {
    text-decoration: none;
    color: ${props => props.theme.colors.primary};
  }
`;

function App() {
  return (
    <ThemeProvider theme={theme}>
      <GlobalStyle />
      <div>App content</div>
    </ThemeProvider>
  );
}
```

---

#### Exemple 6 : Nested Styling

```javascript
const Nav = styled.nav`
  background: #333;
  padding: 20px;
  
  /* Nested element */
  ul {
    list-style: none;
    display: flex;
    gap: 20px;
  }
  
  li {
    display: inline;
  }
  
  a {
    color: white;
    text-decoration: none;
    
    &:hover {
      color: lightblue;
    }
    
    &.active {
      font-weight: bold;
      border-bottom: 2px solid white;
    }
  }
`;

function Navigation() {
  return (
    <Nav>
      <ul>
        <li><a href="/" className="active">Home</a></li>
        <li><a href="/about">About</a></li>
        <li><a href="/contact">Contact</a></li>
      </ul>
    </Nav>
  );
}
```

---

#### Exemple 7 : Animations

```javascript
import styled, { keyframes } from 'styled-components';

const fadeIn = keyframes`
  from {
    opacity: 0;
    transform: translateY(20px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
`;

const spin = keyframes`
  from {
    transform: rotate(0deg);
  }
  to {
    transform: rotate(360deg);
  }
`;

const Card = styled.div`
  animation: ${fadeIn} 0.5s ease-in;
`;

const Spinner = styled.div`
  width: 40px;
  height: 40px;
  border: 4px solid #f3f3f3;
  border-top: 4px solid blue;
  border-radius: 50%;
  animation: ${spin} 1s linear infinite;
`;
```

---

#### Exemple 8 : Attrs (Default Props)

```javascript
const Input = styled.input.attrs(props => ({
  type: props.type || 'text',
  placeholder: props.placeholder || 'Enter text...',
}))`
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 16px;
  
  &:focus {
    outline: none;
    border-color: blue;
  }
`;

// Usage
<Input /> // type="text", placeholder="Enter text..."
<Input type="email" placeholder="Enter email" />
```

---

### ✅ Avantages Styled Components

- ✅ **Dynamic styling** : Props-based styling facile
- ✅ **Scoped automatically** : Pas de conflits
- ✅ **Theming** : ThemeProvider puissant
- ✅ **Co-location** : Styles avec component
- ✅ **Dead code elimination** : Styles inutilisés supprimés
- ✅ **Type-safety** : Avec TypeScript

---

### ❌ Inconvénients Styled Components

- ❌ **Runtime overhead** : Styles générés au runtime
- ❌ **Bundle size** : Library + styles en JS
- ❌ **Learning curve** : Nouvelle syntaxe
- ❌ **DevTools** : Classes générées difficiles à débugger
- ❌ **SSR complexity** : Configuration supplémentaire

---

## 🌊 TAILWIND CSS

### Concept

**Tailwind = Utility-first CSS framework**

```javascript
// ❌ CSS traditionnel
<button className="primary-button">Click</button>

.primary-button {
  background: blue;
  color: white;
  padding: 10px 20px;
  border-radius: 4px;
}

// ✅ Tailwind
<button className="bg-blue-500 text-white px-5 py-2 rounded">
  Click
</button>
```

---

### Installation

```bash
npm install -D tailwindcss postcss autoprefixer
npx tailwindcss init -p
```

**Configuration :**
```javascript
// tailwind.config.js
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        primary: '#007bff',
        secondary: '#6c757d',
      },
      spacing: {
        '128': '32rem',
      },
    },
  },
  plugins: [],
}
```

```css
/* index.css */
@tailwind base;
@tailwind components;
@tailwind utilities;
```

---

### Classes de Base

#### Layout
```javascript
// Display
<div className="flex">...</div>
<div className="grid">...</div>
<div className="hidden">...</div>
<div className="block">...</div>

// Flexbox
<div className="flex justify-center items-center gap-4">
  <div>Item 1</div>
  <div>Item 2</div>
</div>

// Grid
<div className="grid grid-cols-3 gap-4">
  <div>1</div>
  <div>2</div>
  <div>3</div>
</div>
```

#### Spacing
```javascript
// Padding: p-{size}
<div className="p-4">Padding 1rem</div>
<div className="px-4 py-2">Padding horizontal 1rem, vertical 0.5rem</div>

// Margin: m-{size}
<div className="m-4">Margin 1rem</div>
<div className="mx-auto">Margin horizontal auto (center)</div>

// Gap
<div className="flex gap-4">Gap 1rem</div>
```

#### Typography
```javascript
<h1 className="text-4xl font-bold text-gray-900">
  Large Bold Title
</h1>

<p className="text-base text-gray-600 leading-relaxed">
  Paragraph text
</p>

<span className="text-sm italic text-blue-500">
  Small italic blue text
</span>
```

#### Colors
```javascript
// Background
<div className="bg-blue-500">...</div>
<div className="bg-gray-100">...</div>

// Text
<p className="text-red-600">Error text</p>
<p className="text-green-500">Success text</p>

// Border
<div className="border border-gray-300">...</div>
<div className="border-2 border-blue-500">...</div>
```

---

### Exemples Pratiques

#### Exemple 1 : Button Component

```javascript
function Button({ variant = 'primary', size = 'medium', children }) {
  const baseStyles = "font-semibold rounded transition-colors duration-200";
  
  const variants = {
    primary: "bg-blue-500 text-white hover:bg-blue-600",
    secondary: "bg-gray-500 text-white hover:bg-gray-600",
    outline: "bg-transparent border-2 border-blue-500 text-blue-500 hover:bg-blue-50",
    danger: "bg-red-500 text-white hover:bg-red-600",
  };
  
  const sizes = {
    small: "px-3 py-1 text-sm",
    medium: "px-5 py-2 text-base",
    large: "px-7 py-3 text-lg",
  };
  
  return (
    <button className={`${baseStyles} ${variants[variant]} ${sizes[size]}`}>
      {children}
    </button>
  );
}

// Usage
<Button variant="primary" size="large">Click me</Button>
<Button variant="outline">Cancel</Button>
```

---

#### Exemple 2 : Card Component

```javascript
function Card({ title, content, image }) {
  return (
    <div className="bg-white rounded-lg shadow-md overflow-hidden hover:shadow-xl transition-shadow duration-300">
      <img 
        src={image} 
        alt={title}
        className="w-full h-48 object-cover"
      />
      <div className="p-6">
        <h3 className="text-xl font-bold text-gray-900 mb-2">
          {title}
        </h3>
        <p className="text-gray-600 leading-relaxed">
          {content}
        </p>
        <button className="mt-4 bg-blue-500 text-white px-4 py-2 rounded hover:bg-blue-600">
          Read More
        </button>
      </div>
    </div>
  );
}
```

---

#### Exemple 3 : Responsive Design

```javascript
function Hero() {
  return (
    <div className="
      px-4 py-8
      md:px-8 md:py-12
      lg:px-16 lg:py-20
      bg-gradient-to-r from-blue-500 to-purple-600
    ">
      <h1 className="
        text-3xl 
        md:text-5xl 
        lg:text-6xl 
        font-bold 
        text-white 
        mb-4
      ">
        Welcome to Our App
      </h1>
      
      <p className="
        text-base 
        md:text-lg 
        lg:text-xl 
        text-white 
        opacity-90
        max-w-2xl
      ">
        Build amazing things with our platform
      </p>
      
      <div className="
        flex 
        flex-col 
        sm:flex-row 
        gap-4 
        mt-8
      ">
        <button className="bg-white text-blue-500 px-6 py-3 rounded">
          Get Started
        </button>
        <button className="border-2 border-white text-white px-6 py-3 rounded">
          Learn More
        </button>
      </div>
    </div>
  );
}

// Breakpoints Tailwind :
// sm: 640px
// md: 768px
// lg: 1024px
// xl: 1280px
// 2xl: 1536px
```

---

#### Exemple 4 : Form avec Tailwind

```javascript
function LoginForm() {
  return (
    <form className="max-w-md mx-auto bg-white p-8 rounded-lg shadow-md">
      <h2 className="text-2xl font-bold text-gray-900 mb-6">
        Login
      </h2>
      
      <div className="mb-4">
        <label className="block text-gray-700 font-semibold mb-2">
          Email
        </label>
        <input
          type="email"
          className="
            w-full 
            px-4 py-2 
            border border-gray-300 
            rounded 
            focus:outline-none 
            focus:ring-2 
            focus:ring-blue-500
            focus:border-transparent
          "
          placeholder="Enter your email"
        />
      </div>
      
      <div className="mb-6">
        <label className="block text-gray-700 font-semibold mb-2">
          Password
        </label>
        <input
          type="password"
          className="
            w-full 
            px-4 py-2 
            border border-gray-300 
            rounded 
            focus:outline-none 
            focus:ring-2 
            focus:ring-blue-500
          "
          placeholder="Enter your password"
        />
      </div>
      
      <button className="
        w-full 
        bg-blue-500 
        text-white 
        py-2 
        rounded 
        font-semibold
        hover:bg-blue-600 
        transition-colors
      ">
        Login
      </button>
    </form>
  );
}
```

---

#### Exemple 5 : Dark Mode

```javascript
// tailwind.config.js
export default {
  darkMode: 'class', // ou 'media'
  // ...
}

// App
function App() {
  const [darkMode, setDarkMode] = useState(false);
  
  return (
    <div className={darkMode ? 'dark' : ''}>
      <div className="min-h-screen bg-white dark:bg-gray-900">
        <nav className="bg-gray-100 dark:bg-gray-800 p-4">
          <h1 className="text-2xl font-bold text-gray-900 dark:text-white">
            My App
          </h1>
          <button
            onClick={() => setDarkMode(!darkMode)}
            className="
              px-4 py-2 
              bg-blue-500 dark:bg-blue-700 
              text-white 
              rounded
            "
          >
            Toggle Dark Mode
          </button>
        </nav>
        
        <main className="p-8">
          <p className="text-gray-700 dark:text-gray-300">
            Content adapts to dark mode
          </p>
        </main>
      </div>
    </div>
  );
}
```

---

#### Exemple 6 : Custom Component avec @apply

```css
/* styles.css */
@layer components {
  .btn {
    @apply px-4 py-2 rounded font-semibold transition-colors;
  }
  
  .btn-primary {
    @apply btn bg-blue-500 text-white hover:bg-blue-600;
  }
  
  .btn-secondary {
    @apply btn bg-gray-500 text-white hover:bg-gray-600;
  }
  
  .card {
    @apply bg-white rounded-lg shadow-md p-6;
  }
}
```

```javascript
function MyComponent() {
  return (
    <div className="card">
      <h2>Card Title</h2>
      <button className="btn-primary">Primary</button>
      <button className="btn-secondary">Secondary</button>
    </div>
  );
}
```

---

### ✅ Avantages Tailwind

- ✅ **Pas de naming** : Pas besoin d'inventer des noms de classes
- ✅ **Consistent design** : Design system intégré
- ✅ **Responsive facile** : Prefixes pour breakpoints
- ✅ **Performance** : PurgeCSS supprime classes inutilisées
- ✅ **Customizable** : Config complète
- ✅ **No context switching** : Styles dans le JSX

---

### ❌ Inconvénients Tailwind

- ❌ **Verbosité** : Classes très longues
- ❌ **HTML pollué** : Moins lisible
- ❌ **Learning curve** : Mémoriser les classes
- ❌ **Non-semantic** : Classes utilitaires partout
- ❌ **Duplication** : Répéter classes sur éléments similaires

---

## 📊 COMPARATIF COMPLET

### Tableau de Comparaison

| Critère | CSS Modules | Styled Components | Tailwind CSS |
|---------|-------------|-------------------|--------------|
| **Scoping** | ✅ Automatic | ✅ Automatic | ❌ Manual |
| **Performance** | ✅ Excellent | ⚠️ Runtime | ✅ Excellent |
| **Bundle size** | ✅ Small | ❌ +15kb | ✅ Small (purged) |
| **Learning curve** | ✅ Easy | ⚠️ Medium | ⚠️ Medium |
| **Dynamic styling** | ❌ Difficult | ✅ Easy | ⚠️ With logic |
| **Theming** | ⚠️ CSS vars | ✅ Built-in | ✅ Config |
| **SSR** | ✅ Easy | ⚠️ Setup needed | ✅ Easy |
| **DevTools** | ✅ Normal classes | ❌ Generated | ✅ Normal classes |
| **Type-safety** | ❌ No | ✅ With TS | ⚠️ With plugins |
| **Co-location** | ❌ Separate files | ✅ Same file | ✅ Inline |

---

### Quand Utiliser Quoi ?

**✅ CSS Modules**
- Vous aimez le CSS traditionnel
- Performance critique
- Team familière avec CSS
- Pas besoin de dynamic styling complexe

**✅ Styled Components**
- Dynamic styling fréquent (props-based)
- Theming complexe
- Strong TypeScript integration
- Component library

**✅ Tailwind CSS**
- Prototypage rapide
- Design system strict
- Responsive design intensif
- Pas envie de nommer des classes

---

## 🎯 QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1 : Pourquoi utiliser CSS Modules au lieu de CSS global ?

**Réponse :**

**Problème CSS global :**
```css
/* button.css */
.button {
  background: blue;
}

/* card.css */
.button {
  background: green; /* ❌ Conflit ! */
}
```

**Solution CSS Modules :**
```css
/* Button.module.css */
.button { background: blue; }

/* Card.module.css */
.button { background: green; }
```

```javascript
import buttonStyles from './Button.module.css';
import cardStyles from './Card.module.css';

// Pas de conflit !
<button className={buttonStyles.button}>Blue</button>
<button className={cardStyles.button}>Green</button>

// Render :
// <button class="Button_button__abc123">Blue</button>
// <button class="Card_button__def456">Green</button>
```

**Avantages :**
- ✅ Scoping automatique
- ✅ Pas de conflits de noms
- ✅ Dead code elimination
- ✅ Syntax CSS familier

---

### Q2 : Comment faire du theming avec Styled Components ?

**Réponse complète :**

```javascript
import styled, { ThemeProvider } from 'styled-components';

// 1. Définir les themes
const lightTheme = {
  colors: {
    background: '#ffffff',
    text: '#000000',
    primary: '#007bff',
  },
  spacing: {
    small: '8px',
    medium: '16px',
  },
};

const darkTheme = {
  colors: {
    background: '#1a1a1a',
    text: '#ffffff',
    primary: '#66b3ff',
  },
  spacing: {
    small: '8px',
    medium: '16px',
  },
};

// 2. Components utilisant le theme
const Container = styled.div`
  background: ${props => props.theme.colors.background};
  color: ${props => props.theme.colors.text};
  padding: ${props => props.theme.spacing.medium};
`;

const Button = styled.button`
  background: ${props => props.theme.colors.primary};
  color: white;
  padding: ${props => props.theme.spacing.small};
`;

// 3. App avec ThemeProvider
function App() {
  const [isDark, setIsDark] = useState(false);
  const theme = isDark ? darkTheme : lightTheme;
  
  return (
    <ThemeProvider theme={theme}>
      <Container>
        <h1>Themed App</h1>
        <Button onClick={() => setIsDark(!isDark)}>
          Toggle Theme
        </Button>
      </Container>
    </ThemeProvider>
  );
}
```

---

### Q3 : Tailwind vs Styled Components : lequel choisir ?

**Réponse comparative :**

**Tailwind CSS :**
```javascript
// ✅ Avantages
// - Prototypage ultra-rapide
// - Design system cohérent
// - Pas de naming fatigue
// - Bundle size optimisé

// ❌ Inconvénients
// - Classes très longues
// - HTML moins lisible
// - Duplication de classes

function Card() {
  return (
    <div className="bg-white rounded-lg shadow-md p-6 hover:shadow-xl transition-shadow">
      <h2 className="text-2xl font-bold text-gray-900 mb-4">Title</h2>
      <p className="text-gray-600">Content</p>
    </div>
  );
}
```

**Styled Components :**
```javascript
// ✅ Avantages
// - Props-based styling facile
// - Logique complexe simple
// - Theming puissant
// - Type-safe avec TS

// ❌ Inconvénients
// - Runtime overhead
// - Bundle size plus gros
// - SSR configuration

const Card = styled.div`
  background: white;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  padding: 24px;
  transition: box-shadow 0.3s;
  
  &:hover {
    box-shadow: 0 8px 16px rgba(0,0,0,0.2);
  }
`;

const Title = styled.h2`
  font-size: 24px;
  font-weight: bold;
  color: #333;
  margin-bottom: 16px;
`;

function CardComponent() {
  return (
    <Card>
      <Title>Title</Title>
      <p>Content</p>
    </Card>
  );
}
```

**Choisir selon le contexte :**

| Projet | Meilleur choix |
|--------|----------------|
| **Landing page** | Tailwind (rapide) |
| **Component library** | Styled Components (flexible) |
| **Dashboard admin** | Tailwind (cohérence) |
| **Design system complexe** | Styled Components (theming) |
| **Prototype MVP** | Tailwind (vitesse) |
| **App avec nombreux states** | Styled Components (props) |

---

### Q4 : Comment optimiser les performances avec CSS-in-JS ?

**Réponse avec techniques :**

**1. Utiliser des transient props ($)**
```javascript
// ❌ MAUVAIS : Tous les props passés au DOM
const Button = styled.button`
  background: ${props => props.primary ? 'blue' : 'gray'};
`;
<Button primary={true}>Click</Button>
// Warning : Unknown prop `primary` on <button>

// ✅ BON : Transient props ($)
const Button = styled.button`
  background: ${props => props.$primary ? 'blue' : 'gray'};
`;
<Button $primary>Click</Button>
// $primary n'est pas passé au DOM
```

**2. Éviter les inline styles dynamiques**
```javascript
// ❌ MAUVAIS : Nouveau component à chaque render
function Card({ bgColor }) {
  const StyledCard = styled.div`
    background: ${bgColor};
  `;
  return <StyledCard>Content</StyledCard>;
}

// ✅ BON : Component défini en dehors
const StyledCard = styled.div`
  background: ${props => props.$bgColor};
`;

function Card({ bgColor }) {
  return <StyledCard $bgColor={bgColor}>Content</StyledCard>;
}
```

**3. Utiliser attrs pour props statiques**
```javascript
// ✅ Optimisé avec attrs
const Input = styled.input.attrs({
  type: 'text',
  autoComplete: 'off',
})`
  padding: 10px;
  border: 1px solid #ddd;
`;
```

**4. SSR avec styled-components**
```javascript
// _document.js (Next.js)
import Document from 'next/document';
import { ServerStyleSheet } from 'styled-components';

export default class MyDocument extends Document {
  static async getInitialProps(ctx) {
    const sheet = new ServerStyleSheet();
    const originalRenderPage = ctx.renderPage;

    try {
      ctx.renderPage = () =>
        originalRenderPage({
          enhanceApp: (App) => (props) =>
            sheet.collectStyles(<App {...props} />),
        });

      const initialProps = await Document.getInitialProps(ctx);
      return {
        ...initialProps,
        styles: (
          <>
            {initialProps.styles}
            {sheet.getStyleElement()}
          </>
        ),
      };
    } finally {
      sheet.seal();
    }
  }
}
```

---

### Q5 : Comment gérer les media queries avec Tailwind ?

**Réponse avec exemples :**

**Approche 1 : Prefixes responsive**
```javascript
<div className="
  text-sm        /* mobile */
  md:text-base   /* >= 768px */
  lg:text-lg     /* >= 1024px */
  xl:text-xl     /* >= 1280px */
">
  Responsive text
</div>

<div className="
  grid 
  grid-cols-1       /* 1 colonne mobile */
  sm:grid-cols-2    /* 2 colonnes tablet */
  lg:grid-cols-3    /* 3 colonnes desktop */
  gap-4
">
  <div>Item 1</div>
  <div>Item 2</div>
  <div>Item 3</div>
</div>
```

**Approche 2 : Custom breakpoints**
```javascript
// tailwind.config.js
export default {
  theme: {
    screens: {
      'xs': '475px',
      'sm': '640px',
      'md': '768px',
      'lg': '1024px',
      'xl': '1280px',
      '2xl': '1536px',
      '3xl': '1920px',
    },
  },
}

// Usage
<div className="xs:text-sm md:text-base 3xl:text-xl">
  Custom breakpoints
</div>
```

**Approche 3 : Mobile-first vs Desktop-first**
```javascript
// Mobile-first (par défaut Tailwind)
<div className="
  text-sm          /* Mobile */
  md:text-base     /* >= Tablet */
  lg:text-lg       /* >= Desktop */
">
  Mobile first
</div>

// Desktop-first (avec max-width)
<div className="
  text-lg          /* Desktop */
  max-lg:text-base /* < 1024px */
  max-md:text-sm   /* < 768px */
">
  Desktop first
</div>
```

**Approche 4 : Visibility breakpoints**
```javascript
<div>
  {/* Visible seulement sur mobile */}
  <nav className="block md:hidden">
    Mobile Menu
  </nav>
  
  {/* Visible seulement sur desktop */}
  <nav className="hidden md:block">
    Desktop Menu
  </nav>
</div>
```

---

### Q6 : Comment partager des styles entre components ?

**Réponse selon l'approche :**

**CSS Modules avec composition**
```css
/* shared.module.css */
.baseButton {
  padding: 10px 20px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

/* Button.module.css */
.button {
  composes: baseButton from './shared.module.css';
  background: blue;
  color: white;
}

.dangerButton {
  composes: baseButton from './shared.module.css';
  background: red;
  color: white;
}
```

**Styled Components avec extension**
```javascript
// shared.js
export const BaseButton = styled.button`
  padding: 10px 20px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
`;

// Button.js
import { BaseButton } from './shared';

export const PrimaryButton = styled(BaseButton)`
  background: blue;
  color: white;
`;

export const DangerButton = styled(BaseButton)`
  background: red;
  color: white;
`;
```

**Tailwind avec @apply**
```css
/* styles.css */
@layer components {
  .btn-base {
    @apply px-5 py-2 rounded cursor-pointer border-none;
  }
  
  .btn-primary {
    @apply btn-base bg-blue-500 text-white;
  }
  
  .btn-danger {
    @apply btn-base bg-red-500 text-white;
  }
}
```

**Tailwind avec component wrapper**
```javascript
// Button.jsx
function Button({ variant = 'primary', children }) {
  const baseClasses = "px-5 py-2 rounded cursor-pointer border-none";
  
  const variants = {
    primary: "bg-blue-500 text-white",
    danger: "bg-red-500 text-white",
    success: "bg-green-500 text-white",
  };
  
  return (
    <button className={`${baseClasses} ${variants[variant]}`}>
      {children}
    </button>
  );
}
```

---

### Q7 : Comment gérer les animations ?

**CSS Modules**
```css
/* Animation.module.css */
@keyframes fadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}

.fadeIn {
  animation: fadeIn 0.5s ease-in;
}

.slideIn {
  animation: slideIn 0.3s ease-out;
}

@keyframes slideIn {
  from { transform: translateX(-100%); }
  to { transform: translateX(0); }
}
```

```javascript
import styles from './Animation.module.css';

function Modal() {
  return <div className={styles.fadeIn}>Modal content</div>;
}
```

**Styled Components**
```javascript
import styled, { keyframes } from 'styled-components';

const fadeIn = keyframes`
  from { opacity: 0; }
  to { opacity: 1; }
`;

const slideIn = keyframes`
  from { transform: translateX(-100%); }
  to { transform: translateX(0); }
`;

const Modal = styled.div`
  animation: ${fadeIn} 0.5s ease-in;
`;

const Sidebar = styled.aside`
  animation: ${slideIn} 0.3s ease-out;
`;
```

**Tailwind**
```javascript
// tailwind.config.js
export default {
  theme: {
    extend: {
      animation: {
        'fade-in': 'fadeIn 0.5s ease-in',
        'slide-in': 'slideIn 0.3s ease-out',
      },
      keyframes: {
        fadeIn: {
          '0%': { opacity: '0' },
          '100%': { opacity: '1' },
        },
        slideIn: {
          '0%': { transform: 'translateX(-100%)' },
          '100%': { transform: 'translateX(0)' },
        },
      },
    },
  },
}

// Usage
<div className="animate-fade-in">Fades in</div>
<div className="animate-slide-in">Slides in</div>

// Built-in animations
<div className="animate-spin">Spinning</div>
<div className="animate-pulse">Pulsing</div>
<div className="animate-bounce">Bouncing</div>
```

---

### Q8 : CSS-in-JS impact sur les performances ?

**Réponse détaillée :**

**Runtime CSS-in-JS (Styled Components) :**
```javascript
// ❌ Overhead au runtime
const Button = styled.button`
  background: blue;
`;

// 1. Parse template literal
// 2. Generate class name
// 3. Inject CSS dans <style> tag
// 4. Render component

// Impact : ~2-5ms par component (négligeable pour petites apps)
```

**Solutions pour optimiser :**

**1. Static extraction (compile-time)**
```bash
# Linaria : CSS-in-JS sans runtime
npm install @linaria/core @linaria/react

# Compile les styles au build
# Performance = CSS Modules
```

**2. CSS Modules (pas de runtime)**
```javascript
// ✅ Styles compilés au build
import styles from './Button.module.css';

// Pas d'overhead runtime
```

**3. Tailwind (pas de runtime)**
```javascript
// ✅ CSS généré au build
<button className="bg-blue-500 px-4 py-2">
  No runtime overhead
</button>
```

**Benchmark (time to interactive) :**
- CSS Modules : **100%** (baseline)
- Tailwind : **102%** (quasi identique)
- Styled Components : **108%** (~8% plus lent)
- Emotion : **105%** (~5% plus lent)

**Conclusion :** CSS-in-JS a un coût mais acceptable pour la plupart des apps.

---

### Q9 : Comment tester des components avec styles ?

**CSS Modules**
```javascript
import { render, screen } from '@testing-library/react';
import Button from './Button';
import styles from './Button.module.css';

test('button has correct class', () => {
  render(<Button>Click</Button>);
  const button = screen.getByText('Click');
  
  expect(button).toHaveClass(styles.button);
});
```

**Styled Components**
```javascript
import { render } from '@testing-library/react';
import 'jest-styled-components'; // Plugin pour tester les styles

const Button = styled.button`
  background: blue;
  color: white;
`;

test('button has correct styles', () => {
  const { container } = render(<Button>Click</Button>);
  const button = container.firstChild;
  
  expect(button).toHaveStyleRule('background', 'blue');
  expect(button).toHaveStyleRule('color', 'white');
});
```

**Tailwind**
```javascript
import { render, screen } from '@testing-library/react';

function Button() {
  return (
    <button className="bg-blue-500 text-white px-4 py-2">
      Click
    </button>
  );
}

test('button has correct classes', () => {
  render(<Button />);
  const button = screen.getByText('Click');
  
  expect(button).toHaveClass('bg-blue-500');
  expect(button).toHaveClass('text-white');
  expect(button).toHaveClass('px-4');
  expect(button).toHaveClass('py-2');
});
```

---

### Q10 : Quelle est la meilleure approche pour un design system ?

**Réponse avec comparaison :**

**Option 1 : Styled Components (Material-UI, Chakra UI)**
```javascript
// ✅ Avantages
// - Props-based variants
// - Theming puissant
// - TypeScript excellent
// - Runtime flexibility

const Button = styled.button`
  ${props => props.theme.typography.button};
  background: ${props => props.theme.colors[props.$variant]};
  padding: ${props => props.theme.spacing[props.$size]};
`;

// Usage
<ThemeProvider theme={customTheme}>
  <Button $variant="primary" $size="large">Click</Button>
</ThemeProvider>
```

**Option 2 : Tailwind (Shadcn/ui, Daisy UI)**
```javascript
// ✅ Avantages
// - Configuration centralisée
// - Consistency forced
// - Performance optimale
// - Facile à override

// tailwind.config.js
export default {
  theme: {
    colors: {
      primary: '#007bff',
      secondary: '#6c757d',
    },
    spacing: {
      sm: '8px',
      md: '16px',
      lg: '24px',
    },
  },
}

// Component
function Button({ variant, size }) {
  return (
    <button className={`bg-${variant} p-${size}`}>
      Click
    </button>
  );
}
```

**Recommandation :**
- **Component library public** → Styled Components (flexibilité)
- **Internal design system** → Tailwind (cohérence + vitesse)
- **Mix des deux** → Tailwind + Headless UI (best of both)

---

## 🔗 LIENS AVEC VOS PROJETS

### fast-react-pizza

**Styling utilisé : Tailwind CSS**

```javascript
// Button component
<button className="
  bg-yellow-400 
  text-stone-800 
  uppercase 
  font-semibold 
  py-3 px-6 
  rounded-full 
  hover:bg-yellow-300 
  transition-colors
">
  Start ordering
</button>

// Pizza item
<div className="
  flex 
  gap-4 
  py-2
">
  <img 
    src={pizza.imageUrl} 
    className="h-24"
  />
  <div className="flex flex-col grow">
    <p className="font-medium">{pizza.name}</p>
    <p className="text-sm text-stone-500">{pizza.ingredients.join(', ')}</p>
  </div>
</div>
```

**Points à mentionner :**
- ✅ "J'ai utilisé Tailwind pour un prototypage rapide"
- ✅ "Classes utilitaires pour responsive design facile"
- ✅ "Configuration custom pour les couleurs du brand"
- ✅ "Hover states et transitions avec Tailwind"

---

### wild-oasis

**Styling utilisé : Styled Components**

```javascript
// Styled components avec theming
const StyledHeader = styled.header`
  background-color: var(--color-grey-0);
  padding: 1.2rem 4.8rem;
  border-bottom: 1px solid var(--color-grey-100);
`;

const StyledSidebar = styled.aside`
  background-color: var(--color-grey-0);
  padding: 3.2rem 2.4rem;
  border-right: 1px solid var(--color-grey-100);
  grid-row: 1 / -1;
`;

// Button variants
const Button = styled.button`
  font-size: 1.4rem;
  padding: 1.2rem 1.6rem;
  border: none;
  border-radius: var(--border-radius-sm);
  
  ${props => props.variation === 'primary' && css`
    background-color: var(--color-brand-600);
    color: var(--color-brand-50);
    
    &:hover {
      background-color: var(--color-brand-700);
    }
  `}
  
  ${props => props.variation === 'secondary' && css`
    background-color: var(--color-grey-0);
    border: 1px solid var(--color-grey-200);
    
    &:hover {
      background-color: var(--color-grey-50);
    }
  `}
`;
```

**Points à mentionner :**
- ✅ "Styled Components pour le theming dark mode"
- ✅ "CSS variables pour les couleurs globales"
- ✅ "Props-based styling pour les variants de Button"
- ✅ "Styled components pour la structure du layout"

---

### worldwise

**Styling : CSS Modules**

```css
/* Map.module.css */
.map {
  flex: 1;
  height: 100%;
  background-color: var(--color-dark-1);
  position: relative;
}

.mapContainer {
  height: 100%;
}

/* City.module.css */
.city {
  padding: 2rem 3rem;
  border-radius: 7px;
  background-color: var(--color-dark-2);
}

.row {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}
```

```javascript
import styles from './City.module.css';

function City() {
  return (
    <div className={styles.city}>
      <div className={styles.row}>
        <h6>City name</h6>
        <h3>{cityName}</h3>
      </div>
    </div>
  );
}
```

---

## 📚 BEST PRACTICES

### 1. Nommer Correctement les Classes

```javascript
// ❌ MAUVAIS
.btn { }
.btn1 { }
.btn-blue { }

// ✅ BON (BEM-style)
.button { }
.button--primary { }
.button--large { }
.button__icon { }
```

---

### 2. Éviter les !important

```css
/* ❌ MAUVAIS */
.button {
  background: blue !important;
}

/* ✅ BON : Augmenter la spécificité */
.container .button {
  background: blue;
}

/* ✅ MEILLEUR : CSS Modules ou CSS-in-JS (scoped) */
```

---

### 3. Mobile-First Responsive

```css
/* ✅ Mobile-first */
.container {
  padding: 10px; /* Mobile */
}

@media (min-width: 768px) {
  .container {
    padding: 20px; /* Tablet */
  }
}

@media (min-width: 1024px) {
  .container {
    padding: 40px; /* Desktop */
  }
}
```

---

### 4. Utilisez CSS Variables

```css
:root {
  --color-primary: #007bff;
  --color-secondary: #6c757d;
  --spacing-sm: 8px;
  --spacing-md: 16px;
  --spacing-lg: 24px;
}

.button {
  background: var(--color-primary);
  padding: var(--spacing-md);
}
```

---

### 5. Organisation des Fichiers

```
src/
├── styles/
│   ├── globals.css          # Reset, variables
│   ├── typography.css       # Font styles
│   └── utilities.css        # Helper classes
├── components/
│   ├── Button/
│   │   ├── Button.jsx
│   │   └── Button.module.css
│   └── Card/
│       ├── Card.jsx
│       └── Card.module.css
```

---

## 📚 RÉCAPITULATIF FINAL

### Choisir sa Solution

**Projet rapide / MVP :**
→ **Tailwind CSS** (vitesse)

**Component library :**
→ **Styled Components** (flexibilité)

**Large app avec team CSS :**
→ **CSS Modules** (familiarité)

**Design system strict :**
→ **Tailwind** (cohérence)

**App avec dynamic theming :**
→ **Styled Components** (ThemeProvider)

---

### Ce qu'il FAUT savoir

**CSS Modules :**
- ✅ Scoping automatique
- ✅ Performance excellente
- ✅ Syntax CSS standard

**Styled Components :**
- ✅ Props-based styling
- ✅ Theming puissant
- ✅ Co-location code/style

**Tailwind CSS :**
- ✅ Prototypage rapide
- ✅ Design system intégré
- ✅ Responsive facile

---

## 🚀 PRÊT POUR LA SUITE ?