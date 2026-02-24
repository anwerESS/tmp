Starters are “all-in-one” dependencies that automatically include all the libraries needed for a specific use case.

Advantages: `Guaranteed compatibility between dependencies` `No conflicts` `Fewer configuration errors`  

**spring-boot-starter-xxxxx**
`web`: automatically includes:  
Spring MVC (for REST APIs and web), Embedded Tomcat(server), Jackson(JSON serialization), Validation, logging  
`data-jpa`: Spring Data JPA, Hibernate (ORM), JDBC)  
`security`: Spring Security Core, Authentication, Authorization, Session Management  
`test`: JUnit 5, Mockito, AssertJ  
`actuator`: Health checks, Metrics, Application info, Env properties  


