# Guide Flask - Préparation Entretien

## 🎯 Objectifs
- Créer des APIs REST avec Flask
- Gérer les routes et méthodes HTTP (GET, POST, PUT, DELETE)
- Valider et gérer les données JSON
- Authentification et sécurité
- Structurer une application Flask professionnelle
- Tests et déploiement

---

## 1. Introduction à Flask

### Installation et setup de base

```bash
# Installation
pip install flask

# Pour développement complet
pip install flask flask-sqlalchemy flask-jwt-extended flask-cors flask-restful
```

### Application Flask minimale

```python
# app.py
from flask import Flask

app = Flask(__name__)

@app.route('/')
def hello():
    return 'Hello, World!'

if __name__ == '__main__':
    app.run(debug=True)
```

```bash
# Lancer l'application
python app.py

# Ou avec flask run
export FLASK_APP=app.py
export FLASK_ENV=development
flask run
```

### Configuration de l'application

```python
# config.py
import os

class Config:
    """Configuration de base."""
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'dev-secret-key'
    SQLALCHEMY_DATABASE_URI = os.environ.get('DATABASE_URL') or 'sqlite:///app.db'
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    JSON_SORT_KEYS = False

class DevelopmentConfig(Config):
    """Configuration pour développement."""
    DEBUG = True
    TESTING = False

class ProductionConfig(Config):
    """Configuration pour production."""
    DEBUG = False
    TESTING = False

class TestingConfig(Config):
    """Configuration pour tests."""
    TESTING = True
    SQLALCHEMY_DATABASE_URI = 'sqlite:///test.db'

# app.py
from flask import Flask
from config import DevelopmentConfig

app = Flask(__name__)
app.config.from_object(DevelopmentConfig)

# Ou charger depuis fichier
# app.config.from_pyfile('config.py')

# Ou depuis variables d'environnement
# app.config.from_prefixed_env()
```

---

## 2. Routes et Méthodes HTTP

### Routes de base

```python
from flask import Flask, request, jsonify

app = Flask(__name__)

# Route simple (GET par défaut)
@app.route('/')
def index():
    return 'Page d\'accueil'

# Route avec paramètre
@app.route('/user/<username>')
def show_user(username):
    return f'Utilisateur: {username}'

# Route avec paramètre typé
@app.route('/post/<int:post_id>')
def show_post(post_id):
    return f'Post ID: {post_id}'

# Types disponibles:
# string (défaut), int, float, path, uuid

# Route avec plusieurs paramètres
@app.route('/user/<username>/post/<int:post_id>')
def show_user_post(username, post_id):
    return f'Post {post_id} de {username}'

# Paramètres optionnels (query parameters)
@app.route('/search')
def search():
    query = request.args.get('q', '')  # ?q=recherche
    page = request.args.get('page', 1, type=int)  # ?page=2
    return f'Recherche: {query}, Page: {page}'
```

### Méthodes HTTP

```python
from flask import Flask, request, jsonify

app = Flask(__name__)

# Données en mémoire (simuler une base de données)
users = [
    {'id': 1, 'name': 'Alice', 'email': 'alice@example.com'},
    {'id': 2, 'name': 'Bob', 'email': 'bob@example.com'}
]

# GET - Récupérer des données
@app.route('/api/users', methods=['GET'])
def get_users():
    """Liste tous les utilisateurs."""
    return jsonify(users), 200

@app.route('/api/users/<int:user_id>', methods=['GET'])
def get_user(user_id):
    """Récupère un utilisateur par ID."""
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        return jsonify(user), 200
    return jsonify({'error': 'Utilisateur non trouvé'}), 404

# POST - Créer une nouvelle ressource
@app.route('/api/users', methods=['POST'])
def create_user():
    """Crée un nouvel utilisateur."""
    if not request.json:
        return jsonify({'error': 'Pas de données JSON'}), 400
    
    # Validation
    if 'name' not in request.json or 'email' not in request.json:
        return jsonify({'error': 'name et email requis'}), 400
    
    new_user = {
        'id': max([u['id'] for u in users]) + 1 if users else 1,
        'name': request.json['name'],
        'email': request.json['email']
    }
    
    users.append(new_user)
    return jsonify(new_user), 201

# PUT - Mettre à jour complètement une ressource
@app.route('/api/users/<int:user_id>', methods=['PUT'])
def update_user(user_id):
    """Met à jour un utilisateur."""
    user = next((u for u in users if u['id'] == user_id), None)
    if not user:
        return jsonify({'error': 'Utilisateur non trouvé'}), 404
    
    if not request.json:
        return jsonify({'error': 'Pas de données JSON'}), 400
    
    user['name'] = request.json.get('name', user['name'])
    user['email'] = request.json.get('email', user['email'])
    
    return jsonify(user), 200

# PATCH - Mettre à jour partiellement
@app.route('/api/users/<int:user_id>', methods=['PATCH'])
def patch_user(user_id):
    """Met à jour partiellement un utilisateur."""
    user = next((u for u in users if u['id'] == user_id), None)
    if not user:
        return jsonify({'error': 'Utilisateur non trouvé'}), 404
    
    if request.json:
        if 'name' in request.json:
            user['name'] = request.json['name']
        if 'email' in request.json:
            user['email'] = request.json['email']
    
    return jsonify(user), 200

# DELETE - Supprimer une ressource
@app.route('/api/users/<int:user_id>', methods=['DELETE'])
def delete_user(user_id):
    """Supprime un utilisateur."""
    global users
    user = next((u for u in users if u['id'] == user_id), None)
    if not user:
        return jsonify({'error': 'Utilisateur non trouvé'}), 404
    
    users = [u for u in users if u['id'] != user_id]
    return jsonify({'message': 'Utilisateur supprimé'}), 200

# Route qui accepte plusieurs méthodes
@app.route('/api/users/<int:user_id>', methods=['GET', 'PUT', 'DELETE'])
def user_operations(user_id):
    if request.method == 'GET':
        return get_user(user_id)
    elif request.method == 'PUT':
        return update_user(user_id)
    elif request.method == 'DELETE':
        return delete_user(user_id)
```

---

## 3. API REST complète - Exemple CRUD

### Structure du projet

```
my_flask_app/
├── app.py
├── config.py
├── models.py
├── routes/
│   ├── __init__.py
│   ├── users.py
│   └── posts.py
├── utils/
│   ├── __init__.py
│   └── validators.py
├── requirements.txt
└── tests/
    ├── __init__.py
    └── test_api.py
```

### Modèles (avec SQLAlchemy)

```python
# models.py
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime

db = SQLAlchemy()

class User(db.Model):
    """Modèle utilisateur."""
    __tablename__ = 'users'
    
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(80), unique=True, nullable=False)
    email = db.Column(db.String(120), unique=True, nullable=False)
    password_hash = db.Column(db.String(255))
    created_at = db.Column(db.DateTime, default=datetime.utcnow)
    
    # Relations
    posts = db.relationship('Post', backref='author', lazy='dynamic')
    
    def to_dict(self):
        """Convertit en dictionnaire."""
        return {
            'id': self.id,
            'username': self.username,
            'email': self.email,
            'created_at': self.created_at.isoformat(),
            'posts_count': self.posts.count()
        }
    
    def __repr__(self):
        return f'<User {self.username}>'

class Post(db.Model):
    """Modèle article."""
    __tablename__ = 'posts'
    
    id = db.Column(db.Integer, primary_key=True)
    title = db.Column(db.String(200), nullable=False)
    content = db.Column(db.Text, nullable=False)
    user_id = db.Column(db.Integer, db.ForeignKey('users.id'), nullable=False)
    created_at = db.Column(db.DateTime, default=datetime.utcnow)
    updated_at = db.Column(db.DateTime, default=datetime.utcnow, onupdate=datetime.utcnow)
    
    def to_dict(self):
        """Convertit en dictionnaire."""
        return {
            'id': self.id,
            'title': self.title,
            'content': self.content,
            'author': self.author.username,
            'created_at': self.created_at.isoformat(),
            'updated_at': self.updated_at.isoformat()
        }
    
    def __repr__(self):
        return f'<Post {self.title}>'
```

### Routes utilisateurs

```python
# routes/users.py
from flask import Blueprint, request, jsonify
from models import db, User
from werkzeug.security import generate_password_hash

users_bp = Blueprint('users', __name__, url_prefix='/api/users')

@users_bp.route('/', methods=['GET'])
def get_users():
    """Liste tous les utilisateurs avec pagination."""
    page = request.args.get('page', 1, type=int)
    per_page = request.args.get('per_page', 10, type=int)
    
    pagination = User.query.paginate(page=page, per_page=per_page, error_out=False)
    
    return jsonify({
        'users': [user.to_dict() for user in pagination.items],
        'total': pagination.total,
        'pages': pagination.pages,
        'current_page': page
    }), 200

@users_bp.route('/<int:user_id>', methods=['GET'])
def get_user(user_id):
    """Récupère un utilisateur par ID."""
    user = User.query.get(user_id)
    if not user:
        return jsonify({'error': 'Utilisateur non trouvé'}), 404
    
    return jsonify(user.to_dict()), 200

@users_bp.route('/', methods=['POST'])
def create_user():
    """Crée un nouvel utilisateur."""
    data = request.get_json()
    
    # Validation
    if not data or not all(k in data for k in ['username', 'email', 'password']):
        return jsonify({'error': 'Données manquantes'}), 400
    
    # Vérifier si l'utilisateur existe
    if User.query.filter_by(username=data['username']).first():
        return jsonify({'error': 'Username déjà utilisé'}), 409
    
    if User.query.filter_by(email=data['email']).first():
        return jsonify({'error': 'Email déjà utilisé'}), 409
    
    # Créer l'utilisateur
    user = User(
        username=data['username'],
        email=data['email'],
        password_hash=generate_password_hash(data['password'])
    )
    
    db.session.add(user)
    db.session.commit()
    
    return jsonify(user.to_dict()), 201

@users_bp.route('/<int:user_id>', methods=['PUT'])
def update_user(user_id):
    """Met à jour un utilisateur."""
    user = User.query.get(user_id)
    if not user:
        return jsonify({'error': 'Utilisateur non trouvé'}), 404
    
    data = request.get_json()
    
    if 'username' in data:
        # Vérifier si le username est déjà pris
        existing = User.query.filter_by(username=data['username']).first()
        if existing and existing.id != user_id:
            return jsonify({'error': 'Username déjà utilisé'}), 409
        user.username = data['username']
    
    if 'email' in data:
        # Vérifier si l'email est déjà pris
        existing = User.query.filter_by(email=data['email']).first()
        if existing and existing.id != user_id:
            return jsonify({'error': 'Email déjà utilisé'}), 409
        user.email = data['email']
    
    db.session.commit()
    return jsonify(user.to_dict()), 200

@users_bp.route('/<int:user_id>', methods=['DELETE'])
def delete_user(user_id):
    """Supprime un utilisateur."""
    user = User.query.get(user_id)
    if not user:
        return jsonify({'error': 'Utilisateur non trouvé'}), 404
    
    db.session.delete(user)
    db.session.commit()
    
    return jsonify({'message': 'Utilisateur supprimé'}), 200

@users_bp.route('/<int:user_id>/posts', methods=['GET'])
def get_user_posts(user_id):
    """Récupère les posts d'un utilisateur."""
    user = User.query.get(user_id)
    if not user:
        return jsonify({'error': 'Utilisateur non trouvé'}), 404
    
    posts = user.posts.all()
    return jsonify([post.to_dict() for post in posts]), 200
```

### Routes posts

```python
# routes/posts.py
from flask import Blueprint, request, jsonify
from models import db, Post, User

posts_bp = Blueprint('posts', __name__, url_prefix='/api/posts')

@posts_bp.route('/', methods=['GET'])
def get_posts():
    """Liste tous les posts avec filtres et pagination."""
    page = request.args.get('page', 1, type=int)
    per_page = request.args.get('per_page', 10, type=int)
    author = request.args.get('author')  # Filtrer par auteur
    
    query = Post.query
    
    if author:
        user = User.query.filter_by(username=author).first()
        if user:
            query = query.filter_by(user_id=user.id)
    
    pagination = query.order_by(Post.created_at.desc()).paginate(
        page=page, per_page=per_page, error_out=False
    )
    
    return jsonify({
        'posts': [post.to_dict() for post in pagination.items],
        'total': pagination.total,
        'pages': pagination.pages,
        'current_page': page
    }), 200

@posts_bp.route('/<int:post_id>', methods=['GET'])
def get_post(post_id):
    """Récupère un post par ID."""
    post = Post.query.get(post_id)
    if not post:
        return jsonify({'error': 'Post non trouvé'}), 404
    
    return jsonify(post.to_dict()), 200

@posts_bp.route('/', methods=['POST'])
def create_post():
    """Crée un nouveau post."""
    data = request.get_json()
    
    # Validation
    if not data or not all(k in data for k in ['title', 'content', 'user_id']):
        return jsonify({'error': 'Données manquantes'}), 400
    
    # Vérifier que l'utilisateur existe
    user = User.query.get(data['user_id'])
    if not user:
        return jsonify({'error': 'Utilisateur non trouvé'}), 404
    
    # Créer le post
    post = Post(
        title=data['title'],
        content=data['content'],
        user_id=data['user_id']
    )
    
    db.session.add(post)
    db.session.commit()
    
    return jsonify(post.to_dict()), 201

@posts_bp.route('/<int:post_id>', methods=['PUT'])
def update_post(post_id):
    """Met à jour un post."""
    post = Post.query.get(post_id)
    if not post:
        return jsonify({'error': 'Post non trouvé'}), 404
    
    data = request.get_json()
    
    if 'title' in data:
        post.title = data['title']
    if 'content' in data:
        post.content = data['content']
    
    db.session.commit()
    return jsonify(post.to_dict()), 200

@posts_bp.route('/<int:post_id>', methods=['DELETE'])
def delete_post(post_id):
    """Supprime un post."""
    post = Post.query.get(post_id)
    if not post:
        return jsonify({'error': 'Post non trouvé'}), 404
    
    db.session.delete(post)
    db.session.commit()
    
    return jsonify({'message': 'Post supprimé'}), 200

@posts_bp.route('/search', methods=['GET'])
def search_posts():
    """Recherche dans les posts."""
    query = request.args.get('q', '')
    
    if not query:
        return jsonify({'error': 'Paramètre de recherche manquant'}), 400
    
    posts = Post.query.filter(
        Post.title.ilike(f'%{query}%') | Post.content.ilike(f'%{query}%')
    ).all()
    
    return jsonify([post.to_dict() for post in posts]), 200
```

### Application principale

```python
# app.py
from flask import Flask, jsonify
from flask_cors import CORS
from models import db
from routes.users import users_bp
from routes.posts import posts_bp
from config import DevelopmentConfig

def create_app(config_class=DevelopmentConfig):
    """Factory pattern pour créer l'application."""
    app = Flask(__name__)
    app.config.from_object(config_class)
    
    # Initialiser les extensions
    db.init_app(app)
    CORS(app)  # Permet les requêtes cross-origin
    
    # Enregistrer les blueprints
    app.register_blueprint(users_bp)
    app.register_blueprint(posts_bp)
    
    # Route racine
    @app.route('/')
    def index():
        return jsonify({
            'message': 'API Flask',
            'version': '1.0',
            'endpoints': {
                'users': '/api/users',
                'posts': '/api/posts'
            }
        })
    
    # Gestion d'erreurs
    @app.errorhandler(404)
    def not_found(error):
        return jsonify({'error': 'Route non trouvée'}), 404
    
    @app.errorhandler(500)
    def internal_error(error):
        db.session.rollback()
        return jsonify({'error': 'Erreur serveur'}), 500
    
    # Créer les tables
    with app.app_context():
        db.create_all()
    
    return app

if __name__ == '__main__':
    app = create_app()
    app.run(debug=True)
```

---

## 4. Validation et gestion des données

### Validation manuelle

```python
from flask import request, jsonify

@app.route('/api/users', methods=['POST'])
def create_user():
    data = request.get_json()
    
    # Vérifier que les données existent
    if not data:
        return jsonify({'error': 'Pas de données'}), 400
    
    # Vérifier les champs requis
    required_fields = ['username', 'email', 'password']
    for field in required_fields:
        if field not in data:
            return jsonify({'error': f'{field} est requis'}), 400
    
    # Validation du format email
    import re
    email_pattern = r'^[\w\.-]+@[\w\.-]+\.\w+$'
    if not re.match(email_pattern, data['email']):
        return jsonify({'error': 'Email invalide'}), 400
    
    # Validation de la longueur du mot de passe
    if len(data['password']) < 8:
        return jsonify({'error': 'Mot de passe trop court (min 8 caractères)'}), 400
    
    # Créer l'utilisateur...
    return jsonify({'message': 'Utilisateur créé'}), 201
```

### Validation avec Flask-Marshmallow

```python
# schemas.py
from flask_marshmallow import Marshmallow
from marshmallow import fields, validate, ValidationError

ma = Marshmallow()

class UserSchema(ma.Schema):
    """Schema pour valider les données utilisateur."""
    id = fields.Int(dump_only=True)
    username = fields.Str(required=True, validate=validate.Length(min=3, max=80))
    email = fields.Email(required=True)
    password = fields.Str(required=True, load_only=True, validate=validate.Length(min=8))
    created_at = fields.DateTime(dump_only=True)
    
    class Meta:
        fields = ('id', 'username', 'email', 'password', 'created_at')

class PostSchema(ma.Schema):
    """Schema pour valider les données post."""
    id = fields.Int(dump_only=True)
    title = fields.Str(required=True, validate=validate.Length(min=1, max=200))
    content = fields.Str(required=True)
    user_id = fields.Int(required=True)
    created_at = fields.DateTime(dump_only=True)
    updated_at = fields.DateTime(dump_only=True)
    
    class Meta:
        fields = ('id', 'title', 'content', 'user_id', 'created_at', 'updated_at')

# Utilisation
user_schema = UserSchema()
users_schema = UserSchema(many=True)

@app.route('/api/users', methods=['POST'])
def create_user():
    try:
        # Valider les données
        data = user_schema.load(request.json)
    except ValidationError as err:
        return jsonify({'errors': err.messages}), 400
    
    # Créer l'utilisateur avec les données validées
    user = User(**data)
    db.session.add(user)
    db.session.commit()
    
    # Sérialiser la réponse
    return user_schema.dump(user), 201

@app.route('/api/users', methods=['GET'])
def get_users():
    users = User.query.all()
    return users_schema.dump(users), 200
```

---

## 5. Authentification et autorisation

### Authentification basique avec JWT

```python
# Installation: pip install flask-jwt-extended

from flask import Flask, request, jsonify
from flask_jwt_extended import (
    JWTManager, create_access_token, create_refresh_token,
    jwt_required, get_jwt_identity
)
from werkzeug.security import generate_password_hash, check_password_hash
from datetime import timedelta

app = Flask(__name__)
app.config['JWT_SECRET_KEY'] = 'super-secret-key'  # À changer en production
app.config['JWT_ACCESS_TOKEN_EXPIRES'] = timedelta(hours=1)
app.config['JWT_REFRESH_TOKEN_EXPIRES'] = timedelta(days=30)

jwt = JWTManager(app)

# Route de connexion
@app.route('/api/auth/login', methods=['POST'])
def login():
    """Authentifie un utilisateur et retourne un token JWT."""
    data = request.get_json()
    
    if not data or not data.get('username') or not data.get('password'):
        return jsonify({'error': 'Username et password requis'}), 400
    
    # Vérifier les credentials (exemple simplifié)
    user = User.query.filter_by(username=data['username']).first()
    
    if not user or not check_password_hash(user.password_hash, data['password']):
        return jsonify({'error': 'Credentials invalides'}), 401
    
    # Créer les tokens
    access_token = create_access_token(identity=user.id)
    refresh_token = create_refresh_token(identity=user.id)
    
    return jsonify({
        'access_token': access_token,
        'refresh_token': refresh_token,
        'user': user.to_dict()
    }), 200

# Route d'inscription
@app.route('/api/auth/register', methods=['POST'])
def register():
    """Enregistre un nouvel utilisateur."""
    data = request.get_json()
    
    # Validation...
    
    # Créer l'utilisateur
    user = User(
        username=data['username'],
        email=data['email'],
        password_hash=generate_password_hash(data['password'])
    )
    
    db.session.add(user)
    db.session.commit()
    
    # Créer le token
    access_token = create_access_token(identity=user.id)
    
    return jsonify({
        'access_token': access_token,
        'user': user.to_dict()
    }), 201

# Refresh token
@app.route('/api/auth/refresh', methods=['POST'])
@jwt_required(refresh=True)
def refresh():
    """Rafraîchit le token d'accès."""
    current_user_id = get_jwt_identity()
    access_token = create_access_token(identity=current_user_id)
    return jsonify({'access_token': access_token}), 200

# Route protégée
@app.route('/api/protected', methods=['GET'])
@jwt_required()
def protected():
    """Route accessible uniquement avec un token valide."""
    current_user_id = get_jwt_identity()
    user = User.query.get(current_user_id)
    
    return jsonify({
        'message': 'Accès autorisé',
        'user': user.to_dict()
    }), 200

# Route protégée avec vérification d'autorisation
@app.route('/api/users/<int:user_id>', methods=['DELETE'])
@jwt_required()
def delete_user(user_id):
    """Seul l'utilisateur lui-même peut se supprimer."""
    current_user_id = get_jwt_identity()
    
    if current_user_id != user_id:
        return jsonify({'error': 'Non autorisé'}), 403
    
    user = User.query.get(user_id)
    if not user:
        return jsonify({'error': 'Utilisateur non trouvé'}), 404
    
    db.session.delete(user)
    db.session.commit()
    
    return jsonify({'message': 'Utilisateur supprimé'}), 200

# Gestion des erreurs JWT
@jwt.unauthorized_loader
def unauthorized_callback(callback):
    return jsonify({'error': 'Token manquant'}), 401

@jwt.invalid_token_loader
def invalid_token_callback(callback):
    return jsonify({'error': 'Token invalide'}), 401

@jwt.expired_token_loader
def expired_token_callback(jwt_header, jwt_data):
    return jsonify({'error': 'Token expiré'}), 401
```

### Utilisation de l'API avec authentification

```bash
# Inscription
curl -X POST http://localhost:5000/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{"username":"alice","email":"alice@example.com","password":"password123"}'

# Connexion
curl -X POST http://localhost:5000/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"alice","password":"password123"}'

# Réponse:
# {
#   "access_token": "eyJ0eXAiOiJKV1QiLCJhbGc...",
#   "refresh_token": "eyJ0eXAiOiJKV1QiLCJhbGc...",
#   "user": {...}
# }

# Accès à une route protégée
curl -X GET http://localhost:5000/api/protected \
  -H "Authorization: Bearer eyJ0eXAiOiJKV1QiLCJhbGc..."
```

---

## 6. Gestion des erreurs

### Gestionnaires d'erreurs personnalisés

```python
from flask import jsonify

@app.errorhandler(400)
def bad_request(error):
    return jsonify({
        'error': 'Requête invalide',
        'message': str(error)
    }), 400

@app.errorhandler(401)
def unauthorized(error):
    return jsonify({
        'error': 'Non authentifié',
        'message': 'Token d\'authentification requis'
    }), 401

@app.errorhandler(403)
def forbidden(error):
    return jsonify({
        'error': 'Accès interdit',
        'message': 'Vous n\'avez pas les permissions nécessaires'
    }), 403

@app.errorhandler(404)
def not_found(error):
    return jsonify({
        'error': 'Ressource non trouvée',
        'message': str(error)
    }), 404

@app.errorhandler(500)
def internal_error(error):
    db.session.rollback()
    return jsonify({
        'error': 'Erreur serveur',
        'message': 'Une erreur interne s\'est produite'
    }), 500

# Exceptions personnalisées
class APIException(Exception):
    """Exception de base pour l'API."""
    status_code = 400
    
    def __init__(self, message, status_code=None, payload=None):
        super().__init__()
        self.message = message
        if status_code is not None:
            self.status_code = status_code
        self.payload = payload
    
    def to_dict(self):
        rv = dict(self.payload or ())
        rv['message'] = self.message
        return rv

class ValidationError(APIException):
    """Erreur de validation."""
    status_code = 400

class NotFoundError(APIException):
    """Ressource non trouvée."""
    status_code = 404

class UnauthorizedError(APIException):
    """Non autorisé."""
    status_code = 401

@app.errorhandler(APIException)
def handle_api_exception(error):
    response = jsonify(error.to_dict())
    response.status_code = error.status_code
    return response

# Utilisation
@app.route('/api/users/<int:user_id>')
def get_user(user_id):
    user = User.query.get(user_id)
    if not user:
        raise NotFoundError(f'Utilisateur {user_id} non trouvé')
    return jsonify(user.to_dict())
```

---

## 7. Middleware et hooks

### Before/After request

```python
from flask import g
import time

@app.before_request
def before_request():
    """Exécuté avant chaque requête."""
    g.start_time = time.time()
    print(f"Requête: {request.method} {request.path}")

@app.after_request
def after_request(response):
    """Exécuté après chaque requête."""
    if hasattr(g, 'start_time'):
        elapsed = time.time() - g.start_time
        print(f"Temps de réponse: {elapsed:.4f}s")
    
    # Ajouter des headers personnalisés
    response.headers['X-Response-Time'] = f"{elapsed:.4f}"
    return response

@app.teardown_request
def teardown_request(exception=None):
    """Exécuté à la fin de la requête (même en cas d'erreur)."""
    if exception:
        print(f"Erreur: {exception}")
    # Nettoyer les ressources

# Middleware de logging
import logging

@app.before_request
def log_request():
    logger = logging.getLogger(__name__)
    logger.info(f"{request.method} {request.path} - {request.remote_addr}")

# Middleware d'authentification personnalisé
from functools import wraps

def require_auth(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        token = request.headers.get('Authorization')
        if not token:
            return jsonify({'error': 'Token manquant'}), 401
        
        # Vérifier le token...
        
        return f(*args, **kwargs)
    return decorated_function

@app.route('/api/protected')
@require_auth
def protected_route():
    return jsonify({'message': 'Accès autorisé'})
```

---

## 8. CORS (Cross-Origin Resource Sharing)

```python
# Installation: pip install flask-cors

from flask_cors import CORS

# Permettre tous les origins (développement uniquement)
app = Flask(__name__)
CORS(app)

# Configuration spécifique
CORS(app, resources={
    r"/api/*": {
        "origins": ["http://localhost:3000", "https://monsite.com"],
        "methods": ["GET", "POST", "PUT", "DELETE"],
        "allow_headers": ["Content-Type", "Authorization"]
    }
})

# Ou par route
from flask_cors import cross_origin

@app.route('/api/public')
@cross_origin()
def public_api():
    return jsonify({'data': 'accessible'})

# Configuration manuelle
@app.after_request
def after_request(response):
    response.headers.add('Access-Control-Allow-Origin', '*')
    response.headers.add('Access-Control-Allow-Headers', 'Content-Type,Authorization')
    response.headers.add('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE')
    return response
```

---

## 9. Tests

### Tests unitaires avec pytest

```python
# tests/test_api.py
import pytest
from app import create_app
from models import db, User

@pytest.fixture
def app():
    """Crée une instance de l'application pour les tests."""
    app = create_app('testing')
    
    with app.app_context():
        db.create_all()
        yield app
        db.drop_all()

@pytest.fixture
def client(app):
    """Crée un client de test."""
    return app.test_client()

@pytest.fixture
def sample_user(app):
    """Crée un utilisateur de test."""
    user = User(username='testuser', email='test@example.com')
    db.session.add(user)
    db.session.commit()
    return user

def test_get_users(client):
    """Test GET /api/users."""
    response = client.get('/api/users')
    assert response.status_code == 200
    data = response.get_json()
    assert 'users' in data

def test_create_user(client):
    """Test POST /api/users."""
    user_data = {
        'username': 'newuser',
        'email': 'newuser@example.com',
        'password': 'password123'
    }
    
    response = client.post('/api/users', json=user_data)
    assert response.status_code == 201
    data = response.get_json()
    assert data['username'] == 'newuser'

def test_get_user(client, sample_user):
    """Test GET /api/users/<id>."""
    response = client.get(f'/api/users/{sample_user.id}')
    assert response.status_code == 200
    data = response.get_json()
    assert data['username'] == 'testuser'

def test_update_user(client, sample_user):
    """Test PUT /api/users/<id>."""
    update_data = {'email': 'updated@example.com'}
    
    response = client.put(f'/api/users/{sample_user.id}', json=update_data)
    assert response.status_code == 200
    data = response.get_json()
    assert data['email'] == 'updated@example.com'

def test_delete_user(client, sample_user):
    """Test DELETE /api/users/<id>."""
    response = client.delete(f'/api/users/{sample_user.id}')
    assert response.status_code == 200
    
    # Vérifier que l'utilisateur n'existe plus
    response = client.get(f'/api/users/{sample_user.id}')
    assert response.status_code == 404

def test_user_not_found(client):
    """Test utilisateur inexistant."""
    response = client.get('/api/users/9999')
    assert response.status_code == 404

def test_invalid_data(client):
    """Test données invalides."""
    invalid_data = {'username': 'test'}  # Manque email et password
    
    response = client.post('/api/users', json=invalid_data)
    assert response.status_code == 400
```

### Lancer les tests

```bash
# Installer pytest
pip install pytest pytest-cov

# Lancer les tests
pytest

# Avec couverture
pytest --cov=app tests/

# Rapport HTML
pytest --cov=app --cov-report=html tests/
```

---

## 10. Déploiement

### Préparer pour la production

```python
# config.py
import os

class ProductionConfig:
    SECRET_KEY = os.environ.get('SECRET_KEY')
    SQLALCHEMY_DATABASE_URI = os.environ.get('DATABASE_URL')
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    DEBUG = False
    TESTING = False

# wsgi.py
from app import create_app

app = create_app('production')

if __name__ == '__main__':
    app.run()
```

### requirements.txt

```
Flask==3.0.0
Flask-SQLAlchemy==3.0.5
Flask-JWT-Extended==4.5.3
Flask-CORS==4.0.0
Flask-Marshmallow==0.15.0
marshmallow-sqlalchemy==0.29.0
python-dotenv==1.0.0
gunicorn==21.2.0
psycopg2-binary==2.9.9
```

### Variables d'environnement (.env)

```bash
FLASK_APP=app.py
FLASK_ENV=production
SECRET_KEY=votre-clé-secrète-très-longue-et-aléatoire
DATABASE_URL=postgresql://user:password@localhost/dbname
JWT_SECRET_KEY=votre-jwt-secret-key
```

### Lancer avec Gunicorn

```bash
# Installation
pip install gunicorn

# Lancer
gunicorn -w 4 -b 0.0.0.0:5000 wsgi:app

# Ou avec configuration
gunicorn -c gunicorn_config.py wsgi:app
```

```python
# gunicorn_config.py
bind = "0.0.0.0:5000"
workers = 4
threads = 2
timeout = 120
accesslog = "logs/access.log"
errorlog = "logs/error.log"
loglevel = "info"
```

### Docker

```dockerfile
# Dockerfile
FROM python:3.11-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

EXPOSE 5000

CMD ["gunicorn", "-w", "4", "-b", "0.0.0.0:5000", "wsgi:app"]
```

```yaml
# docker-compose.yml
version: '3.8'

services:
  web:
    build: .
    ports:
      - "5000:5000"
    environment:
      - FLASK_ENV=production
      - DATABASE_URL=postgresql://postgres:password@db:5432/myapp
    depends_on:
      - db
  
  db:
    image: postgres:15
    environment:
      - POSTGRES_PASSWORD=password
      - POSTGRES_DB=myapp
    volumes:
      - postgres_data:/var/lib/postgresql/data

volumes:
  postgres_data:
```

---

## 11. Bonnes pratiques

### Structure du projet professionnelle

```
my_flask_api/
├── app/
│   ├── __init__.py          # Factory
│   ├── models/
│   │   ├── __init__.py
│   │   ├── user.py
│   │   └── post.py
│   ├── routes/
│   │   ├── __init__.py
│   │   ├── auth.py
│   │   ├── users.py
│   │   └── posts.py
│   ├── schemas/
│   │   ├── __init__.py
│   │   ├── user.py
│   │   └── post.py
│   ├── utils/
│   │   ├── __init__.py
│   │   ├── validators.py
│   │   └── decorators.py
│   └── config.py
├── tests/
│   ├── __init__.py
│   ├── conftest.py
│   ├── test_auth.py
│   ├── test_users.py
│   └── test_posts.py
├── migrations/              # Flask-Migrate
├── .env
├── .env.example
├── .gitignore
├── requirements.txt
├── wsgi.py
├── Dockerfile
├── docker-compose.yml
└── README.md
```

### Checklist Flask

✅ **Sécurité**
- [ ] Utiliser HTTPS en production
- [ ] SECRET_KEY aléatoire et sécurisée
- [ ] JWT pour authentification
- [ ] Validation des entrées
- [ ] Protection CSRF si formulaires
- [ ] Rate limiting
- [ ] Headers de sécurité

✅ **Performance**
- [ ] Pagination pour grandes listes
- [ ] Caching (Flask-Caching)
- [ ] Compression des réponses
- [ ] Indexation base de données
- [ ] Connection pooling

✅ **Code**
- [ ] Blueprints pour organisation
- [ ] Factory pattern
- [ ] Gestion d'erreurs
- [ ] Logging approprié
- [ ] Tests unitaires
- [ ] Documentation API

✅ **Production**
- [ ] Gunicorn ou uWSGI
- [ ] Variables d'environnement
- [ ] Base de données production
- [ ] Monitoring et logs
- [ ] Backups

---

## 🎯 Points clés pour l'entretien

### Concepts Flask à maîtriser
- **Routes et méthodes HTTP** (GET, POST, PUT, DELETE)
- **request et response** (JSON, headers, query params)
- **Blueprints** pour structurer l'app
- **SQLAlchemy** pour la base de données
- **JWT** pour authentification
- **Validation** des données
- **Gestion d'erreurs** personnalisée
- **CORS** pour APIs
- **Tests** avec pytest

### Questions fréquentes
1. **Différence entre Flask et Django ?**
   - Flask : microframework, minimaliste, flexible
   - Django : framework complet, batteries included
   
2. **Comment sécuriser une API Flask ?**
   - JWT pour authentification
   - Validation des entrées
   - HTTPS uniquement
   - Rate limiting
   - CORS configuré correctement

3. **Comment structurer une grande application Flask ?**
   - Blueprints
   - Factory pattern
   - Séparation models/routes/schemas

4. **Comment gérer les migrations de base de données ?**
   - Flask-Migrate (Alembic)

**Bonne chance pour votre entretien Flask ! 🚀**
