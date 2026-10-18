# SOAP — Cours Complet pour Entretien Java / Spring Boot

---

## Table des matières

1. [Introduction à SOAP](#1-introduction-à-soap)
2. [Architecture SOAP](#2-architecture-soap)
3. [Structure d'un message SOAP](#3-structure-dun-message-soap)
4. [WSDL — Web Services Description Language](#4-wsdl--web-services-description-language)
5. [XSD — XML Schema Definition](#5-xsd--xml-schema-definition)
6. [SOAP vs REST](#6-soap-vs-rest)
7. [Avantages et Inconvénients de SOAP](#7-avantages-et-inconvénients-de-soap)
8. [SOAP avec Spring Boot — Spring-WS](#8-soap-avec-spring-boot--spring-ws)
9. [Sécurité SOAP — WS-Security](#9-sécurité-soap--ws-security)
10. [Gestion des erreurs — SOAP Fault](#10-gestion-des-erreurs--soap-fault)
11. [Questions / Réponses orales](#11-questions--réponses-orales)

---

## 1. Introduction à SOAP

**SOAP** (Simple Object Access Protocol) est un protocole de communication basé sur **XML** permettant l'échange de messages structurés entre applications distribuées sur un réseau. Il a été standardisé par le **W3C** et est indépendant du langage de programmation et du système d'exploitation.

### Caractéristiques clés

- **Protocol-agnostic** : fonctionne sur HTTP, HTTPS, SMTP, TCP, etc.
- **Fortement typé** : les messages sont validés contre un schéma **XSD**.
- **Basé sur XML** : format verbeux mais standardisé.
- **Contrat-first** : le service est défini par un fichier **WSDL** avant toute implémentation.
- **Stateful ou Stateless** : selon l'implémentation.

### Historique

| Année | Étape |
|-------|-------|
| 1998 | Conception chez Microsoft |
| 2000 | Première spécification SOAP 1.1 |
| 2003 | SOAP 1.2 (recommandation W3C) |
| Aujourd'hui | Encore dominant dans les systèmes bancaires, assurances, télécoms |

---

## 2. Architecture SOAP

SOAP s'appuie sur un modèle **client / service** et repose sur trois acteurs fondamentaux dans l'architecture **SOA** (Service-Oriented Architecture) :

```
┌─────────────────────────────────────────────────────┐
│                   UDDI Registry                     │
│          (annuaire de services - optionnel)         │
└──────────────┬──────────────────────────────────────┘
               │  publish / find
    ┌──────────▼──────────┐       bind        ┌──────────────────┐
    │   Service Provider  │◄──────────────────│  Service Consumer│
    │  (expose le WSDL)   │                   │   (le client)    │
    └─────────────────────┘                   └──────────────────┘
```

- **Service Provider** : publie le service et expose son **WSDL**.
- **Service Consumer** : consomme le service en envoyant des messages SOAP.
- **UDDI** (Universal Description, Discovery and Integration) : annuaire optionnel pour découvrir les services (peu utilisé aujourd'hui).

### Transport

SOAP utilise principalement **HTTP/HTTPS** comme protocole de transport. Le message SOAP est envoyé dans le **body** d'une requête HTTP POST.

---

## 3. Structure d'un message SOAP

Un message SOAP est un document **XML** composé de trois parties :

```
┌──────────────────────────────────┐
│         SOAP Envelope            │  ← Racine obligatoire
│  ┌────────────────────────────┐  │
│  │       SOAP Header          │  │  ← Optionnel (sécurité, auth, trace)
│  └────────────────────────────┘  │
│  ┌────────────────────────────┐  │
│  │        SOAP Body           │  │  ← Obligatoire (données métier)
│  │  ┌──────────────────────┐  │  │
│  │  │    SOAP Fault        │  │  │  ← Optionnel (erreurs)
│  │  └──────────────────────┘  │  │
│  └────────────────────────────┘  │
└──────────────────────────────────┘
```

### Exemple — Requête SOAP (GetClient)

```xml
<soapenv:Envelope
    xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:cli="http://example.com/client">

    <soapenv:Header>
        <!-- Métadonnées : authentification, traçabilité, etc. -->
        <wsse:Security xmlns:wsse="http://...">
            <wsse:UsernameToken>
                <wsse:Username>anouar</wsse:Username>
                <wsse:Password>secret</wsse:Password>
            </wsse:UsernameToken>
        </wsse:Security>
    </soapenv:Header>

    <soapenv:Body>
        <cli:GetClientRequest>
            <cli:clientId>12345</cli:clientId>
        </cli:GetClientRequest>
    </soapenv:Body>

</soapenv:Envelope>
```

### Exemple — Réponse SOAP

```xml
<soapenv:Envelope
    xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:cli="http://example.com/client">

    <soapenv:Body>
        <cli:GetClientResponse>
            <cli:clientId>12345</cli:clientId>
            <cli:nom>Dupont</cli:nom>
            <cli:email>dupont@mail.com</cli:email>
        </cli:GetClientResponse>
    </soapenv:Body>

</soapenv:Envelope>
```

### Différence SOAP 1.1 vs SOAP 1.2

| Aspect | SOAP 1.1 | SOAP 1.2 |
|--------|----------|----------|
| Namespace | `http://schemas.xmlsoap.org/soap/envelope/` | `http://www.w3.org/2003/05/soap-envelope` |
| SOAPAction | Header HTTP obligatoire | Optionnel (dans Content-Type) |
| Content-Type | `text/xml` | `application/soap+xml` |
| Fault | `faultcode` / `faultstring` | `Code` / `Reason` / `Detail` |

---

## 4. WSDL — Web Services Description Language

Le **WSDL** est un fichier **XML** qui décrit contractuellement un service web SOAP. C'est le **contrat** entre le producteur et le consommateur du service.

### Structure d'un fichier WSDL

```
WSDL
├── <types>         → Définition des types de données (XSD)
├── <message>       → Définition des messages entrants/sortants
├── <portType>      → Définition des opérations (interface abstraite)
├── <binding>       → Liaison entre opérations et protocole (SOAP/HTTP)
└── <service>       → URL d'accès concrète au service (endpoint)
```

### Exemple complet WSDL

```xml
<definitions name="ClientService"
    targetNamespace="http://example.com/client"
    xmlns="http://schemas.xmlsoap.org/wsdl/"
    xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"
    xmlns:tns="http://example.com/client"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema">

    <!-- 1. TYPES : référence au XSD -->
    <types>
        <xsd:schema targetNamespace="http://example.com/client">
            <xsd:element name="GetClientRequest">
                <xsd:complexType>
                    <xsd:sequence>
                        <xsd:element name="clientId" type="xsd:int"/>
                    </xsd:sequence>
                </xsd:complexType>
            </xsd:element>
            <xsd:element name="GetClientResponse">
                <xsd:complexType>
                    <xsd:sequence>
                        <xsd:element name="clientId" type="xsd:int"/>
                        <xsd:element name="nom" type="xsd:string"/>
                        <xsd:element name="email" type="xsd:string"/>
                    </xsd:sequence>
                </xsd:complexType>
            </xsd:element>
        </xsd:schema>
    </types>

    <!-- 2. MESSAGES -->
    <message name="GetClientInput">
        <part name="parameters" element="tns:GetClientRequest"/>
    </message>
    <message name="GetClientOutput">
        <part name="parameters" element="tns:GetClientResponse"/>
    </message>

    <!-- 3. PORT TYPE (interface abstraite) -->
    <portType name="ClientPortType">
        <operation name="GetClient">
            <input message="tns:GetClientInput"/>
            <output message="tns:GetClientOutput"/>
        </operation>
    </portType>

    <!-- 4. BINDING (liaison SOAP) -->
    <binding name="ClientBinding" type="tns:ClientPortType">
        <soap:binding style="document" transport="http://schemas.xmlsoap.org/soap/http"/>
        <operation name="GetClient">
            <soap:operation soapAction="http://example.com/client/GetClient"/>
            <input><soap:body use="literal"/></input>
            <output><soap:body use="literal"/></output>
        </operation>
    </binding>

    <!-- 5. SERVICE (endpoint concret) -->
    <service name="ClientService">
        <port name="ClientPort" binding="tns:ClientBinding">
            <soap:address location="http://localhost:8080/ws/clients"/>
        </port>
    </service>

</definitions>
```

### Styles WSDL

| Style | Description | Usage recommandé |
|-------|-------------|-----------------|
| **Document / Literal** | Le body contient directement l'élément XSD | ✅ Standard actuel (WS-I Basic Profile) |
| **RPC / Encoded** | Paramètres encodés selon des règles SOAP | ❌ Déprécié |
| **RPC / Literal** | Paramètres littéraux, nom de l'opération présent | ⚠️ Rare |

> **En pratique** : toujours utiliser **Document / Literal Wrapped** avec Spring-WS.

---

## 5. XSD — XML Schema Definition

Le **XSD** (XML Schema Definition) définit la structure et les types de données des messages SOAP. Il joue le rôle de **contrat de données**.

### Rôle dans SOAP

```
WSDL <types> → référence le XSD → valide les messages XML
```

### Exemple XSD

```xml
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"
           targetNamespace="http://example.com/client"
           elementFormDefault="qualified">

    <!-- Type simple avec contrainte -->
    <xs:simpleType name="EmailType">
        <xs:restriction base="xs:string">
            <xs:pattern value="[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}"/>
        </xs:restriction>
    </xs:simpleType>

    <!-- Type complexe -->
    <xs:complexType name="ClientType">
        <xs:sequence>
            <xs:element name="clientId"  type="xs:int"/>
            <xs:element name="nom"       type="xs:string" minOccurs="1" maxOccurs="1"/>
            <xs:element name="email"     type="tns:EmailType" minOccurs="0"/>
            <xs:element name="actif"     type="xs:boolean" default="true"/>
        </xs:sequence>
    </xs:complexType>

    <!-- Éléments racines -->
    <xs:element name="GetClientRequest">
        <xs:complexType>
            <xs:sequence>
                <xs:element name="clientId" type="xs:int"/>
            </xs:sequence>
        </xs:complexType>
    </xs:element>

    <xs:element name="GetClientResponse">
        <xs:complexType>
            <xs:sequence>
                <xs:element name="client" type="tns:ClientType"/>
            </xs:sequence>
        </xs:complexType>
    </xs:element>

</xs:schema>
```

### Types XSD courants

| Type XSD | Équivalent Java |
|----------|----------------|
| `xs:string` | `String` |
| `xs:int` | `int` / `Integer` |
| `xs:long` | `long` / `Long` |
| `xs:boolean` | `boolean` / `Boolean` |
| `xs:decimal` | `BigDecimal` |
| `xs:date` | `LocalDate` |
| `xs:dateTime` | `LocalDateTime` |
| `xs:base64Binary` | `byte[]` |

---

## 6. SOAP vs REST

### Tableau comparatif

| Critère | SOAP | REST |
|---------|------|------|
| **Protocole** | Protocole strict (règles formelles) | Style architectural (pas un protocole) |
| **Format** | XML uniquement | JSON, XML, YAML, texte brut… |
| **Transport** | HTTP, SMTP, JMS, TCP | HTTP uniquement |
| **Contrat** | WSDL obligatoire | OpenAPI/Swagger (optionnel) |
| **Typage** | Fortement typé (XSD) | Faiblement typé |
| **Sécurité** | WS-Security (standard riche) | HTTPS + OAuth2 + JWT |
| **Transactions** | WS-AtomicTransaction (ACID) | Non natif |
| **Performance** | Lent (XML verbeux, parsing coûteux) | Rapide (JSON léger) |
| **Couplage** | Fort (changement WSDL = impact client) | Faible |
| **Gestion d'erreur** | SOAP Fault structuré | Codes HTTP (400, 404, 500…) |
| **Stateful** | Supporté nativement | Stateless par définition |
| **Usage typique** | Banque, assurance, télécoms, legacy | APIs modernes, microservices, mobile |

### Quand choisir SOAP ?

- Système legacy qui expose déjà un **WSDL**
- Besoin de **transactions distribuées** (WS-AtomicTransaction)
- Sécurité avancée au niveau message (**WS-Security**, signature XML)
- Contrat **strict et immuable** entre équipes

### Quand choisir REST ?

- APIs publiques ou mobiles
- Microservices avec **couplage faible**
- Performance critique (JSON vs XML)
- Équipes agiles avec évolution fréquente du contrat

---

## 7. Avantages et Inconvénients de SOAP

### ✅ Avantages

| Avantage | Détail |
|----------|--------|
| **Contrat formel (WSDL)** | Garantit la compatibilité entre client et serveur |
| **Validation automatique (XSD)** | Les messages sont validés avant traitement |
| **Sécurité avancée (WS-Security)** | Chiffrement, signature, gestion des tokens au niveau du message |
| **Transactions (WS-AtomicTransaction)** | Support des transactions ACID distribuées |
| **Fiabilité (WS-ReliableMessaging)** | Garantie de livraison des messages |
| **Interopérabilité** | Fonctionne entre langages différents (Java, .NET, Python…) |
| **Neutral au transport** | HTTP, SMTP, JMS, TCP |

### ❌ Inconvénients

| Inconvénient | Détail |
|--------------|--------|
| **Verbosité XML** | Messages lourds → performance dégradée |
| **Complexité** | WSDL + XSD + WS-* standards difficiles à maîtriser |
| **Couplage fort** | Un changement de WSDL peut casser les clients |
| **Pas adapté mobile** | Trop lourd pour les applications mobiles |
| **Outillage lourd** | Nécessite des frameworks spécifiques (JAX-WS, Spring-WS) |
| **Courbe d'apprentissage** | Plus difficile à prendre en main que REST |

---

## 8. SOAP avec Spring Boot — Spring-WS

**Spring-WS** (Spring Web Services) est le module Spring dédié au développement de services web **SOAP**. Il favorise l'approche **Contract-First** (WSDL/XSD en premier).

### Dépendance Maven

```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-web-services</artifactId>
</dependency>
<!-- Pour la génération des classes Java depuis XSD -->
<dependency>
    <groupId>org.glassfish.jaxb</groupId>
    <artifactId>jaxb-runtime</artifactId>
</dependency>
```

### Plugin Maven — génération des classes depuis XSD

```xml
<plugin>
    <groupId>org.codehaus.mojo</groupId>
    <artifactId>jaxb2-maven-plugin</artifactId>
    <version>3.1.0</version>
    <executions>
        <execution>
            <goals><goal>xjc</goal></goals>
        </execution>
    </executions>
    <configuration>
        <sources>
            <source>src/main/resources/xsd/clients.xsd</source>
        </sources>
    </configuration>
</plugin>
```

> Le plugin **XJC** génère automatiquement les classes Java (`GetClientRequest`, `GetClientResponse`, etc.) depuis le fichier XSD.

### Configuration Spring-WS

```java
@EnableWs
@Configuration
public class WebServiceConfig extends WsConfigurerAdapter {

    @Bean
    public ServletRegistrationBean<MessageDispatcherServlet> messageDispatcherServlet(
            ApplicationContext context) {
        MessageDispatcherServlet servlet = new MessageDispatcherServlet();
        servlet.setApplicationContext(context);
        servlet.setTransformWsdlLocations(true);
        return new ServletRegistrationBean<>(servlet, "/ws/*");
    }

    @Bean(name = "clients")
    public DefaultWsdl11Definition defaultWsdl11Definition(XsdSchema clientsSchema) {
        DefaultWsdl11Definition definition = new DefaultWsdl11Definition();
        definition.setPortTypeName("ClientsPort");
        definition.setLocationUri("/ws");
        definition.setTargetNamespace("http://example.com/client");
        definition.setSchema(clientsSchema);
        return definition;
    }

    @Bean
    public XsdSchema clientsSchema() {
        return new SimpleXsdSchema(new ClassPathResource("xsd/clients.xsd"));
    }
}
```

> Le **WSDL** est automatiquement généré et exposé à l'URL : `http://localhost:8080/ws/clients.wsdl`

### Implémentation du Endpoint

```java
@Endpoint
public class ClientEndpoint {

    private static final String NAMESPACE_URI = "http://example.com/client";

    private final ClientService clientService;

    public ClientEndpoint(ClientService clientService) {
        this.clientService = clientService;
    }

    @PayloadRoot(namespace = NAMESPACE_URI, localPart = "GetClientRequest")
    @ResponsePayload
    public GetClientResponse getClient(@RequestPayload GetClientRequest request) {
        GetClientResponse response = new GetClientResponse();
        Client client = clientService.findById(request.getClientId());
        response.setClientId(client.getId());
        response.setNom(client.getNom());
        response.setEmail(client.getEmail());
        return response;
    }
}
```

### Client SOAP avec Spring-WS (WebServiceTemplate)

```java
@Service
public class ClientSoapClient extends WebServiceGatewaySupport {

    public GetClientResponse getClient(int clientId) {
        GetClientRequest request = new GetClientRequest();
        request.setClientId(clientId);

        return (GetClientResponse) getWebServiceTemplate()
                .marshalSendAndReceive(
                    "http://localhost:8080/ws",
                    request,
                    new SoapActionCallback("http://example.com/client/GetClient")
                );
    }
}
```

### Architecture Contract-First dans Spring-WS

```
src/main/resources/
    └── xsd/
        └── clients.xsd          ← Contrat de données (XSD)
                ↓
    [jaxb2-maven-plugin XJC]
                ↓
    target/generated-sources/
        └── GetClientRequest.java
        └── GetClientResponse.java
                ↓
    @Endpoint → @PayloadRoot → logique métier
                ↓
    WSDL auto-généré par DefaultWsdl11Definition
```

---

## 9. Sécurité SOAP — WS-Security

**WS-Security** (WSS) est un standard OASIS qui ajoute la sécurité au niveau du message SOAP (pas seulement au niveau du transport comme HTTPS).

### Mécanismes de sécurité

| Mécanisme | Description |
|-----------|-------------|
| **UsernameToken** | Login/password dans le Header SOAP |
| **X.509 Certificate** | Authentification par certificat numérique |
| **XML Signature** | Signature numérique du message (intégrité) |
| **XML Encryption** | Chiffrement du body SOAP (confidentialité) |
| **SAML Token** | Authentification fédérée (SSO) |

### Avantage clé de WS-Security vs HTTPS

Avec **HTTPS**, la sécurité s'applique au **transport** uniquement. Si le message passe par un intermédiaire (proxy, ESB), il peut être déchiffré à chaque nœud.

Avec **WS-Security**, la sécurité est au niveau du **message XML** lui-même : le message reste chiffré/signé de bout en bout, même s'il transite par plusieurs intermédiaires.

```
Client → [Proxy A] → [ESB] → [Proxy B] → Serveur
          ↑             ↑          ↑
     HTTPS déchiffre  HTTPS     HTTPS
     WS-Security ✅   garde le  message
                      message   chiffré
```

### Implémentation avec Spring-WS + Wss4j

```java
@Bean
public Wss4jSecurityInterceptor securityInterceptor() {
    Wss4jSecurityInterceptor interceptor = new Wss4jSecurityInterceptor();
    interceptor.setValidationActions("UsernameToken");
    interceptor.setValidationCallbackHandler(new SimplePasswordValidationCallbackHandler());
    return interceptor;
}
```

---

## 10. Gestion des erreurs — SOAP Fault

En SOAP, les erreurs sont retournées via un élément **`<Fault>`** dans le **`<Body>`** du message.

### Structure SOAP Fault (SOAP 1.1)

```xml
<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/">
    <soapenv:Body>
        <soapenv:Fault>
            <faultcode>soapenv:Server</faultcode>
            <faultstring>Client introuvable avec l'id 999</faultstring>
            <detail>
                <errorCode>CLIENT_NOT_FOUND</errorCode>
            </detail>
        </soapenv:Fault>
    </soapenv:Body>
</soapenv:Envelope>
```

### Codes de Fault standards

| FaultCode | Signification |
|-----------|--------------|
| `soap:Client` | Erreur côté client (données invalides, requête mal formée) |
| `soap:Server` | Erreur côté serveur (exception métier, NPE…) |
| `soap:VersionMismatch` | Version SOAP incompatible |
| `soap:MustUnderstand` | Header obligatoire non compris |

### Gestion dans Spring-WS

```java
@Endpoint
public class ClientEndpoint {

    @PayloadRoot(namespace = NAMESPACE_URI, localPart = "GetClientRequest")
    @ResponsePayload
    public GetClientResponse getClient(@RequestPayload GetClientRequest request)
            throws Exception {

        Client client = clientService.findById(request.getClientId());

        if (client == null) {
            throw new SoapFaultException("Client introuvable avec l'id " + request.getClientId());
        }

        // ...
    }
}
```

```java
// Mapping des exceptions en SOAP Fault
@SoapFault(faultCode = FaultCode.SERVER)
public class ClientNotFoundException extends RuntimeException {
    public ClientNotFoundException(String message) {
        super(message);
    }
}
```

---

## 11. Questions / Réponses orales

---

### Q1 — Qu'est-ce que SOAP et en quoi diffère-t-il de REST ?

> **SOAP** est un **protocole** de communication basé sur XML avec des règles strictes définies par le W3C. Il utilise un **contrat formel** (le WSDL) et supporte des standards avancés comme WS-Security et WS-AtomicTransaction.
>
> **REST** est un **style architectural** qui utilise les verbes HTTP (GET, POST, PUT, DELETE) et privilégie le format JSON. Il est plus léger, plus simple et mieux adapté aux APIs modernes et aux microservices.
>
> En résumé : SOAP est privilégié dans les systèmes **legacy**, bancaires ou d'assurance où le contrat est strict et la sécurité au niveau message est requise. REST est préféré pour les APIs publiques et les microservices.

---

### Q2 — À quoi sert le fichier WSDL ?

> Le **WSDL** (Web Services Description Language) est le **contrat d'interface** d'un service SOAP. Il décrit :
> - Les **opérations** disponibles (méthodes),
> - Les **messages** entrants et sortants (types de données),
> - Le **protocole de binding** (SOAP sur HTTP),
> - L'**URL d'accès** au service (endpoint).
>
> À partir du WSDL, un client peut générer automatiquement ses classes de proxy (stubs) sans connaître les détails d'implémentation du service.

---

### Q3 — Quelle est la différence entre XSD et WSDL ?

> Le **XSD** définit la structure et les types des données métier (les éléments XML). Le **WSDL** utilise le XSD pour définir les messages et les opérations du service.
>
> On peut dire que le XSD est le **dictionnaire** (les types de données) et le WSDL est le **contrat** (les opérations et les règles d'appel). Dans Spring-WS, on commence toujours par écrire le XSD, puis le WSDL est généré automatiquement.

---

### Q4 — Qu'est-ce que l'approche Contract-First ?

> Dans l'approche **Contract-First**, on définit d'abord le **WSDL et le XSD** (le contrat) avant d'écrire la moindre ligne de code Java. Les classes Java sont ensuite **générées automatiquement** à partir du XSD (avec le plugin JAXB/XJC).
>
> C'est l'approche recommandée avec **Spring-WS** car elle garantit un contrat stable entre le producteur et le consommateur du service, indépendamment des choix d'implémentation.
>
> L'approche opposée est **Code-First** : on écrit le code Java d'abord et le WSDL est généré depuis les annotations. Elle est plus rapide mais produit des WSDL moins maîtrisés.

---

### Q5 — Comment Spring-WS gère-t-il le routing des messages SOAP ?

> Spring-WS utilise le **`MessageDispatcherServlet`** pour recevoir tous les messages SOAP entrants. Il route ensuite chaque message vers le bon **`@Endpoint`** grâce à l'annotation **`@PayloadRoot`** qui identifie l'opération par son **namespace** et le **localPart** (nom de l'élément XML racine du body).
>
> Par exemple, un message dont le body contient `<GetClientRequest>` sera routé vers la méthode annotée avec `@PayloadRoot(localPart = "GetClientRequest")`.

---

### Q6 — Qu'est-ce qu'un SOAP Fault ?

> Un **SOAP Fault** est la façon standard de retourner une erreur dans SOAP. C'est un élément XML structuré placé dans le `<Body>` du message de réponse, contenant un **faultcode** (type d'erreur), un **faultstring** (message lisible) et un **detail** (informations complémentaires).
>
> En Spring-WS, on peut mapper automatiquement les exceptions Java en SOAP Fault via l'annotation `@SoapFault(faultCode = FaultCode.SERVER)` sur la classe d'exception.

---

### Q7 — Qu'est-ce que WS-Security et pourquoi est-il supérieur à HTTPS seul ?

> **WS-Security** est un standard OASIS qui implémente la sécurité **au niveau du message SOAP** (chiffrement XML, signature numérique, UsernameToken, SAML).
>
> L'avantage sur HTTPS : HTTPS sécurise uniquement le **canal de transport**. Si le message transite par un **proxy ou un ESB**, il peut être déchiffré à chaque nœud intermédiaire. Avec WS-Security, le message reste sécurisé de **bout en bout**, peu importe les intermédiaires — ce qu'on appelle la sécurité **end-to-end** au niveau message.

---

### Q8 — Comment consommer un service SOAP externe dans Spring Boot ?

> Pour consommer un service SOAP externe, j'utilise le **`WebServiceTemplate`** de Spring-WS. Je génère d'abord les classes Java depuis le WSDL du service tiers (avec le plugin `wsdl2java` ou le plugin Maven `jaxb2`). Ensuite je configure un bean `WebServiceTemplate` avec l'URL du service et j'utilise la méthode `marshalSendAndReceive()` pour envoyer la requête et récupérer la réponse de façon typée.
>
> Pour les tests, j'utilise **MockWebServiceServer** de Spring-WS qui permet de mocker le service distant sans appel réseau réel.

---

### Q9 — Quelle est la différence entre les styles Document/Literal et RPC/Encoded ?

> **Document/Literal** : le body SOAP contient directement l'élément XSD défini dans le contrat. C'est le style recommandé par le **WS-I Basic Profile** et celui utilisé par Spring-WS. Il est interopérable et validable via XSD.
>
> **RPC/Encoded** : les paramètres sont encodés selon les règles SOAP propriétaires. Ce style est **déprécié** car il n'est pas interopérable et ne peut pas être validé par un XSD.
>
> En pratique, on utilise toujours **Document/Literal Wrapped** pour un maximum de compatibilité.

---

### Q10 — Comment tester un Endpoint SOAP avec Spring-WS ?

> Spring-WS fournit **`MockWebServiceClient`** pour tester les Endpoints en isolation, sans démarrer de serveur HTTP réel.
>
> ```java
> @SpringBootTest
> public class ClientEndpointTest {
>
>     @Autowired
>     private ApplicationContext context;
>
>     private MockWebServiceClient mockClient;
>
>     @BeforeEach
>     void setUp() {
>         mockClient = MockWebServiceClient.createClient(context);
>     }
>
>     @Test
>     void shouldReturnClient() {
>         Source request = new StringSource(
>             "<GetClientRequest xmlns='http://example.com/client'>" +
>             "  <clientId>1</clientId>" +
>             "</GetClientRequest>");
>
>         mockClient.sendRequest(withPayload(request))
>             .andExpect(payload(new StringSource(
>                 "<GetClientResponse xmlns='http://example.com/client'>" +
>                 "  <nom>Dupont</nom>" +
>                 "</GetClientResponse>")));
>     }
> }
> ```
>
> Cette approche teste le **parsing XML**, le **routing** et la **logique métier** du Endpoint de bout en bout.

---

### Q11 — Peut-on avoir SOAP et REST dans la même application Spring Boot ?

> Oui, tout à fait. Dans Spring Boot, **Spring-WS** (SOAP) et **Spring MVC** ou **Spring WebFlux** (REST) coexistent sans conflit.
>
> - Le `MessageDispatcherServlet` de Spring-WS est mappé sur `/ws/*`.
> - Le `DispatcherServlet` de Spring MVC est mappé sur `/api/*` par exemple.
>
> Les deux peuvent partager la même couche service (`@Service`) et la même base de données. C'est un pattern courant lors de la **migration progressive** d'un système SOAP vers REST : on expose les deux interfaces en parallèle.

---

### Q12 — Quelles annotations Spring-WS sont essentielles ?

> - **`@EnableWs`** : active le support Spring-WS dans la configuration.
> - **`@Endpoint`** : marque une classe comme gestionnaire de messages SOAP (équivalent du `@RestController` en REST).
> - **`@PayloadRoot`** : route un message SOAP entrant vers une méthode précise, basé sur le namespace et le localPart du body.
> - **`@RequestPayload`** : désérialise le body SOAP entrant en objet Java.
> - **`@ResponsePayload`** : sérialise l'objet Java retourné en body SOAP de réponse.

---

*Bonne chance pour ton entretien ! 🚀*
