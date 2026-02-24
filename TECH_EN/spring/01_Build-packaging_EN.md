## [**..**](./00_index.md)

Build & Packaging Java

# Maven, Build & Packaging

## 🔧 Maven - The Daily Essentials

### Everyday Maven Commands

**`mvn clean`**
- Deletes the `/target` directory
- Cleans up previously compiled files
- Use before a full build to avoid cache issues

**`mvn install`**
- Executes: compile → test → package → install
- Installs the artifact in the local `.m2` repository
- Allows other local projects to use this dependency

**`mvn clean install`**
- Most common combination
- Cleans and rebuilds the entire project
- Guarantees a clean build with no leftover artifacts

**`mvn clean install -U`**
- `-U` = `--update-snapshots`
- Forces update of SNAPSHOT dependencies from remote repositories
- **Critical**: Resolves issues with stale cached dependencies

**`mvn clean install -DskipTests`**
- Skips test execution (but still compiles them)
- Saves time during development
- ⚠️ Never use in CI/CD!

**`mvn clean install -Dmaven.test.skip=true`**
- Completely skips tests (does not even compile them)
- Faster than `-DskipTests`

**`mvn dependency:tree`**
- Displays the full dependency tree
- Essential for detecting version conflicts

**`mvn dependency:resolve`**
- Downloads all dependencies without compiling
- Useful for pre-loading the `.m2` cache

**`mvn spring-boot:run`**
- Launches the Spring Boot application directly
- Fast development without packaging

**`mvn package`**
- Creates the JAR/WAR in `/target`
- Does not install it in `.m2`

**`mvn versions:display-dependency-updates`**
- Lists dependencies that have newer versions available

### Maven Lifecycle - The 3 Cycles

#### 1. **Clean Lifecycle**
- `pre-clean` → `clean` → `post-clean`

#### 2. **Default Lifecycle** (the most important)
```
validate → compile → test → package → verify → install → deploy
```

**Key phases:**
- **validate**: Verifies the project is correct
- **compile**: Compiles source code (`/src/main/java` → `/target/classes`)
- **test**: Runs unit tests (JUnit)
- **package**: Creates the JAR/WAR
- **verify**: Runs integration tests
- **install**: Installs into `.m2/repository`
- **deploy**: Deploys to a remote repository (Nexus, Artifactory)

#### 3. **Site Lifecycle**
- `pre-site` → `site` → `post-site` → `site-deploy`
- Generates project documentation

**Important**: Each phase automatically executes all preceding phases!

### .m2 Directory - The Local Repository

**Structure:**
```
~/.m2/
├── repository/          # Local dependency cache
│   └── com/example/...
└── settings.xml         # Global Maven configuration
```

**Default location:**
- Linux/Mac: `~/.m2`
- Windows: `C:\Users\{username}\.m2`

**Why it matters:**
- Avoids downloading dependencies on every build
- Can grow quickly (several GB)
- If corrupted: delete `/repository` and rerun `mvn install`

**Clearing the cache:**
```bash
rm -rf ~/.m2/repository
mvn dependency:resolve
```

### settings.xml - Maven Configuration

**Two locations:**
1. **Global**: `$MAVEN_HOME/conf/settings.xml`
2. **User**: `~/.m2/settings.xml` (takes priority)

**Typical configuration for Spring Boot:**

```xml
<settings>
  <!-- Servers (credentials for private repositories) -->
  <servers>
    <server>
      <id>nexus-releases</id>
      <username>deployUser</username>
      <password>{encrypted-password}</password>
    </server>
  </servers>

  <!-- Mirrors (to speed up or use a proxy) -->
  <mirrors>
    <mirror>
      <id>company-nexus</id>
      <mirrorOf>central</mirrorOf>
      <url>https://nexus.company.com/repository/maven-public</url>
    </mirror>
  </mirrors>

  <!-- Profiles -->
  <profiles>
    <profile>
      <id>dev</id>
      <repositories>
        <repository>
          <id>spring-snapshots</id>
          <url>https://repo.spring.io/snapshot</url>
          <snapshots><enabled>true</enabled></snapshots>
        </repository>
      </repositories>
    </profile>
  </profiles>

  <activeProfiles>
    <activeProfile>dev</activeProfile>
  </activeProfiles>

  <!-- Proxy (if required in a corporate environment) -->
  <proxies>
    <proxy>
      <id>company-proxy</id>
      <active>true</active>
      <protocol>http</protocol>
      <host>proxy.company.com</host>
      <port>8080</port>
    </proxy>
  </proxies>
</settings>
```

### Maven Repositories

**Repository types:**

1. **Maven Central** (default)
   - Main public repository
   - URL: https://repo.maven.apache.org/maven2

2. **Spring Repositories**
   - **Releases**: https://repo.spring.io/release
   - **Milestones**: https://repo.spring.io/milestone
   - **Snapshots**: https://repo.spring.io/snapshot

3. **Enterprise repositories** (Nexus, Artifactory)
   - Hosts internal artifacts
   - Acts as a proxy/cache for Maven Central

**Configuration in pom.xml:**
```xml
<repositories>
  <repository>
    <id>spring-milestones</id>
    <n>Spring Milestones</n>
    <url>https://repo.spring.io/milestone</url>
  </repository>
</repositories>

<pluginRepositories>
  <pluginRepository>
    <id>spring-milestones</id>
    <url>https://repo.spring.io/milestone</url>
  </pluginRepository>
</pluginRepositories>
```

### POM.xml - Structure for Spring Boot

```xml
<project>
  <modelVersion>4.0.0</modelVersion>
  
  <!-- Spring Boot parent inheritance -->
  <parent>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-parent</artifactId>
    <version>3.2.0</version>
  </parent>

  <!-- Project coordinates -->
  <groupId>com.example</groupId>
  <artifactId>my-app</artifactId>
  <version>1.0.0-SNAPSHOT</version>
  <packaging>jar</packaging>

  <!-- Properties -->
  <properties>
    <java.version>17</java.version>
    <maven.compiler.source>17</maven.compiler.source>
    <maven.compiler.target>17</maven.compiler.target>
  </properties>

  <!-- Dependencies -->
  <dependencies>
    <dependency>
      <groupId>org.springframework.boot</groupId>
      <artifactId>spring-boot-starter-web</artifactId>
    </dependency>
  </dependencies>

  <!-- Plugins -->
  <build>
    <plugins>
      <plugin>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-maven-plugin</artifactId>
      </plugin>
    </plugins>
  </build>
</project>
```

### Packaging Types

**JAR (default for Spring Boot)**
```xml
<packaging>jar</packaging>
```
- Standalone application with embedded Tomcat
- Run with: `java -jar app.jar`
- **Fat JAR**: Contains all dependencies

**WAR (for deployment on an external server)**
```xml
<packaging>war</packaging>
```
- For external Tomcat, WildFly, WebLogic
- Requires a `SpringBootServletInitializer` class

### Versions - SNAPSHOT vs RELEASE

**SNAPSHOT** (`1.0.0-SNAPSHOT`)
- In-development version
- Can change with every build
- Maven checks for updates regularly
- Used during active development

**RELEASE** (`1.0.0`, `1.0.1`)
- Fixed and immutable version
- Once published, never changes
- Used in production

## 🏗️ Gradle (Maven Alternative)

**build.gradle (Groovy DSL):**
```groovy
plugins {
    id 'java'
    id 'org.springframework.boot' version '3.2.0'
}

dependencies {
    implementation 'org.springframework.boot:spring-boot-starter-web'
    testImplementation 'org.springframework.boot:spring-boot-starter-test'
}
```

**Advantages over Maven:**
- Incremental build (faster)
- More concise syntax
- Distributed build cache
- Better multi-module support

**Equivalent commands:**
- `gradle clean build` = `mvn clean install`
- `gradle bootRun` = `mvn spring-boot:run`
- `gradle dependencies` = `mvn dependency:tree`

## ⚡ GraalVM - Native Compilation

**For Spring Boot:**

```xml
<plugin>
  <groupId>org.graalvm.buildtools</groupId>
  <artifactId>native-maven-plugin</artifactId>
</plugin>
```

**Native build:**
```bash
mvn -Pnative native:compile
```

**Advantages:**
- Startup: 50ms vs 3-5s (classic JVM)
- Memory: 50MB vs 300MB+
- Perfect for microservices and serverless

**Limitations:**
- Long compilation time (5-10 min)
- Limited Java reflection (requires hints)
- Some libraries may be incompatible

## 🐳 Docker Image Builds - 3 Approaches

### 1. Dockerfile (Traditional)

**Optimal multi-stage for Spring Boot:**
```dockerfile
# Stage 1: Build
FROM maven:3.9-eclipse-temurin-17 AS build
WORKDIR /app
COPY pom.xml .
RUN mvn dependency:go-offline
COPY src ./src
RUN mvn package -DskipTests

# Stage 2: Runtime
FROM eclipse-temurin:17-jre-alpine
WORKDIR /app
COPY --from=build /app/target/*.jar app.jar
EXPOSE 8080
ENTRYPOINT ["java", "-jar", "app.jar"]
```

**Build:**
```bash
docker build -t myapp:latest .
```

**Advantages:** Full control, flexible  
**Disadvantages:** Verbose, requires Docker

### 2. Buildpacks (Cloud Native)

```bash
mvn spring-boot:build-image
```

**Or with configuration:**
```xml
<plugin>
  <groupId>org.springframework.boot</groupId>
  <artifactId>spring-boot-maven-plugin</artifactId>
  <configuration>
    <image>
      <n>myapp:${project.version}</n>
    </image>
  </configuration>
</plugin>
```

**Advantages:** Zero configuration, CNCF standards, automatically optimized  
**Disadvantages:** Less control

### 3. Google Jib (No Docker Required!)

**pom.xml configuration:**
```xml
<plugin>
  <groupId>com.google.cloud.tools</groupId>
  <artifactId>jib-maven-plugin</artifactId>
  <version>3.4.0</version>
  <configuration>
    <from>
      <image>eclipse-temurin:17-jre</image>
    </from>
    <to>
      <image>myapp</image>
      <tags>
        <tag>latest</tag>
        <tag>${project.version}</tag>
      </tags>
    </to>
    <container>
      <ports>
        <port>8080</port>
      </ports>
      <jvmFlags>
        <jvmFlag>-Xms512m</jvmFlag>
        <jvmFlag>-Xmx1024m</jvmFlag>
      </jvmFlags>
    </container>
  </configuration>
</plugin>
```

**Jib commands:**

```bash
# Build into local Docker
mvn compile jib:dockerBuild

# Push directly to registry (without Docker)
mvn compile jib:build

# With custom configuration
mvn compile jib:build -Dimage=gcr.io/myproject/myapp:v1.0
```

**Optimized layering by Jib:**
```
Layer 1: Dependencies (rarely changed)
Layer 2: Spring Boot loader
Layer 3: Snapshot dependencies
Layer 4: Resources
Layer 5: Classes (changes frequently)
```

**Jib advantages:**
- ✅ No Docker installation required
- ✅ Reproducible builds (same image = same layers)
- ✅ Intelligent caching (ultra-fast rebuilds)
- ✅ Ideal for CI/CD
- ✅ Security: no Docker daemon to expose

## 📊 Comparison of the 3 Approaches

| Criterion | Dockerfile | Buildpacks | Jib |
|---------|-----------|------------|-----|
| **Docker required** | ✅ Yes | ✅ Yes | ❌ No |
| **Speed** | Average | Fast | Very fast |
| **Simplicity** | Complex | Simple | Simple |
| **Control** | Full | Limited | Medium |
| **CI/CD** | Complex | Simple | Excellent |
| **Layering** | Manual | Auto | Optimized |

**Recommendation for Spring Boot:**
- **Development**: Jib (`mvn jib:dockerBuild`)
- **Simple production**: Buildpacks (`mvn spring-boot:build-image`)
- **Specific needs**: Dockerfile (multi-stage)

# Q&A - Build & Packaging Java

## Maven & Gradle

**Q1: What is the main difference between Maven and Gradle?**  
**A:** Maven uses XML (pom.xml) with a rigid, convention-based structure, while Gradle uses a Groovy/Kotlin DSL offering more flexibility. Gradle is generally faster thanks to incremental builds and build caching.

**Q2: What is the Maven lifecycle?**  
**A:** Maven has 3 cycles: clean, default (compile, test, package, install, deploy), and site. Each phase automatically executes all preceding phases.

**Q3: Why choose Gradle over Maven?**  
**A:** Gradle offers better performance (incremental builds), more flexibility for complex builds, and a more concise syntax. Maven is preferable for simple projects with standard conventions.

## GraalVM

**Q4: What is GraalVM and why use it?**  
**A:** GraalVM is a JVM that enables native compilation (AOT). Advantages: ultra-fast startup (milliseconds vs seconds), reduced memory footprint, perfect for microservices, serverless, and containers.

**Q5: What is the difference between JIT and AOT compilation?**  
**A:** JIT (Just-In-Time) compiles bytecode to native code at runtime (classic JVM). AOT (Ahead-Of-Time) compiles to native before execution, producing a standalone executable (GraalVM native-image).

**Q6: What are the drawbacks of GraalVM native?**  
**A:** Longer compilation time, limitations on Java reflection (requires configuration), sometimes larger executable size, and some libraries may not be compatible.

## Docker Image Build Approaches

**Q7: Explain the 3 Docker image build approaches**  
**A:**
- **Dockerfile**: Traditional approach, full control, requires Docker daemon
- **Buildpacks**: Automatic detection, CNCF standards, no Dockerfile needed
- **Jib**: Build without Docker, optimized for Java, automatic dependency layering

**Q8: What is the main advantage of Jib?**  
**A:** Jib does not require Docker to be installed. It builds the image directly from Maven/Gradle, which greatly simplifies CI/CD pipelines (no need for Docker-in-Docker).

**Q9: How does Jib optimize images?**  
**A:** Jib intelligently separates layers: dependencies (rarely change), resources, classes, and configuration. On a rebuild, only modified layers are rebuilt, drastically speeding up the process.

**Q10: Difference between `jib:build` and `jib:dockerBuild`?**  
**A:**
- `jib:dockerBuild`: Builds the image into the local Docker daemon
- `jib:build`: Pushes the image directly to a remote registry (Docker Hub, GCR, etc.) without going through local Docker

**Q11: When to use Dockerfile vs Jib?**  
**A:** Use Dockerfile for very specific needs (system installations, complex multi-stage). Use Jib for standard Java applications — it is simpler, faster, and does not require Docker.

**Q12: What is Buildpacks and when to use it?**  
**A:** Buildpacks automatically detects the application type and creates an optimized image with no configuration. Ideal for standardized deployments on cloud platforms (Heroku, Cloud Foundry, Kubernetes).

## Practical Questions

**Q13: Complete Maven command to compile and create a Docker image with Jib?**  
**A:** `mvn compile jib:dockerBuild` or simply `mvn package jib:dockerBuild`

**Q14: How do you configure Jib in pom.xml?**  
**A:**
```xml
<plugin>
  <groupId>com.google.cloud.tools</groupId>
  <artifactId>jib-maven-plugin</artifactId>
  <version>3.4.0</version>
  <configuration>
    <to>
      <image>myapp:latest</image>
    </to>
  </configuration>
</plugin>
```

**Q15: Advantages of GraalVM for containers?**  
**A:** Much smaller final images (10-50 MB vs 200+ MB), instant startup (important for auto-scaling), reduced memory consumption = more containers per node = lower costs.
