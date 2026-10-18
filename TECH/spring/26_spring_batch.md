# 📦 Cours Spring Batch — Préparation Entretien Développeur Java / Spring Boot

> **Format :** Français · Termes techniques en anglais  
> **Objectif :** Maîtriser Spring Batch de A à Z pour un poste backend Java/Spring Boot  
> **Niveau :** Débutant → Avancé

---

## Table des matières

1. [Introduction à Spring Batch](#1-introduction-à-spring-batch)
2. [Architecture & Concepts fondamentaux](#2-architecture--concepts-fondamentaux)
3. [Job & Step](#3-job--step)
4. [ItemReader](#4-itemreader)
5. [ItemProcessor](#5-itemprocessor)
6. [ItemWriter](#6-itemwriter)
7. [Chunk-Oriented Processing](#7-chunk-oriented-processing)
8. [Tasklet](#8-tasklet)
9. [JobRepository & JobLauncher](#9-jobrepository--joblauncher)
10. [JobParameters & JobInstance & JobExecution](#10-jobparameters--jobinstance--jobexecution)
11. [Listeners & Hooks](#11-listeners--hooks)
12. [Gestion des erreurs — Skip, Retry, Restart](#12-gestion-des-erreurs--skip-retry-restart)
13. [Partitioning & Multi-threading](#13-partitioning--multi-threading)
14. [Flows, Decisions & Conditional Steps](#14-flows-decisions--conditional-steps)
15. [Spring Batch avec Spring Boot](#15-spring-batch-avec-spring-boot)
16. [Tests Spring Batch](#16-tests-spring-batch)
17. [Bonnes pratiques](#17-bonnes-pratiques)
18. [Questions/Réponses d'entretien](#18-questionsréponses-dentretien)

---

## 1. Introduction à Spring Batch

### Qu'est-ce que Spring Batch ?

**Spring Batch** est un framework open-source du projet Spring dédié au traitement **batch** (traitement par lots). Il fournit des fonctionnalités réutilisables essentielles pour le traitement de volumes importants de données : journalisation, transactions, gestion des statistiques, restart/retry/skip, et bien plus.

### Quand utiliser Spring Batch ?

Spring Batch est adapté aux cas suivants :

| Cas d'usage                                    | Exemple concret                                     |
|-----------------------------------------------|-----------------------------------------------------|
| Import massif de données                       | Importer 5 millions de lignes CSV → base de données |
| Export de données                              | Générer un fichier de facturation mensuelle         |
| Synchronisation entre systèmes                 | Synchroniser ERP et base analytique chaque nuit     |
| Calculs périodiques                            | Calcul des intérêts bancaires quotidiens            |
| Transformation de données (ETL)                | Extract → Transform → Load vers un data warehouse   |
| Nettoyage et purge                             | Archiver les données de plus de 5 ans               |
| Génération de rapports                         | Rapport de ventes mensuel en PDF/CSV                |
| Envoi de notifications en masse                | Email marketing, alertes                            |

### Ce que Spring Batch apporte

- **Gestion des transactions** : chaque chunk est traité dans une transaction.
- **Restart automatique** : si le job échoue, il peut reprendre là où il s'est arrêté.
- **Skip & Retry** : ignorer ou retenter les enregistrements en erreur.
- **Partitioning** : traitement parallèle pour les gros volumes.
- **Monitoring** : toutes les exécutions sont tracées en base de données.
- **Scheduling** : intégration avec Spring Scheduler ou Quartz.

### Dépendances Maven

```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-batch</artifactId>
</dependency>

<!-- Base de données pour JobRepository (H2 en dev, PostgreSQL en prod) -->
<dependency>
    <groupId>org.postgresql</groupId>
    <artifactId>postgresql</artifactId>
</dependency>

<!-- Pour les tests -->
<dependency>
    <groupId>org.springframework.batch</groupId>
    <artifactId>spring-batch-test</artifactId>
    <scope>test</scope>
</dependency>
```

---

## 2. Architecture & Concepts fondamentaux

### Vue d'ensemble

```
┌──────────────────────────────────────────────────────────────────────┐
│                          Spring Batch                                │
│                                                                      │
│   ┌─────────────┐     ┌─────────────────────────────────────────┐  │
│   │ JobLauncher │────▶│                  Job                    │  │
│   └─────────────┘     │                                         │  │
│                        │  ┌──────────┐  ┌──────────┐  ┌──────┐ │  │
│   ┌─────────────┐     │  │  Step 1  │─▶│  Step 2  │─▶│  ... │ │  │
│   │JobRepository│     │  └──────────┘  └──────────┘  └──────┘ │  │
│   └─────────────┘     └─────────────────────────────────────────┘  │
│          │                      │                                    │
│          │                      ▼                                    │
│          │           ┌─────────────────────┐                       │
│          │           │  Chunk Processing   │                       │
│          │           │  ItemReader         │                       │
│          │           │  ItemProcessor      │                       │
│          │           │  ItemWriter         │                       │
│          │           └─────────────────────┘                       │
│          │                                                          │
│          └──────────────▶ Base de données (metadata tables)        │
└──────────────────────────────────────────────────────────────────────┘
```

### Hiérarchie des concepts

```
Job
 └── JobInstance (une exécution logique avec des JobParameters)
      └── JobExecution (une tentative d'exécution concrète)
           └── StepExecution (une exécution d'un Step)
                ├── ItemReader
                ├── ItemProcessor
                └── ItemWriter
```

### Tables de métadonnées Spring Batch

Spring Batch crée automatiquement des tables en base pour tracker les exécutions :

| Table                        | Contenu                                              |
|-----------------------------|------------------------------------------------------|
| `BATCH_JOB_INSTANCE`        | Les instances de jobs (combinaison job + params)     |
| `BATCH_JOB_EXECUTION`       | Chaque tentative d'exécution d'un job                |
| `BATCH_JOB_EXECUTION_PARAMS`| Les paramètres d'une exécution                       |
| `BATCH_STEP_EXECUTION`      | Statistiques de chaque step (read/write/skip count)  |
| `BATCH_JOB_EXECUTION_CONTEXT`| Contexte sérialisé du job (pour restart)            |
| `BATCH_STEP_EXECUTION_CONTEXT`| Contexte sérialisé du step (pour restart)          |

---

## 3. Job & Step

### Job

Un **Job** est le concept de plus haut niveau dans Spring Batch. Il représente un traitement batch complet, composé d'une ou plusieurs **Steps** exécutées séquentiellement ou en parallèle.

```java
@Configuration
public class ImportJobConfig {

    @Bean
    public Job importUsersJob(JobRepository jobRepository,
                              Step readCsvStep,
                              Step sendEmailStep) {
        return new JobBuilder("importUsersJob", jobRepository)
                .start(readCsvStep)
                .next(sendEmailStep)
                .build();
    }
}
```

### Step

Un **Step** est une phase autonome d'un Job. Chaque Step encapsule la logique de traitement. Il existe deux types de Steps :

1. **Chunk-oriented Step** : traitement par lots (ItemReader → ItemProcessor → ItemWriter)
2. **Tasklet Step** : exécution d'une tâche simple (une seule opération)

```java
@Bean
public Step readCsvStep(JobRepository jobRepository,
                        PlatformTransactionManager transactionManager,
                        ItemReader<User> reader,
                        ItemProcessor<User, UserEntity> processor,
                        ItemWriter<UserEntity> writer) {
    return new StepBuilder("readCsvStep", jobRepository)
            .<User, UserEntity>chunk(100, transactionManager)
            .reader(reader)
            .processor(processor)
            .writer(writer)
            .build();
}
```

### StepExecution & ExecutionContext

Chaque Step dispose d'un **ExecutionContext** : une map key/value persistée en base, permettant de sauvegarder l'état pour le restart.

```java
// Dans un ItemReader, sauvegarder l'état courant
@BeforeStep
public void saveStepExecution(StepExecution stepExecution) {
    this.stepExecution = stepExecution;
}

// Dans read()
stepExecution.getExecutionContext().putInt("current.line", currentLine);

// Pour récupérer l'état lors d'un restart
int startAt = stepExecution.getExecutionContext().getInt("current.line", 0);
```

---

## 4. ItemReader

### Rôle

L'**ItemReader** lit les données source **un élément à la fois**. Quand il n'y a plus de données, il retourne `null` (signal de fin de lecture).

```java
public interface ItemReader<T> {
    T read() throws Exception, UnexpectedInputException, ParseException, NonTransientResourceException;
}
```

### FlatFileItemReader — Lecture de fichiers CSV/TXT

```java
@Bean
public FlatFileItemReader<User> csvUserReader() {
    return new FlatFileItemReaderBuilder<User>()
            .name("csvUserReader")
            .resource(new ClassPathResource("users.csv"))
            .linesToSkip(1)          // Ignorer l'en-tête
            .delimited()
            .delimiter(",")
            .names("id", "firstName", "lastName", "email", "birthDate")
            .fieldSetMapper(new BeanWrapperFieldSetMapper<>() {{
                setTargetType(User.class);
            }})
            .build();
}
```

### FlatFileItemReader avec FieldSetMapper personnalisé

```java
@Bean
public FlatFileItemReader<User> csvUserReader() {
    return new FlatFileItemReaderBuilder<User>()
            .name("csvUserReader")
            .resource(new FileSystemResource("/data/users.csv"))
            .linesToSkip(1)
            .delimited()
            .delimiter(";")
            .names("firstName", "lastName", "email", "age")
            .fieldSetMapper(fieldSet -> {
                User user = new User();
                user.setFirstName(fieldSet.readString("firstName"));
                user.setLastName(fieldSet.readString("lastName"));
                user.setEmail(fieldSet.readString("email"));
                user.setAge(fieldSet.readInt("age"));
                return user;
            })
            .build();
}
```

### JdbcCursorItemReader — Lecture depuis la base de données

```java
@Bean
public JdbcCursorItemReader<User> dbUserReader(DataSource dataSource) {
    return new JdbcCursorItemReaderBuilder<User>()
            .name("dbUserReader")
            .dataSource(dataSource)
            .sql("SELECT id, first_name, last_name, email FROM users WHERE active = true ORDER BY id")
            .rowMapper(new BeanPropertyRowMapper<>(User.class))
            .build();
}
```

### JdbcPagingItemReader — Lecture paginée (recommandée pour gros volumes)

```java
@Bean
public JdbcPagingItemReader<User> pagingUserReader(DataSource dataSource,
                                                    PagingQueryProvider queryProvider) {
    return new JdbcPagingItemReaderBuilder<User>()
            .name("pagingUserReader")
            .dataSource(dataSource)
            .queryProvider(queryProvider)
            .pageSize(1000)
            .rowMapper(new BeanPropertyRowMapper<>(User.class))
            .build();
}

@Bean
public PagingQueryProvider queryProvider(DataSource dataSource) throws Exception {
    SqlPagingQueryProviderFactoryBean factory = new SqlPagingQueryProviderFactoryBean();
    factory.setDataSource(dataSource);
    factory.setSelectClause("SELECT id, first_name, last_name, email");
    factory.setFromClause("FROM users");
    factory.setWhereClause("WHERE active = true");
    factory.setSortKey("id");
    return factory.getObject();
}
```

### JpaPagingItemReader — Lecture JPA

```java
@Bean
public JpaPagingItemReader<User> jpaUserReader(EntityManagerFactory entityManagerFactory) {
    return new JpaPagingItemReaderBuilder<User>()
            .name("jpaUserReader")
            .entityManagerFactory(entityManagerFactory)
            .queryString("SELECT u FROM User u WHERE u.active = true ORDER BY u.id")
            .pageSize(500)
            .build();
}
```

### StaxEventItemReader — Lecture XML

```java
@Bean
public StaxEventItemReader<User> xmlUserReader() {
    return new StaxEventItemReaderBuilder<User>()
            .name("xmlUserReader")
            .resource(new ClassPathResource("users.xml"))
            .addFragmentRootElements("user")
            .unmarshaller(userUnmarshaller())
            .build();
}
```

### ItemReader personnalisé

```java
@Component
public class RestApiItemReader implements ItemReader<Product>, InitializingBean {

    private final WebClient webClient;
    private Iterator<Product> iterator;

    public RestApiItemReader(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public void afterPropertiesSet() {
        // Charger tous les produits depuis l'API au démarrage du step
        List<Product> products = webClient.get()
                .uri("/api/products")
                .retrieve()
                .bodyToFlux(Product.class)
                .collectList()
                .block();
        this.iterator = products != null ? products.iterator() : Collections.emptyIterator();
    }

    @Override
    public Product read() {
        return iterator.hasNext() ? iterator.next() : null; // null = fin de lecture
    }
}
```

---

## 5. ItemProcessor

### Rôle

L'**ItemProcessor** transforme et/ou filtre les données lues par l'ItemReader avant qu'elles soient écrites. Il est **optionnel**.

```java
public interface ItemProcessor<I, O> {
    O process(I item) throws Exception;
    // Retourner null = filtrer l'élément (non transmis à l'ItemWriter)
}
```

### ItemProcessor simple

```java
@Component
public class UserProcessor implements ItemProcessor<UserDto, UserEntity> {

    @Override
    public UserEntity process(UserDto dto) throws Exception {
        // Filtrer les emails invalides
        if (!isValidEmail(dto.getEmail())) {
            return null; // Cet élément sera ignoré (filtré)
        }

        // Transformer DTO → Entité
        UserEntity entity = new UserEntity();
        entity.setFirstName(dto.getFirstName().trim().toUpperCase());
        entity.setLastName(dto.getLastName().trim().toUpperCase());
        entity.setEmail(dto.getEmail().toLowerCase());
        entity.setCreatedAt(LocalDateTime.now());
        entity.setActive(true);
        return entity;
    }

    private boolean isValidEmail(String email) {
        return email != null && email.matches("^[\\w.-]+@[\\w.-]+\\.[a-z]{2,}$");
    }
}
```

### CompositeItemProcessor — Chaîner plusieurs processors

```java
@Bean
public CompositeItemProcessor<UserDto, UserEntity> compositeProcessor(
        ValidationProcessor validationProcessor,
        EnrichmentProcessor enrichmentProcessor,
        MappingProcessor mappingProcessor) {

    CompositeItemProcessor<UserDto, UserEntity> composite = new CompositeItemProcessor<>();
    composite.setDelegates(List.of(
            validationProcessor,
            enrichmentProcessor,
            mappingProcessor
    ));
    return composite;
}
```

### ClassifierCompositeItemProcessor — Router vers différents processors

```java
@Bean
public ClassifierCompositeItemProcessor<User, User> classifierProcessor(
        PremiumUserProcessor premiumProcessor,
        StandardUserProcessor standardProcessor) {

    ClassifierCompositeItemProcessor<User, User> processor = new ClassifierCompositeItemProcessor<>();
    processor.setClassifier(user ->
            user.isPremium() ? premiumProcessor : standardProcessor
    );
    return processor;
}
```

---

## 6. ItemWriter

### Rôle

L'**ItemWriter** écrit les données transformées. Il reçoit une **liste** d'éléments (un chunk entier) à chaque appel, et non un élément à la fois.

```java
public interface ItemWriter<T> {
    void write(Chunk<? extends T> chunk) throws Exception;
}
```

### FlatFileItemWriter — Écriture CSV/TXT

```java
@Bean
public FlatFileItemWriter<UserEntity> csvUserWriter() {
    return new FlatFileItemWriterBuilder<UserEntity>()
            .name("csvUserWriter")
            .resource(new FileSystemResource("/output/users_export.csv"))
            .headerCallback(writer -> writer.write("ID,FIRST_NAME,LAST_NAME,EMAIL"))
            .delimited()
            .delimiter(",")
            .names("id", "firstName", "lastName", "email")
            .build();
}
```

### JdbcBatchItemWriter — Écriture en base via JDBC

```java
@Bean
public JdbcBatchItemWriter<UserEntity> dbUserWriter(DataSource dataSource) {
    return new JdbcBatchItemWriterBuilder<UserEntity>()
            .dataSource(dataSource)
            .sql("INSERT INTO users (first_name, last_name, email, created_at) " +
                 "VALUES (:firstName, :lastName, :email, :createdAt) " +
                 "ON CONFLICT (email) DO UPDATE SET " +
                 "first_name = EXCLUDED.first_name, last_name = EXCLUDED.last_name")
            .beanMapped()   // Utilise les noms des propriétés du bean
            .build();
}
```

### JpaItemWriter — Écriture JPA

```java
@Bean
public JpaItemWriter<UserEntity> jpaUserWriter(EntityManagerFactory entityManagerFactory) {
    JpaItemWriter<UserEntity> writer = new JpaItemWriter<>();
    writer.setEntityManagerFactory(entityManagerFactory);
    return writer;
}
```

### CompositeItemWriter — Écrire vers plusieurs destinations simultanément

```java
@Bean
public CompositeItemWriter<UserEntity> compositeWriter(
        JdbcBatchItemWriter<UserEntity> dbWriter,
        FlatFileItemWriter<UserEntity> auditWriter) {

    CompositeItemWriter<UserEntity> composite = new CompositeItemWriter<>();
    composite.setDelegates(List.of(dbWriter, auditWriter));
    return composite;
}
```

### ClassifierCompositeItemWriter — Router vers différents writers

```java
@Bean
public ClassifierCompositeItemWriter<Order> classifierWriter(
        JdbcBatchItemWriter<Order> validOrderWriter,
        FlatFileItemWriter<Order> rejectedOrderWriter) {

    ClassifierCompositeItemWriter<Order> writer = new ClassifierCompositeItemWriter<>();
    writer.setClassifier(order ->
            order.isValid() ? validOrderWriter : rejectedOrderWriter
    );
    return writer;
}
```

### ItemWriter personnalisé

```java
@Component
public class EmailNotificationWriter implements ItemWriter<User> {

    private final EmailService emailService;

    public EmailNotificationWriter(EmailService emailService) {
        this.emailService = emailService;
    }

    @Override
    public void write(Chunk<? extends User> chunk) throws Exception {
        List<? extends User> users = chunk.getItems();
        // Traitement du chunk entier (batch d'emails)
        users.forEach(user ->
                emailService.sendWelcomeEmail(user.getEmail(), user.getFirstName())
        );
    }
}
```

---

## 7. Chunk-Oriented Processing

### Principe

Le **chunk-oriented processing** est le modèle de traitement central de Spring Batch. L'idée est de lire les données **N éléments à la fois** (chunk-size), de les traiter, puis de les écrire dans une seule transaction.

```
Pour chaque chunk :
┌──────────────────────────────────────────────────────────┐
│  TRANSACTION BEGIN                                        │
│                                                          │
│  REPEAT chunk-size times:                                │
│    item = reader.read()           ← 1 item à la fois    │
│    if item == null → break        ← fin des données      │
│    processed = processor.process(item)                   │
│    if processed != null → add to chunk                   │
│                                                          │
│  writer.write(chunk)              ← tout le chunk d'un coup │
│                                                          │
│  TRANSACTION COMMIT                                      │
└──────────────────────────────────────────────────────────┘
```

### Configuration du chunk-size

```java
@Bean
public Step processOrdersStep(JobRepository jobRepository,
                               PlatformTransactionManager transactionManager) {
    return new StepBuilder("processOrdersStep", jobRepository)
            .<Order, OrderEntity>chunk(500, transactionManager)  // chunk-size = 500
            .reader(orderReader())
            .processor(orderProcessor())
            .writer(orderWriter())
            .build();
}
```

### Choix du chunk-size

| Volume de données       | Chunk-size recommandé  | Justification                            |
|------------------------|------------------------|------------------------------------------|
| Petits objets, I/O rapide | 500 – 1000          | Balance entre performance et mémoire     |
| Gros objets (images, blobs) | 10 – 50           | Limiter la consommation mémoire          |
| Appels réseau (REST, SOAP) | 1 – 10            | Éviter les timeouts et erreurs réseau    |
| Insertions simples en BDD | 1000 – 5000        | Optimiser les batch inserts JDBC         |

### Impact sur les transactions

Chaque chunk est **atomique** : si l'écriture échoue, tout le chunk est rollbacké et aucun des N items du chunk n'est persisté. Les chunks précédents (déjà committés) ne sont pas affectés.

---

## 8. Tasklet

### Qu'est-ce qu'un Tasklet ?

Un **Tasklet** est un Step simplifié pour des tâches ponctuelles qui ne s'inscrivent pas dans le modèle chunk. Il s'exécute une seule fois dans une transaction.

```java
public interface Tasklet {
    RepeatStatus execute(StepContribution contribution, ChunkContext chunkContext) throws Exception;
    // RepeatStatus.FINISHED : le tasklet est terminé
    // RepeatStatus.CONTINUABLE : le tasklet sera rappelé (boucle)
}
```

### Exemples d'usage des Tasklets

- Déplacer/supprimer un fichier après traitement
- Envoyer un email de notification de fin de job
- Vider une table avant import
- Appeler une procédure stockée
- Préparer le contexte d'exécution

### Tasklet simple — Nettoyage de table

```java
@Component
public class TruncateTableTasklet implements Tasklet {

    private final JdbcTemplate jdbcTemplate;

    public TruncateTableTasklet(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public RepeatStatus execute(StepContribution contribution,
                                ChunkContext chunkContext) throws Exception {
        jdbcTemplate.execute("TRUNCATE TABLE staging_users");
        log.info("Table staging_users vidée avec succès");
        return RepeatStatus.FINISHED;
    }
}
```

### Tasklet — Déplacement de fichier

```java
@Component
public class MoveFileTasklet implements Tasklet {

    @Value("${batch.input.file}")
    private String inputFilePath;

    @Value("${batch.archive.directory}")
    private String archiveDirectory;

    @Override
    public RepeatStatus execute(StepContribution contribution,
                                ChunkContext chunkContext) throws Exception {
        Path source = Paths.get(inputFilePath);
        String timestamp = LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyyMMdd_HHmmss"));
        Path target = Paths.get(archiveDirectory, "users_" + timestamp + ".csv");

        Files.move(source, target, StandardCopyOption.REPLACE_EXISTING);
        log.info("Fichier déplacé vers : {}", target);
        return RepeatStatus.FINISHED;
    }
}
```

### Intégration du Tasklet dans un Step

```java
@Bean
public Step truncateStep(JobRepository jobRepository,
                         PlatformTransactionManager transactionManager,
                         TruncateTableTasklet truncateTableTasklet) {
    return new StepBuilder("truncateStep", jobRepository)
            .tasklet(truncateTableTasklet, transactionManager)
            .build();
}

@Bean
public Step moveFileStep(JobRepository jobRepository,
                         PlatformTransactionManager transactionManager,
                         MoveFileTasklet moveFileTasklet) {
    return new StepBuilder("moveFileStep", jobRepository)
            .tasklet(moveFileTasklet, transactionManager)
            .build();
}
```

### Job avec Tasklets + Chunk Step

```java
@Bean
public Job importJob(JobRepository jobRepository,
                     Step truncateStep,
                     Step importCsvStep,
                     Step moveFileStep,
                     Step notifyStep) {
    return new JobBuilder("importJob", jobRepository)
            .start(truncateStep)      // Tasklet : vider la table
            .next(importCsvStep)      // Chunk : lire CSV → écrire en BDD
            .next(moveFileStep)       // Tasklet : archiver le fichier
            .next(notifyStep)         // Tasklet : envoyer email
            .build();
}
```

---

## 9. JobRepository & JobLauncher

### JobRepository

Le **JobRepository** est le mécanisme de persistance de Spring Batch. Il est responsable de :
- Créer et mettre à jour les `JobInstance`, `JobExecution`, `StepExecution`
- Persister les `ExecutionContext` pour le restart
- Stocker les statistiques (read count, write count, skip count...)

```java
// Configuration automatique par Spring Boot
// Mais on peut personnaliser :

@Configuration
public class BatchConfig {

    @Bean
    public JobRepository jobRepository(DataSource dataSource,
                                        PlatformTransactionManager transactionManager) throws Exception {
        JobRepositoryFactoryBean factory = new JobRepositoryFactoryBean();
        factory.setDataSource(dataSource);
        factory.setTransactionManager(transactionManager);
        factory.setIsolationLevelForCreate("ISOLATION_READ_COMMITTED");
        factory.setTablePrefix("BATCH_"); // Préfixe des tables de métadonnées
        factory.afterPropertiesSet();
        return factory.getObject();
    }
}
```

### JobLauncher

Le **JobLauncher** est responsable du démarrage des jobs. Il reçoit un `Job` et des `JobParameters`.

```java
@Service
public class BatchJobService {

    private final JobLauncher jobLauncher;
    private final Job importUsersJob;

    public BatchJobService(JobLauncher jobLauncher, Job importUsersJob) {
        this.jobLauncher = jobLauncher;
        this.importUsersJob = importUsersJob;
    }

    public void launchImport(String fileName) throws Exception {
        JobParameters params = new JobParametersBuilder()
                .addString("fileName", fileName)
                .addLong("timestamp", System.currentTimeMillis()) // Pour rejouer le job
                .toJobParameters();

        JobExecution execution = jobLauncher.run(importUsersJob, params);
        log.info("Job lancé, status: {}", execution.getStatus());
    }
}
```

### Lancement asynchrone

```java
@Bean
public JobLauncher asyncJobLauncher(JobRepository jobRepository) throws Exception {
    TaskExecutorJobLauncher launcher = new TaskExecutorJobLauncher();
    launcher.setJobRepository(jobRepository);
    launcher.setTaskExecutor(new SimpleAsyncTaskExecutor());
    launcher.afterPropertiesSet();
    return launcher;
}
```

### Lancement via endpoint REST

```java
@RestController
@RequestMapping("/api/batch")
public class BatchController {

    private final JobLauncher jobLauncher;
    private final Job importUsersJob;

    @PostMapping("/import")
    public ResponseEntity<String> launchImport(@RequestParam String fileName) {
        try {
            JobParameters params = new JobParametersBuilder()
                    .addString("fileName", fileName)
                    .addLong("startTime", System.currentTimeMillis())
                    .toJobParameters();

            JobExecution execution = jobLauncher.run(importUsersJob, params);
            return ResponseEntity.ok("Job lancé : " + execution.getId());
        } catch (Exception e) {
            return ResponseEntity.internalServerError().body("Erreur : " + e.getMessage());
        }
    }
}
```

### Lancement via Scheduling

```java
@Component
@EnableScheduling
public class BatchScheduler {

    private final JobLauncher jobLauncher;
    private final Job importUsersJob;

    // Tous les jours à 2h du matin
    @Scheduled(cron = "0 0 2 * * ?")
    public void scheduledImport() throws Exception {
        JobParameters params = new JobParametersBuilder()
                .addLong("startTime", System.currentTimeMillis())
                .toJobParameters();
        jobLauncher.run(importUsersJob, params);
    }
}
```

---

## 10. JobParameters & JobInstance & JobExecution

### JobParameters

Les **JobParameters** sont les paramètres passés au job lors de son lancement. Ils servent à :
1. **Identifier une JobInstance** (paramètres = clé d'identité)
2. **Transmettre des données au job** (nom de fichier, date, etc.)

```java
JobParameters params = new JobParametersBuilder()
        .addString("fileName", "users_2024_01.csv")   // String
        .addLong("reportDate", 20240101L)              // Long
        .addDouble("threshold", 0.95)                  // Double
        .addDate("executionDate", new Date())           // Date
        .addLocalDate("period", LocalDate.now())       // LocalDate (Spring Batch 5+)
        .toJobParameters();
```

### Identifier un paramètre (pour la JobInstance)

Par défaut, tous les paramètres contribuent à l'identité de la JobInstance. On peut marquer certains comme non-identifiants :

```java
// Spring Batch 5+
JobParameters params = new JobParametersBuilder()
        .addString("fileName", "users.csv", true)         // identifying = true (défaut)
        .addLong("timestamp", System.currentTimeMillis(), false) // identifying = false
        .toJobParameters();
```

### JobInstance

Une **JobInstance** représente une exécution logique d'un job avec des paramètres donnés. Deux runs avec les mêmes paramètres `identifying` partagent la même JobInstance.

```
Job "importUsersJob" + params {fileName="users_jan.csv"} → JobInstance #1
Job "importUsersJob" + params {fileName="users_feb.csv"} → JobInstance #2
```

**Règle** : on ne peut pas relancer une JobInstance qui s'est terminée avec succès (`COMPLETED`). Pour rejouer, il faut changer les paramètres ou supprimer l'entrée en base.

### JobExecution

Une **JobExecution** représente une tentative concrète d'exécuter une JobInstance. Une même JobInstance peut avoir plusieurs JobExecutions (en cas de restart après échec).

```
JobInstance #1 (fileName="users_jan.csv")
 ├── JobExecution #1 → FAILED (erreur réseau à 60%)
 └── JobExecution #2 → COMPLETED (reprise depuis 60%)
```

### États d'une JobExecution

| Status         | Description                                              |
|---------------|----------------------------------------------------------|
| `STARTING`    | En cours de démarrage                                    |
| `STARTED`     | En cours d'exécution                                     |
| `STOPPING`    | Arrêt demandé                                            |
| `STOPPED`     | Arrêté manuellement                                      |
| `COMPLETED`   | Terminé avec succès                                      |
| `FAILED`      | Terminé en erreur                                        |
| `ABANDONED`   | Abandonné (ne peut plus être restarté)                   |
| `UNKNOWN`     | État inconnu                                             |

### Accéder aux JobParameters depuis un Step

```java
@Component
@StepScope
public class ParameterizedFileReader implements ItemReader<User> {

    private final FlatFileItemReader<User> delegate;

    // Injection via @Value avec le contexte du step
    public ParameterizedFileReader(
            @Value("#{jobParameters['fileName']}") String fileName) {

        this.delegate = new FlatFileItemReaderBuilder<User>()
                .name("fileReader")
                .resource(new FileSystemResource("/data/" + fileName))
                .delimited()
                .names("firstName", "lastName", "email")
                .targetType(User.class)
                .build();
    }

    @Override
    public User read() throws Exception {
        return delegate.read();
    }
}
```

---

## 11. Listeners & Hooks

### Types de Listeners

Spring Batch propose des listeners à chaque niveau du traitement :

| Interface                   | Méthodes clés                                              | Niveau     |
|----------------------------|------------------------------------------------------------|-----------|
| `JobExecutionListener`     | `beforeJob()`, `afterJob()`                                | Job        |
| `StepExecutionListener`    | `beforeStep()`, `afterStep()`                              | Step       |
| `ChunkListener`            | `beforeChunk()`, `afterChunk()`, `afterChunkError()`       | Chunk      |
| `ItemReadListener`         | `beforeRead()`, `afterRead()`, `onReadError()`             | Read       |
| `ItemProcessListener`      | `beforeProcess()`, `afterProcess()`, `onProcessError()`    | Process    |
| `ItemWriteListener`        | `beforeWrite()`, `afterWrite()`, `onWriteError()`          | Write      |
| `SkipListener`             | `onSkipInRead()`, `onSkipInProcess()`, `onSkipInWrite()`   | Skip       |
| `RetryListener`            | `open()`, `close()`, `onError()`                           | Retry      |

### JobExecutionListener

```java
@Component
public class JobNotificationListener implements JobExecutionListener {

    @Override
    public void beforeJob(JobExecution jobExecution) {
        log.info("=== Démarrage du job : {} ===", jobExecution.getJobInstance().getJobName());
        log.info("Paramètres : {}", jobExecution.getJobParameters());
    }

    @Override
    public void afterJob(JobExecution jobExecution) {
        BatchStatus status = jobExecution.getStatus();

        if (status == BatchStatus.COMPLETED) {
            log.info("=== Job terminé avec succès ===");
            log.info("Durée : {} ms",
                    jobExecution.getEndTime().toEpochMilli() -
                    jobExecution.getStartTime().toEpochMilli());
        } else if (status == BatchStatus.FAILED) {
            log.error("=== Job échoué ===");
            jobExecution.getAllFailureExceptions().forEach(ex ->
                    log.error("Erreur : {}", ex.getMessage())
            );
            // Alerter l'équipe ops...
            notificationService.sendAlert("Job batch échoué : " + jobExecution.getJobInstance().getJobName());
        }
    }
}
```

### StepExecutionListener

```java
@Component
public class StepMetricsListener implements StepExecutionListener {

    @Override
    public void beforeStep(StepExecution stepExecution) {
        log.info("--- Début du step : {} ---", stepExecution.getStepName());
    }

    @Override
    public ExitStatus afterStep(StepExecution stepExecution) {
        log.info("--- Fin du step : {} ---", stepExecution.getStepName());
        log.info("  Read count    : {}", stepExecution.getReadCount());
        log.info("  Write count   : {}", stepExecution.getWriteCount());
        log.info("  Skip count    : {}", stepExecution.getSkipCount());
        log.info("  Filter count  : {}", stepExecution.getFilterCount());
        log.info("  Status        : {}", stepExecution.getStatus());

        // Personnaliser l'ExitStatus si besoin
        if (stepExecution.getSkipCount() > 100) {
            return new ExitStatus("COMPLETED_WITH_WARNINGS");
        }
        return stepExecution.getExitStatus();
    }
}
```

### SkipListener — Tracer les éléments ignorés

```java
@Component
public class SkipAuditListener implements SkipListener<User, UserEntity> {

    private final SkipRepository skipRepository;

    @Override
    public void onSkipInRead(Throwable t) {
        log.warn("Erreur de lecture ignorée : {}", t.getMessage());
        skipRepository.saveSkipEvent("READ", null, t.getMessage());
    }

    @Override
    public void onSkipInProcess(User item, Throwable t) {
        log.warn("Élément ignoré en processing : {} - Erreur : {}", item.getEmail(), t.getMessage());
        skipRepository.saveSkipEvent("PROCESS", item.getEmail(), t.getMessage());
    }

    @Override
    public void onSkipInWrite(UserEntity item, Throwable t) {
        log.warn("Élément ignoré en écriture : {} - Erreur : {}", item.getEmail(), t.getMessage());
        skipRepository.saveSkipEvent("WRITE", item.getEmail(), t.getMessage());
    }
}
```

### Enregistrement des Listeners dans le Step

```java
@Bean
public Step importStep(JobRepository jobRepository,
                       PlatformTransactionManager transactionManager,
                       StepMetricsListener metricsListener,
                       SkipAuditListener skipAuditListener) {
    return new StepBuilder("importStep", jobRepository)
            .<User, UserEntity>chunk(500, transactionManager)
            .reader(reader())
            .processor(processor())
            .writer(writer())
            .listener(metricsListener)      // StepExecutionListener
            .listener(skipAuditListener)    // SkipListener
            .build();
}

@Bean
public Job importJob(JobRepository jobRepository,
                     Step importStep,
                     JobNotificationListener notificationListener) {
    return new JobBuilder("importJob", jobRepository)
            .listener(notificationListener)  // JobExecutionListener
            .start(importStep)
            .build();
}
```

---

## 12. Gestion des erreurs — Skip, Retry, Restart

### Skip — Ignorer les éléments en erreur

Le **skip** permet de continuer le traitement même si certains éléments lèvent une exception. L'élément problématique est ignoré et le processing continue.

```java
@Bean
public Step importStep(JobRepository jobRepository,
                       PlatformTransactionManager transactionManager) {
    return new StepBuilder("importStep", jobRepository)
            .<User, UserEntity>chunk(500, transactionManager)
            .reader(reader())
            .processor(processor())
            .writer(writer())
            // Configuration du skip
            .faultTolerant()
            .skip(ValidationException.class)        // Ignorer ces exceptions
            .skip(DataIntegrityViolationException.class)
            .noSkip(FileNotFoundException.class)    // Ne JAMAIS ignorer celles-ci
            .skipLimit(100)                         // Maximum 100 éléments ignorés au total
            .listener(skipAuditListener())
            .build();
}
```

### Retry — Retenter en cas d'erreur transitoire

Le **retry** retente l'opération qui a échoué (lecture, processing ou écriture) un nombre de fois configuré avant de considérer l'élément en erreur.

```java
@Bean
public Step importStep(JobRepository jobRepository,
                       PlatformTransactionManager transactionManager) {
    return new StepBuilder("importStep", jobRepository)
            .<User, UserEntity>chunk(500, transactionManager)
            .reader(reader())
            .processor(processor())
            .writer(writer())
            .faultTolerant()
            // Retry
            .retry(TransientDataAccessException.class)  // Retenter pour ces exceptions
            .retry(ResourceAccessException.class)        // Timeout réseau, etc.
            .noRetry(ValidationException.class)          // Ne jamais retenter celles-ci
            .retryLimit(3)                               // Maximum 3 tentatives par item
            // Skip après épuisement des retries
            .skip(TransientDataAccessException.class)
            .skipLimit(50)
            .build();
}
```

### Backoff Policy — Délai entre les retries

```java
@Bean
public Step importStepWithBackoff(JobRepository jobRepository,
                                   PlatformTransactionManager transactionManager) {
    return new StepBuilder("importStep", jobRepository)
            .<User, UserEntity>chunk(500, transactionManager)
            .reader(reader())
            .processor(processor())
            .writer(writer())
            .faultTolerant()
            .retry(TransientDataAccessException.class)
            .retryLimit(3)
            .retryContextCache(new MapRetryContextCache())
            .build();
}

// Pour un backoff exponentiel, utiliser RetryTemplate
@Bean
public RetryTemplate retryTemplate() {
    return RetryTemplate.builder()
            .maxAttempts(3)
            .exponentialBackoff(1000, 2, 10000)  // 1s, 2s, 4s (max 10s)
            .retryOn(TransientDataAccessException.class)
            .build();
}
```

### Restart — Reprendre un job échoué

Spring Batch supporte le **restart automatique** : si un job échoue à mi-chemin, on peut le relancer avec les mêmes paramètres et il reprend là où il s'est arrêté (grâce à l'`ExecutionContext` persisté).

```java
// Relancer le job avec les mêmes paramètres → le job reprend depuis l'échec
JobParameters sameParams = new JobParametersBuilder()
        .addString("fileName", "users.csv")  // mêmes paramètres identifying
        .toJobParameters();

jobLauncher.run(importJob, sameParams); // Reprend depuis le dernier checkpoint
```

**Condition** : le `ItemReader` doit être **restartable** (implémenter `ItemStream` et sauvegarder sa position dans l'`ExecutionContext`). Les readers fournis par Spring Batch (FlatFileItemReader, JdbcPagingItemReader...) sont restartables par défaut.

### Désactiver le restart

```java
@Bean
public Job nonRestartableJob(JobRepository jobRepository, Step step) {
    return new JobBuilder("nonRestartableJob", jobRepository)
            .preventRestart()   // Le job ne peut pas être restarté
            .start(step)
            .build();
}

// Ou au niveau du Step
@Bean
public Step nonRestartableStep(JobRepository jobRepository,
                                PlatformTransactionManager transactionManager) {
    return new StepBuilder("nonRestartableStep", jobRepository)
            .<User, UserEntity>chunk(500, transactionManager)
            .reader(reader())
            .writer(writer())
            .allowStartIfComplete(true)  // Autorise le re-run même si COMPLETED
            .startLimit(2)              // Nombre max de fois que ce step peut démarrer
            .build();
}
```

---

## 13. Partitioning & Multi-threading

### Pourquoi paralléliser ?

Pour les très gros volumes de données, un traitement séquentiel peut être trop lent. Spring Batch propose plusieurs stratégies de parallélisation.

### 1. Multi-threaded Step

Plusieurs threads exécutent le chunk processing en parallèle. Simple mais le reader doit être **thread-safe**.

```java
@Bean
public Step multiThreadedStep(JobRepository jobRepository,
                               PlatformTransactionManager transactionManager) {
    return new StepBuilder("multiThreadedStep", jobRepository)
            .<User, UserEntity>chunk(500, transactionManager)
            .reader(synchronizedReader())  // IMPORTANT : reader thread-safe
            .processor(processor())
            .writer(writer())
            .taskExecutor(taskExecutor())
            .throttleLimit(4)   // Nombre max de threads simultanés (deprecated en SB5, utiliser taskExecutor directement)
            .build();
}

@Bean
public TaskExecutor taskExecutor() {
    ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
    executor.setCorePoolSize(4);
    executor.setMaxPoolSize(8);
    executor.setQueueCapacity(25);
    executor.setThreadNamePrefix("batch-thread-");
    executor.initialize();
    return executor;
}

// Reader thread-safe avec SynchronizedItemStreamReader
@Bean
public SynchronizedItemStreamReader<User> synchronizedReader() {
    SynchronizedItemStreamReader<User> reader = new SynchronizedItemStreamReader<>();
    reader.setDelegate(flatFileReader()); // Le reader de base
    return reader;
}
```

### 2. Partitioning — Partager le travail entre Workers

Le **partitioning** divise les données en partitions indépendantes, chacune traitée par un **worker step** (dans le même process ou distribuée).

```
Manager Step (Partitioner)
 ├── Worker Step 1 → Traite lignes 1-10000
 ├── Worker Step 2 → Traite lignes 10001-20000
 ├── Worker Step 3 → Traites lignes 20001-30000
 └── Worker Step 4 → Traite lignes 30001-40000
```

#### Implémentation du Partitioner

```java
@Component
public class RangePartitioner implements Partitioner {

    private final JdbcTemplate jdbcTemplate;

    @Override
    public Map<String, ExecutionContext> partition(int gridSize) {
        // Récupérer le min et max des IDs à traiter
        Integer minId = jdbcTemplate.queryForObject(
                "SELECT MIN(id) FROM users WHERE processed = false", Integer.class);
        Integer maxId = jdbcTemplate.queryForObject(
                "SELECT MAX(id) FROM users WHERE processed = false", Integer.class);

        Map<String, ExecutionContext> partitions = new HashMap<>();
        int range = (maxId - minId) / gridSize;

        for (int i = 0; i < gridSize; i++) {
            ExecutionContext context = new ExecutionContext();
            int startId = minId + (i * range);
            int endId = (i == gridSize - 1) ? maxId : startId + range - 1;

            context.putInt("minId", startId);
            context.putInt("maxId", endId);
            partitions.put("partition_" + i, context);

            log.info("Partition {} : IDs {} à {}", i, startId, endId);
        }
        return partitions;
    }
}
```

#### Worker Step avec reader paramétré par partition

```java
@Bean
@StepScope
public JdbcPagingItemReader<User> partitionedReader(
        DataSource dataSource,
        @Value("#{stepExecutionContext['minId']}") Integer minId,
        @Value("#{stepExecutionContext['maxId']}") Integer maxId) {

    Map<String, Object> parameterValues = new HashMap<>();
    parameterValues.put("minId", minId);
    parameterValues.put("maxId", maxId);

    return new JdbcPagingItemReaderBuilder<User>()
            .name("partitionedUserReader")
            .dataSource(dataSource)
            .selectClause("SELECT id, first_name, last_name, email")
            .fromClause("FROM users")
            .whereClause("WHERE id >= :minId AND id <= :maxId AND processed = false")
            .parameterValues(parameterValues)
            .sortKey("id")
            .pageSize(500)
            .rowMapper(new BeanPropertyRowMapper<>(User.class))
            .build();
}
```

#### Configuration du Master Step

```java
@Bean
public Step masterStep(JobRepository jobRepository,
                       RangePartitioner partitioner,
                       Step workerStep) {
    return new StepBuilder("masterStep", jobRepository)
            .partitioner("workerStep", partitioner)
            .step(workerStep)
            .gridSize(4)                // 4 partitions
            .taskExecutor(taskExecutor())  // Traitement parallèle
            .build();
}

@Bean
public Step workerStep(JobRepository jobRepository,
                       PlatformTransactionManager transactionManager) {
    return new StepBuilder("workerStep", jobRepository)
            .<User, UserEntity>chunk(500, transactionManager)
            .reader(partitionedReader(null, null, null))
            .processor(processor())
            .writer(writer())
            .build();
}
```

### 3. Parallel Steps — Steps en parallèle

```java
@Bean
public Job parallelJob(JobRepository jobRepository,
                       Flow flow1, Flow flow2, Step finalStep) {
    Flow parallelFlow = new FlowBuilder<SimpleFlow>("parallelFlow")
            .split(new SimpleAsyncTaskExecutor())
            .add(flow1, flow2)
            .build();

    return new JobBuilder("parallelJob", jobRepository)
            .start(parallelFlow)
            .next(finalStep)
            .end()
            .build();
}
```

---

## 14. Flows, Decisions & Conditional Steps

### Flow

Un **Flow** est une séquence de Steps réutilisable et composable.

```java
@Bean
public Flow importFlow(Step validateStep, Step importStep, Step enrichStep) {
    return new FlowBuilder<SimpleFlow>("importFlow")
            .start(validateStep)
            .next(importStep)
            .next(enrichStep)
            .build();
}

@Bean
public Job myJob(JobRepository jobRepository, Flow importFlow, Step reportStep) {
    return new JobBuilder("myJob", jobRepository)
            .start(importFlow)
            .next(reportStep)
            .end()
            .build();
}
```

### Conditional Steps — Transitions conditionnelles

On peut définir des transitions basées sur l'`ExitStatus` d'un Step.

```java
@Bean
public Job conditionalJob(JobRepository jobRepository,
                           Step stepA, Step stepB, Step stepC, Step failureStep) {
    return new JobBuilder("conditionalJob", jobRepository)
            .start(stepA)
                .on("COMPLETED").to(stepB)
                .on("FAILED").to(failureStep)
            .from(stepB)
                .on("*").to(stepC)           // Wildcard : tous les statuts
            .from(failureStep)
                .on("*").end("FAILED")       // Terminer le job avec statut FAILED
            .end()
            .build();
}
```

### JobExecutionDecider — Logique de décision personnalisée

```java
@Component
public class FileExistsDecider implements JobExecutionDecider {

    @Value("${batch.input.file}")
    private String filePath;

    @Override
    public FlowExecutionStatus decide(JobExecution jobExecution, StepExecution stepExecution) {
        File file = new File(filePath);
        if (file.exists() && file.isFile()) {
            return new FlowExecutionStatus("FILE_EXISTS");
        } else {
            return new FlowExecutionStatus("NO_FILE");
        }
    }
}

@Bean
public Job jobWithDecision(JobRepository jobRepository,
                            FileExistsDecider decider,
                            Step downloadStep, Step importStep, Step noFileStep) {
    return new JobBuilder("jobWithDecision", jobRepository)
            .start(decider)
                .on("FILE_EXISTS").to(importStep)
                .on("NO_FILE").to(downloadStep)
            .from(downloadStep)
                .on("COMPLETED").to(importStep)
            .from(importStep)
                .on("*").end()
            .end()
            .build();
}
```

---

## 15. Spring Batch avec Spring Boot

### Auto-configuration

Spring Boot auto-configure Spring Batch. Le simple fait d'ajouter `spring-boot-starter-batch` active :
- La création automatique du `JobRepository` (en base)
- La création du `JobLauncher`
- La création des tables de métadonnées au démarrage

### Configuration `application.yml`

```yaml
spring:
  batch:
    job:
      enabled: false          # Ne pas lancer le job au démarrage automatiquement
      name: importUsersJob    # Quel job lancer si enabled=true
    jdbc:
      initialize-schema: always  # Créer les tables de métadonnées (always/never/embedded)
      table-prefix: BATCH_       # Préfixe des tables (défaut: BATCH_)

  datasource:
    url: jdbc:postgresql://localhost:5432/batchdb
    username: batchuser
    password: batchpass

  jpa:
    hibernate:
      ddl-auto: validate
```

### `@EnableBatchProcessing` (Spring Batch 4 vs 5)

```java
// Spring Batch 4 : nécessaire
@Configuration
@EnableBatchProcessing
public class BatchConfig { ... }

// Spring Batch 5 (Spring Boot 3+) : plus nécessaire
// L'auto-configuration de Spring Boot gère tout
@Configuration
public class BatchConfig { ... }
```

### @StepScope et @JobScope

Ces annotations permettent de créer des beans dont le cycle de vie est lié à l'exécution d'un step ou d'un job, et d'injecter les `JobParameters` et `StepExecutionContext` via `@Value`.

```java
// @StepScope : bean créé à chaque exécution de step, détruit à la fin
@Bean
@StepScope
public FlatFileItemReader<User> reader(
        @Value("#{jobParameters['fileName']}") String fileName,
        @Value("#{stepExecutionContext['partition.start']}") Integer start) {
    return new FlatFileItemReaderBuilder<User>()
            .name("reader")
            .resource(new FileSystemResource(fileName))
            // ...
            .build();
}

// @JobScope : bean créé à chaque JobExecution
@Bean
@JobScope
public Step conditionalStep(
        @Value("#{jobParameters['mode']}") String mode) {
    // Logique selon le mode
}
```

### Profil de configuration pour les tests

```java
@SpringBootTest
@ActiveProfiles("test")
class BatchConfigTest {

    @TestConfiguration
    static class TestBatchConfig {
        @Bean
        public DataSource dataSource() {
            return new EmbeddedDatabaseBuilder()
                    .setType(EmbeddedDatabaseType.H2)
                    .addScript("/org/springframework/batch/core/schema-h2.sql")
                    .build();
        }
    }
}
```

---

## 16. Tests Spring Batch

### Dépendance de test

```xml
<dependency>
    <groupId>org.springframework.batch</groupId>
    <artifactId>spring-batch-test</artifactId>
    <scope>test</scope>
</dependency>
```

### Test d'un Job complet

```java
@SpringBootTest
@ExtendWith(SpringExtension.class)
class ImportUsersJobTest {

    @Autowired
    private JobLauncherTestUtils jobLauncherTestUtils;

    @Autowired
    private JobRepositoryTestUtils jobRepositoryTestUtils;

    @Autowired
    private Job importUsersJob;

    @BeforeEach
    void setUp() {
        jobLauncherTestUtils.setJob(importUsersJob);
        jobRepositoryTestUtils.removeJobExecutions(); // Nettoyer entre tests
    }

    @Test
    void testImportJobCompletes() throws Exception {
        // Lancer le job avec des paramètres de test
        JobExecution execution = jobLauncherTestUtils.launchJob(
                new JobParametersBuilder()
                        .addString("fileName", "test-users.csv")
                        .addLong("timestamp", System.currentTimeMillis())
                        .toJobParameters()
        );

        // Vérifications
        assertThat(execution.getStatus()).isEqualTo(BatchStatus.COMPLETED);
        assertThat(execution.getExitStatus().getExitCode()).isEqualTo("COMPLETED");
    }

    @Test
    void testImportJobStats() throws Exception {
        JobExecution execution = jobLauncherTestUtils.launchJob();

        StepExecution stepExecution = execution.getStepExecutions()
                .stream()
                .filter(s -> s.getStepName().equals("importStep"))
                .findFirst()
                .orElseThrow();

        assertThat(stepExecution.getReadCount()).isEqualTo(100);
        assertThat(stepExecution.getWriteCount()).isEqualTo(95);
        assertThat(stepExecution.getSkipCount()).isEqualTo(5);
    }
}
```

### Test d'un Step isolé

```java
@SpringBootTest
class ImportStepTest {

    @Autowired
    private JobLauncherTestUtils jobLauncherTestUtils;

    @Test
    void testImportStepAlone() throws Exception {
        // Lancer uniquement le step "importStep"
        JobExecution execution = jobLauncherTestUtils.launchStep("importStep");

        StepExecution stepExecution = execution.getStepExecutions().iterator().next();
        assertThat(stepExecution.getStatus()).isEqualTo(BatchStatus.COMPLETED);
        assertThat(stepExecution.getWriteCount()).isGreaterThan(0);
    }
}
```

### Test d'un ItemProcessor

```java
class UserProcessorTest {

    private UserProcessor processor;

    @BeforeEach
    void setUp() {
        processor = new UserProcessor();
    }

    @Test
    void shouldMapDtoToEntity() throws Exception {
        UserDto dto = new UserDto("Anouar", "Doe", "anouar@test.com");
        UserEntity entity = processor.process(dto);

        assertThat(entity).isNotNull();
        assertThat(entity.getFirstName()).isEqualTo("ANOUAR");
        assertThat(entity.getEmail()).isEqualTo("anouar@test.com");
    }

    @Test
    void shouldFilterInvalidEmail() throws Exception {
        UserDto dto = new UserDto("Test", "User", "invalid-email");
        UserEntity result = processor.process(dto);

        assertThat(result).isNull(); // Filtré
    }
}
```

### Test avec Testcontainers

```java
@SpringBootTest
@Testcontainers
class ImportJobIntegrationTest {

    @Container
    static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15-alpine")
            .withDatabaseName("batchdb")
            .withUsername("test")
            .withPassword("test");

    @DynamicPropertySource
    static void configureDataSource(DynamicPropertyRegistry registry) {
        registry.add("spring.datasource.url", postgres::getJdbcUrl);
        registry.add("spring.datasource.username", postgres::getUsername);
        registry.add("spring.datasource.password", postgres::getPassword);
        registry.add("spring.batch.jdbc.initialize-schema", () -> "always");
    }

    @Autowired
    private JobLauncherTestUtils jobLauncherTestUtils;

    @Test
    void fullIntegrationTest() throws Exception {
        JobExecution execution = jobLauncherTestUtils.launchJob(
                new JobParametersBuilder()
                        .addString("fileName", "classpath:test-data/users.csv")
                        .toJobParameters()
        );
        assertThat(execution.getStatus()).isEqualTo(BatchStatus.COMPLETED);
    }
}
```

---

## 17. Bonnes pratiques

### 1. Toujours utiliser @StepScope pour les readers/writers paramétrés

```java
// ✅ Permet l'injection de JobParameters et la bonne gestion du cycle de vie
@Bean
@StepScope
public FlatFileItemReader<User> reader(@Value("#{jobParameters['file']}") String file) { ... }
```

### 2. Choisir le bon chunk-size

Tester avec différentes valeurs. Trop petit = overhead transactionnel. Trop grand = consommation mémoire et risque de timeout.

### 3. Rendre les readers restartables

Utiliser des readers qui implémentent `ItemStream` pour sauvegarder leur position dans l'`ExecutionContext`.

### 4. Utiliser JdbcPagingItemReader plutôt que JdbcCursorItemReader pour les gros volumes

Le `CursorItemReader` maintient une connexion ouverte. Le `PagingItemReader` fait des requêtes paginées et est plus adapté aux longs traitements.

### 5. Définir des politiques de skip et retry adaptées

```java
// Distinguer les erreurs transitoires (retry) des erreurs métier (skip)
.faultTolerant()
.retry(TransientDataAccessException.class)  // Erreur réseau/BDD temporaire → retry
.retryLimit(3)
.skip(ValidationException.class)            // Donnée invalide → skip + audit
.skipLimit(Integer.MAX_VALUE)
```

### 6. Auditer les éléments skippés

Implémenter un `SkipListener` qui persiste les enregistrements ignorés avec la raison de l'erreur pour analyse ultérieure.

### 7. Dimensionner la JVM pour le batch

```
-Xmx2g -Xms512m
-XX:+UseG1GC
-XX:MaxGCPauseMillis=200
```

### 8. Séparer les DataSources (batch vs applicatif)

```yaml
# DataSource pour les métadonnées Spring Batch
spring:
  datasource:
    batch:
      url: jdbc:postgresql://localhost/batchdb
  
  # DataSource métier
  datasource:
    business:
      url: jdbc:postgresql://localhost/businessdb
```

### 9. Monitorer avec Spring Boot Actuator

```yaml
management:
  endpoints:
    web:
      exposure:
        include: batch, health, metrics
```

```
GET /actuator/batch/jobs              → Liste des jobs
GET /actuator/batch/jobs/{jobName}    → Détails d'un job
GET /actuator/batch/jobs/{jobName}/instances/{id} → Détails d'une instance
```

### 10. Utiliser des paramètres identifying pour contrôler le restart

```java
// Le timestamp non-identifying permet de relancer sans changer les params métier
new JobParametersBuilder()
        .addString("reportMonth", "2024-01", true)   // identifying : clé métier
        .addLong("runId", System.currentTimeMillis(), false)  // non-identifying : technique
        .toJobParameters();
```

---

## 18. Questions/Réponses d'entretien

---

### 🔹 Question 1 : Qu'est-ce que Spring Batch et dans quels cas l'utiliser ?

**Réponse :**

Spring Batch est un framework Java dédié au traitement batch, c'est-à-dire au traitement massif et automatisé de données, généralement de manière planifiée. Il fournit des mécanismes essentiels comme la gestion des transactions par chunk, le restart en cas d'échec, le skip/retry d'éléments en erreur, et la traçabilité complète de chaque exécution.

On l'utilise typiquement pour : l'import/export de fichiers volumineux (CSV, XML, JSON), la synchronisation entre systèmes (ETL), les calculs périodiques (facturation, intérêts), la génération de rapports, la purge de données, ou l'envoi de notifications en masse.

---

### 🔹 Question 2 : Expliquez l'architecture de base de Spring Batch.

**Réponse :**

L'architecture centrale repose sur quelques concepts clés :

Un **Job** est le traitement batch complet, composé de un ou plusieurs **Steps**. Chaque Step peut être de type **chunk** (le plus courant : ItemReader → ItemProcessor → ItemWriter) ou **Tasklet** (tâche ponctuelle).

Le **JobLauncher** démarre les jobs avec des **JobParameters**. Le **JobRepository** persiste toutes les métadonnées d'exécution en base de données (tables BATCH_*) : instances, exécutions, statistiques, contextes.

La notion de **JobInstance** représente l'exécution logique (un job + ses paramètres identifying). La **JobExecution** représente une tentative concrète d'exécuter cette instance. Ainsi, si un job échoue et est restarté, on a deux JobExecutions pour la même JobInstance.

---

### 🔹 Question 3 : Qu'est-ce que le chunk-oriented processing ? Comment fonctionne-t-il ?

**Réponse :**

Le chunk-oriented processing est le modèle de traitement principal de Spring Batch. Son principe : lire N éléments un par un (via l'ItemReader), les transformer (via l'ItemProcessor, optionnel), accumuler jusqu'à atteindre le chunk-size, puis écrire tout le chunk d'un coup (ItemWriter) dans une seule transaction.

Le cycle pour chaque chunk : la transaction commence, le reader est appelé en boucle jusqu'à ce qu'il retourne `null` (fin des données) ou que le chunk-size soit atteint, le processor transforme ou filtre chaque élément (retour `null` = filtré), puis le writer reçoit la liste complète des éléments du chunk, et enfin la transaction est committée.

Ce modèle garantit que si l'écriture d'un chunk échoue, seul ce chunk est rollbacké. Les chunks précédents déjà committés sont conservés.

---

### 🔹 Question 4 : Quelle est la différence entre un Step de type chunk et un Tasklet ?

**Réponse :**

Un **Step chunk** suit le modèle ItemReader → ItemProcessor → ItemWriter. Il est conçu pour traiter un grand nombre d'éléments de manière itérative et transactionnelle. C'est le mode à utiliser pour les imports/exports massifs.

Un **Tasklet** est un Step simplifié pour une opération unique et ponctuelle. On implémente une interface avec une seule méthode `execute()` qui retourne `FINISHED` ou `CONTINUABLE`. Il s'exécute dans une seule transaction.

On utilise les Tasklets pour des opérations de préparation ou de nettoyage : vider une table staging avant import, déplacer un fichier après traitement, envoyer un email de fin de job, appeler une procédure stockée, etc.

---

### 🔹 Question 5 : Expliquez la différence entre JobInstance, JobExecution et StepExecution.

**Réponse :**

La **JobInstance** représente l'exécution logique d'un job identifiée par ses paramètres `identifying`. Par exemple, le job "importFactures" avec le paramètre "mois=2024-01" est une instance. On ne peut pas recréer une instance déjà complétée avec succès.

La **JobExecution** représente une tentative concrète d'exécuter cette instance. Une même JobInstance peut avoir plusieurs JobExecutions : par exemple, la première tente échoue (FAILED), on corrige le problème et on relance → une deuxième JobExecution démarre sur la même instance en reprenant là où elle s'est arrêtée.

La **StepExecution** représente l'exécution d'un Step dans le cadre d'une JobExecution. Elle contient les statistiques : `readCount`, `writeCount`, `skipCount`, `filterCount`, le status, et l'ExecutionContext du step.

---

### 🔹 Question 6 : Comment fonctionne le mécanisme de restart dans Spring Batch ?

**Réponse :**

Spring Batch supporte le restart automatique grâce à la persistance de l'`ExecutionContext` en base de données. Quand un job échoue, l'état de chaque reader est sauvegardé (position dans le fichier, dernière page lue, etc.).

Pour restarted un job, on le relance avec les **mêmes JobParameters identifying**. Spring Batch détecte qu'il existe une JobExecution en état FAILED pour cette instance et lance une nouvelle JobExecution en mode restart. Les Steps déjà complétés avec succès sont ignorés, et le Step en échec reprend depuis le dernier checkpoint.

Pour que cela fonctionne, les ItemReaders doivent être **restartables**, c'est-à-dire implémenter l'interface `ItemStream` et sauvegarder leur position dans l'ExecutionContext. Les readers built-in de Spring Batch le font automatiquement.

---

### 🔹 Question 7 : Comment gérer les erreurs avec Skip et Retry ?

**Réponse :**

Le **skip** permet de continuer le traitement en ignorant les éléments qui lèvent une exception. On configure les classes d'exceptions à ignorer (`skip()`), celles à ne jamais ignorer (`noSkip()`), et une limite maximale d'éléments ignorés (`skipLimit()`). On implémente généralement un `SkipListener` pour auditer les éléments ignorés (les sauvegarder dans une table d'erreurs avec la raison).

Le **retry** permet de retenter l'opération qui a échoué avant de considérer l'élément en erreur. On l'utilise pour les erreurs transitoires : timeout réseau, deadlock temporaire en base, service externe indisponible. On configure les exceptions à retenter, une limite de tentatives (`retryLimit()`), et éventuellement une politique de backoff (délai exponentiel entre les tentatives).

En pratique, on les combine : retry pour les erreurs transitoires, skip pour les erreurs métier, le tout avec un SkipListener qui trace les rejets.

---

### 🔹 Question 8 : Comment paralléliser un traitement Spring Batch ?

**Réponse :**

Spring Batch offre plusieurs stratégies de parallélisation :

Le **multi-threaded step** : plusieurs threads exécutent le chunk processing en parallèle. Simple à mettre en place avec `taskExecutor()` dans la configuration du Step. Attention : le reader doit être thread-safe (utiliser `SynchronizedItemStreamReader`).

Le **partitioning** : on divise les données en partitions indépendantes (par exemple, par plage d'IDs), chacune traitée par un worker step. Un master step (le Partitioner) définit les partitions, puis chaque worker traite sa partition indépendamment. Idéal pour les très gros volumes car chaque partition a son propre contexte transactionnel.

Les **parallel flows** : plusieurs Steps ou Flows s'exécutent en parallèle (par exemple, Step A et Step B en parallèle, puis Step C une fois les deux terminés).

Le choix dépend du cas : multi-threaded pour simplifier, partitioning pour les très gros volumes avec isolation forte, parallel flows pour des étapes indépendantes.

---

### 🔹 Question 9 : Qu'est-ce que @StepScope et pourquoi est-il important ?

**Réponse :**

`@StepScope` est une annotation Spring qui crée un bean avec un cycle de vie lié à l'exécution d'un Step. Concrètement, un bean `@StepScope` est instancié au démarrage du Step et détruit à sa fin.

C'est important pour deux raisons principales :

Premièrement, il permet d'injecter les `JobParameters` et les valeurs de l'`ExecutionContext` via `@Value("#{jobParameters['key']}")` et `@Value("#{stepExecutionContext['key']}")`. Sans `@StepScope`, ces valeurs ne seraient pas disponibles au moment de l'instanciation du bean (au démarrage du contexte Spring).

Deuxièmement, dans un contexte multi-threaded ou avec partitioning, chaque thread/partition obtient sa propre instance du bean, évitant les problèmes de concurrence sur des objets stateful comme les readers et writers.

---

### 🔹 Question 10 : Qu'est-ce que Testcontainers et comment l'utiliser avec Spring Batch ?

**Réponse :**

Testcontainers est une bibliothèque Java qui démarre de vrais containers Docker pendant les tests JUnit. Pour Spring Batch, cela permet de tester avec une vraie base de données PostgreSQL (par exemple) au lieu d'H2 en mémoire, ce qui garantit que le comportement des requêtes SQL, des transactions et des tables de métadonnées Spring Batch est identique à la production.

On annote le test avec `@Testcontainers`, on déclare un `@Container PostgreSQLContainer`, et on utilise `@DynamicPropertySource` pour injecter l'URL JDBC du container dans le contexte Spring. On peut ensuite utiliser `JobLauncherTestUtils` (fourni par `spring-batch-test`) pour lancer le job ou un step spécifique et vérifier les statistiques d'exécution.

En Spring Boot 3.1+ avec Testcontainers, l'annotation `@ServiceConnection` simplifie encore la configuration automatique.

---

### 🔹 Question 11 : Comment passer des paramètres à un job Spring Batch et accéder à ces paramètres dans les steps ?

**Réponse :**

On crée les paramètres avec `JobParametersBuilder` et on les passe au `JobLauncher.run()`. Dans un Step, on accède aux paramètres via `@StepScope` et l'expression SpEL `@Value("#{jobParameters['nomDuParam']}")`.

Par exemple, pour un reader paramétré par le nom du fichier :

```java
@Bean
@StepScope
public FlatFileItemReader<User> reader(
        @Value("#{jobParameters['fileName']}") String fileName) {
    return new FlatFileItemReaderBuilder<User>()
            .resource(new FileSystemResource(fileName))
            // ...
            .build();
}
```

Les paramètres `identifying` (valeur par défaut) contribuent à l'identité de la JobInstance. Un paramètre `non-identifying` (comme un timestamp de run) permet de relancer un job sans changer sa logique d'identité, utile pour réexécuter manuellement un traitement.

---

### 🔹 Question 12 : Quelle est la différence entre JdbcCursorItemReader et JdbcPagingItemReader ?

**Réponse :**

Le `JdbcCursorItemReader` exécute une seule requête SQL et maintient un curseur de base de données ouvert pendant toute la durée du Step. Simple à configurer, mais la connexion reste ouverte pendant tout le traitement, ce qui peut poser des problèmes pour les traitements longs (timeout, saturation du pool de connexions). Il n'est pas thread-safe.

Le `JdbcPagingItemReader` exécute des requêtes paginées : il récupère N enregistrements à la fois (page-size), libère la connexion entre chaque page, et reprend à la page suivante. Il est restartable nativement (il sait à quelle page il en était), thread-safe, et recommandé pour les gros volumes ou les traitements longs.

En pratique, pour un entretien Java/Spring Boot, je recommande toujours le `JdbcPagingItemReader` en production dès que le volume de données est significatif.

---

*Fin du cours
