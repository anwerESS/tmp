## [**..**](./00_index.md)

## **Asynchronous Inter-Service Communication in Microservices**

Asynchronous communication is a key building block of event-driven microservice architectures. The expertise covered here includes the complete implementation of messaging solutions with **RabbitMQ** and **Kafka**, using the modern frameworks **Spring Cloud Functions** and **Spring Cloud Stream**.

### **RabbitMQ Integration**

**RabbitMQ** is a robust message broker based on the AMQP protocol, offering flexible messaging patterns for inter-service communication.

**Basic configuration:**
```java
@Configuration
public class RabbitMQConfig {
    
    @Bean
    public Queue orderQueue() {
        return new Queue("order.queue", true); // durable queue
    }
    
    @Bean
    public DirectExchange orderExchange() {
        return new DirectExchange("order.exchange");
    }
    
    @Bean
    public Binding orderBinding(Queue orderQueue, DirectExchange orderExchange) {
        return BindingBuilder
            .bind(orderQueue)
            .to(orderExchange)
            .with("order.routing.key");
    }
    
    @Bean
    public Jackson2JsonMessageConverter messageConverter() {
        return new Jackson2JsonMessageConverter();
    }
}
```

**Publisher (Producer):**
```java
@Service
public class OrderPublisher {
    private final RabbitTemplate rabbitTemplate;
    
    public OrderPublisher(RabbitTemplate rabbitTemplate) {
        this.rabbitTemplate = rabbitTemplate;
    }
    
    public void publishOrderCreatedEvent(OrderCreatedEvent event) {
        rabbitTemplate.convertAndSend(
            "order.exchange",
            "order.routing.key",
            event
        );
    }
    
    // With publish confirmation
    public void publishWithConfirmation(OrderEvent event) {
        rabbitTemplate.convertAndSend(
            "order.exchange",
            "order.routing.key",
            event,
            message -> {
                message.getMessageProperties().setDeliveryMode(MessageDeliveryMode.PERSISTENT);
                message.getMessageProperties().setHeader("event-type", event.getType());
                return message;
            }
        );
    }
}
```

**Consumer (Listener):**
```java
@Service
public class OrderConsumer {
    
    @RabbitListener(queues = "order.queue")
    public void handleOrderCreatedEvent(OrderCreatedEvent event) {
        log.info("Received order created event: {}", event.getOrderId());
        processOrder(event);
    }
    
    // With error handling and retry
    @RabbitListener(
        queues = "order.queue",
        errorHandler = "orderErrorHandler"
    )
    public void handleOrderWithRetry(OrderCreatedEvent event, 
                                      @Header(AmqpHeaders.DELIVERY_TAG) long tag,
                                      Channel channel) throws IOException {
        try {
            processOrder(event);
            channel.basicAck(tag, false); // Manual acknowledgement
        } catch (Exception e) {
            log.error("Error processing order", e);
            channel.basicNack(tag, false, true); // Requeue
        }
    }
}

@Component
public class OrderErrorHandler implements RabbitListenerErrorHandler {
    
    @Override
    public Object handleError(Message amqpMessage, org.springframework.messaging.Message<?> message,
                             ListenerExecutionFailedException exception) {
        log.error("Error handling message: {}", exception.getMessage());
        // Dead Letter Queue logic
        return null;
    }
}
```

**Dead Letter Queue (DLQ) configuration:**
```java
@Configuration
public class DLQConfig {
    
    @Bean
    public Queue orderQueue() {
        return QueueBuilder.durable("order.queue")
            .withArgument("x-dead-letter-exchange", "order.dlx")
            .withArgument("x-dead-letter-routing-key", "order.dlq")
            .withArgument("x-message-ttl", 300000) // 5 minutes
            .build();
    }
    
    @Bean
    public Queue deadLetterQueue() {
        return new Queue("order.dlq", true);
    }
    
    @Bean
    public DirectExchange deadLetterExchange() {
        return new DirectExchange("order.dlx");
    }
    
    @Bean
    public Binding dlqBinding() {
        return BindingBuilder
            .bind(deadLetterQueue())
            .to(deadLetterExchange())
            .with("order.dlq");
    }
}
```

---

### **Kafka: Setting up Consumer and Producer**

**Apache Kafka** is a high-performance distributed streaming platform, ideal for large-scale event-driven architectures.

**Kafka configuration:**
```java
@Configuration
public class KafkaProducerConfig {
    
    @Value("${spring.kafka.bootstrap-servers}")
    private String bootstrapServers;
    
    @Bean
    public ProducerFactory<String, OrderEvent> producerFactory() {
        Map<String, Object> configProps = new HashMap<>();
        configProps.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers);
        configProps.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class);
        configProps.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, JsonSerializer.class);
        configProps.put(ProducerConfig.ACKS_CONFIG, "all");
        configProps.put(ProducerConfig.RETRIES_CONFIG, 3);
        configProps.put(ProducerConfig.ENABLE_IDEMPOTENCE_CONFIG, true);
        return new DefaultKafkaProducerFactory<>(configProps);
    }
    
    @Bean
    public KafkaTemplate<String, OrderEvent> kafkaTemplate() {
        return new KafkaTemplate<>(producerFactory());
    }
}

@Configuration
public class KafkaConsumerConfig {
    
    @Value("${spring.kafka.bootstrap-servers}")
    private String bootstrapServers;
    
    @Bean
    public ConsumerFactory<String, OrderEvent> consumerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers);
        props.put(ConsumerConfig.GROUP_ID_CONFIG, "order-service-group");
        props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class);
        props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, JsonDeserializer.class);
        props.put(JsonDeserializer.TRUSTED_PACKAGES, "com.example.*");
        props.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG, "earliest");
        props.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, false);
        return new DefaultKafkaConsumerFactory<>(props);
    }
    
    @Bean
    public ConcurrentKafkaListenerContainerFactory<String, OrderEvent> kafkaListenerContainerFactory() {
        ConcurrentKafkaListenerContainerFactory<String, OrderEvent> factory = 
            new ConcurrentKafkaListenerContainerFactory<>();
        factory.setConsumerFactory(consumerFactory());
        factory.setConcurrency(3); // Number of consumer threads
        factory.getContainerProperties().setAckMode(ContainerProperties.AckMode.MANUAL);
        return factory;
    }
}
```

**Kafka Producer:**
```java
@Service
public class OrderEventProducer {
    private final KafkaTemplate<String, OrderEvent> kafkaTemplate;
    
    public OrderEventProducer(KafkaTemplate<String, OrderEvent> kafkaTemplate) {
        this.kafkaTemplate = kafkaTemplate;
    }
    
    public void sendOrderEvent(OrderEvent event) {
        kafkaTemplate.send("order-events", event.getOrderId().toString(), event);
    }
    
    // With callback
    public void sendOrderEventWithCallback(OrderEvent event) {
        CompletableFuture<SendResult<String, OrderEvent>> future = 
            kafkaTemplate.send("order-events", event.getOrderId().toString(), event);
            
        future.whenComplete((result, ex) -> {
            if (ex == null) {
                log.info("Sent message=[{}] with offset=[{}]", 
                    event, result.getRecordMetadata().offset());
            } else {
                log.error("Unable to send message=[{}] due to: {}", event, ex.getMessage());
            }
        });
    }
    
    // With custom headers
    public void sendOrderEventWithHeaders(OrderEvent event) {
        ProducerRecord<String, OrderEvent> record = new ProducerRecord<>(
            "order-events",
            event.getOrderId().toString(),
            event
        );
        
        record.headers().add("event-type", event.getType().getBytes());
        record.headers().add("correlation-id", UUID.randomUUID().toString().getBytes());
        
        kafkaTemplate.send(record);
    }
}
```

**Kafka Consumer:**
```java
@Service
public class OrderEventConsumer {
    
    @KafkaListener(
        topics = "order-events",
        groupId = "order-service-group"
    )
    public void consumeOrderEvent(OrderEvent event) {
        log.info("Consumed order event: {}", event);
        processOrder(event);
    }
    
    // With manual acknowledgement
    @KafkaListener(
        topics = "order-events",
        groupId = "order-service-group",
        containerFactory = "kafkaListenerContainerFactory"
    )
    public void consumeWithManualAck(
            @Payload OrderEvent event,
            @Header(KafkaHeaders.RECEIVED_KEY) String key,
            @Header(KafkaHeaders.RECEIVED_PARTITION) int partition,
            @Header(KafkaHeaders.OFFSET) long offset,
            Acknowledgment acknowledgment) {
        try {
            log.info("Received: key={}, partition={}, offset={}", key, partition, offset);
            processOrder(event);
            acknowledgment.acknowledge(); // Manual commit
        } catch (Exception e) {
            log.error("Error processing event", e);
            // Do not acknowledge to trigger retry
        }
    }
    
    // Multiple topics
    @KafkaListener(
        topics = {"order-events", "payment-events"},
        groupId = "notification-service-group"
    )
    public void consumeMultipleTopics(
            ConsumerRecord<String, String> record,
            @Header(KafkaHeaders.RECEIVED_TOPIC) String topic) {
        log.info("Received from topic {}: {}", topic, record.value());
    }
}
```

---

### **Creating Topics Programmatically**

**Creating Kafka topics:**
```java
@Configuration
public class KafkaTopicConfig {
    
    @Value("${spring.kafka.bootstrap-servers}")
    private String bootstrapServers;
    
    @Bean
    public KafkaAdmin kafkaAdmin() {
        Map<String, Object> configs = new HashMap<>();
        configs.put(AdminClientConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers);
        return new KafkaAdmin(configs);
    }
    
    @Bean
    public NewTopic orderEventsTopic() {
        return TopicBuilder.name("order-events")
            .partitions(3)
            .replicas(2)
            .config(TopicConfig.RETENTION_MS_CONFIG, "604800000") // 7 days
            .config(TopicConfig.COMPRESSION_TYPE_CONFIG, "snappy")
            .build();
    }
    
    @Bean
    public NewTopic paymentEventsTopic() {
        return TopicBuilder.name("payment-events")
            .partitions(5)
            .replicas(2)
            .compact() // Compaction enabled
            .build();
    }
    
    @Bean
    public NewTopic notificationEventsTopic() {
        return new NewTopic("notification-events", 3, (short) 2);
    }
}
```

**Dynamic topic creation:**
```java
@Service
public class KafkaTopicService {
    private final KafkaAdmin kafkaAdmin;
    
    public KafkaTopicService(KafkaAdmin kafkaAdmin) {
        this.kafkaAdmin = kafkaAdmin;
    }
    
    public void createTopic(String topicName, int partitions, short replicationFactor) {
        NewTopic newTopic = TopicBuilder.name(topicName)
            .partitions(partitions)
            .replicas(replicationFactor)
            .build();
            
        kafkaAdmin.createOrModifyTopics(newTopic);
        log.info("Topic {} created successfully", topicName);
    }
    
    public void deleteTopic(String topicName) {
        kafkaAdmin.deleteTopics(topicName);
        log.info("Topic {} deleted", topicName);
    }
    
    public boolean topicExists(String topicName) {
        try (AdminClient adminClient = AdminClient.create(kafkaAdmin.getConfigurationProperties())) {
            Set<String> topics = adminClient.listTopics().names().get();
            return topics.contains(topicName);
        } catch (Exception e) {
            log.error("Error checking topic existence", e);
            return false;
        }
    }
}
```

---

### **Serializing and Deserializing Java Objects**

**Custom Serializer:**
```java
public class OrderEventSerializer implements Serializer<OrderEvent> {
    private final ObjectMapper objectMapper = new ObjectMapper();
    
    @Override
    public void configure(Map<String, ?> configs, boolean isKey) {
        objectMapper.registerModule(new JavaTimeModule());
        objectMapper.disable(SerializationFeature.WRITE_DATES_AS_TIMESTAMPS);
    }
    
    @Override
    public byte[] serialize(String topic, OrderEvent data) {
        if (data == null) {
            return null;
        }
        try {
            return objectMapper.writeValueAsBytes(data);
        } catch (JsonProcessingException e) {
            throw new SerializationException("Error serializing OrderEvent", e);
        }
    }
}
```

**Custom Deserializer:**
```java
public class OrderEventDeserializer implements Deserializer<OrderEvent> {
    private final ObjectMapper objectMapper = new ObjectMapper();
    
    @Override
    public void configure(Map<String, ?> configs, boolean isKey) {
        objectMapper.registerModule(new JavaTimeModule());
        objectMapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
    }
    
    @Override
    public OrderEvent deserialize(String topic, byte[] data) {
        if (data == null) {
            return null;
        }
        try {
            return objectMapper.readValue(data, OrderEvent.class);
        } catch (IOException e) {
            throw new SerializationException("Error deserializing OrderEvent", e);
        }
    }
}
```

**Configuration with custom serializers:**
```java
@Configuration
public class CustomSerializationConfig {
    
    @Bean
    public ProducerFactory<String, OrderEvent> customProducerFactory() {
        Map<String, Object> configProps = new HashMap<>();
        configProps.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers);
        configProps.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class);
        configProps.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, OrderEventSerializer.class);
        return new DefaultKafkaProducerFactory<>(configProps);
    }
    
    @Bean
    public ConsumerFactory<String, OrderEvent> customConsumerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers);
        props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class);
        props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, OrderEventDeserializer.class);
        return new DefaultKafkaConsumerFactory<>(props);
    }
}
```

**Avro Serialization (schema registry):**
```java
@Configuration
public class AvroSerializationConfig {
    
    @Bean
    public ProducerFactory<String, GenericRecord> avroProducerFactory() {
        Map<String, Object> configProps = new HashMap<>();
        configProps.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers);
        configProps.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class);
        configProps.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, KafkaAvroSerializer.class);
        configProps.put("schema.registry.url", "http://localhost:8081");
        return new DefaultKafkaProducerFactory<>(configProps);
    }
}
```

---

### **Spring Cloud Functions**

**Spring Cloud Functions** offers a modern functional approach for building event-driven microservices, with a unified abstraction over messaging platforms.

**Function-based approach:**
```java
@Configuration
public class OrderFunctions {
    
    // Supplier - Producer
    @Bean
    public Supplier<OrderEvent> orderEventSupplier() {
        return () -> {
            OrderEvent event = generateOrderEvent();
            log.info("Supplying order event: {}", event);
            return event;
        };
    }
    
    // Consumer - Simple processing
    @Bean
    public Consumer<OrderEvent> orderEventConsumer() {
        return event -> {
            log.info("Consuming order event: {}", event);
            processOrder(event);
        };
    }
    
    // Function - Transformation
    @Bean
    public Function<OrderEvent, PaymentEvent> orderToPayment() {
        return orderEvent -> {
            log.info("Transforming order to payment: {}", orderEvent.getOrderId());
            return PaymentEvent.builder()
                .orderId(orderEvent.getOrderId())
                .amount(orderEvent.getTotalAmount())
                .timestamp(Instant.now())
                .build();
        };
    }
    
    // BiFunction - Combining streams
    @Bean
    public BiFunction<OrderEvent, CustomerEvent, EnrichedOrderEvent> enrichOrder() {
        return (orderEvent, customerEvent) -> {
            log.info("Enriching order {} with customer {}", 
                orderEvent.getOrderId(), customerEvent.getCustomerId());
            return EnrichedOrderEvent.builder()
                .orderEvent(orderEvent)
                .customerEvent(customerEvent)
                .enrichedAt(Instant.now())
                .build();
        };
    }
}
```

**Reactive functions:**
```java
@Configuration
public class ReactiveOrderFunctions {
    
    @Bean
    public Function<Flux<OrderEvent>, Flux<PaymentEvent>> processOrders() {
        return orderFlux -> orderFlux
            .doOnNext(order -> log.info("Processing order: {}", order.getOrderId()))
            .filter(order -> order.getTotalAmount().compareTo(BigDecimal.ZERO) > 0)
            .map(order -> convertToPayment(order))
            .delayElements(Duration.ofMillis(100))
            .doOnNext(payment -> log.info("Created payment: {}", payment.getPaymentId()));
    }
    
    @Bean
    public Consumer<Flux<OrderEvent>> batchOrderProcessor() {
        return orderFlux -> orderFlux
            .buffer(Duration.ofSeconds(5))
            .subscribe(orders -> {
                log.info("Processing batch of {} orders", orders.size());
                processBatch(orders);
            });
    }
}
```

**Configuration in application.yml:**
```yaml
spring:
  cloud:
    function:
      definition: orderEventSupplier;orderEventConsumer;orderToPayment
    stream:
      bindings:
        orderEventSupplier-out-0:
          destination: order-events
          content-type: application/json
        orderEventConsumer-in-0:
          destination: order-events
          group: order-service-group
        orderToPayment-in-0:
          destination: order-events
        orderToPayment-out-0:
          destination: payment-events
```

---

### **Spring Cloud Stream**

**Spring Cloud Stream** provides a high-level abstraction for building event-driven microservices, with native support for RabbitMQ and Kafka.

**Spring Cloud Stream configuration with Kafka:**
```yaml
spring:
  cloud:
    stream:
      kafka:
        binder:
          brokers: localhost:9092
          auto-create-topics: true
          configuration:
            key.serializer: org.apache.kafka.common.serialization.StringSerializer
            value.serializer: org.springframework.kafka.support.serializer.JsonSerializer
      bindings:
        orderProcessor-in-0:
          destination: order-events
          group: order-service-group
          consumer:
            max-attempts: 3
            back-off-initial-interval: 1000
            back-off-multiplier: 2
        orderProcessor-out-0:
          destination: processed-orders
          producer:
            partition-key-expression: headers['partitionKey']
            partition-count: 3
```

**Stream processing with Spring Cloud Stream:**
```java
@Configuration
public class OrderStreamProcessor {
    
    @Bean
    public Function<Message<OrderEvent>, Message<ProcessedOrder>> orderProcessor() {
        return message -> {
            OrderEvent event = message.getPayload();
            log.info("Processing order event: {}", event.getOrderId());
            
            // Business logic
            ProcessedOrder processed = processOrderEvent(event);
            
            // Add headers
            return MessageBuilder
                .withPayload(processed)
                .setHeader("partitionKey", event.getCustomerId())
                .setHeader("event-type", "ORDER_PROCESSED")
                .build();
        };
    }
    
    @Bean
    public Consumer<Message<OrderEvent>> orderValidator() {
        return message -> {
            OrderEvent event = message.getPayload();
            Map<String, Object> headers = message.getHeaders();
            
            log.info("Validating order: {}, Headers: {}", event.getOrderId(), headers);
            
            if (!isValid(event)) {
                throw new ValidationException("Invalid order");
            }
        };
    }
}
```

---

### **Kafka Topics, Consumer Groups, and Partitions**

**Architecture with partitions:**
```java
@Configuration
public class KafkaPartitionConfig {
    
    @Bean
    public NewTopic orderEventsPartitionedTopic() {
        return TopicBuilder.name("order-events-partitioned")
            .partitions(6) // 6 partitions for parallelism
            .replicas(3)   // 3 replicas for HA
            .config(TopicConfig.MIN_IN_SYNC_REPLICAS_CONFIG, "2")
            .build();
    }
}
```

**Producer with partition key:**
```java
@Service
public class PartitionedOrderProducer {
    private final KafkaTemplate<String, OrderEvent> kafkaTemplate;
    
    public void sendOrderEvent(OrderEvent event) {
        // Use customerId as partition key
        // Guarantees all events from the same customer go to the same partition
        String partitionKey = event.getCustomerId().toString();
        
        kafkaTemplate.send("order-events-partitioned", partitionKey, event)
            .whenComplete((result, ex) -> {
                if (ex == null) {
                    RecordMetadata metadata = result.getRecordMetadata();
                    log.info("Sent to partition: {}, offset: {}", 
                        metadata.partition(), metadata.offset());
                } else {
                    log.error("Failed to send message", ex);
                }
            });
    }
    
    // Custom partitioner
    public void sendWithCustomPartition(OrderEvent event) {
        int partition = calculatePartition(event);
        
        ProducerRecord<String, OrderEvent> record = new ProducerRecord<>(
            "order-events-partitioned",
            partition,
            event.getOrderId().toString(),
            event
        );
        
        kafkaTemplate.send(record);
    }
    
    private int calculatePartition(OrderEvent event) {
        // Business logic to determine the partition
        return Math.abs(event.getCustomerId().hashCode()) % 6;
    }
}
```

**Consumer groups for scalability:**
```java
@Service
public class ScalableOrderConsumer {
    
    // Consumer Group 1 - Order Processing
    @KafkaListener(
        topics = "order-events-partitioned",
        groupId = "order-processing-group",
        concurrency = "3" // 3 instances in the same group
    )
    public void processOrder(
            @Payload OrderEvent event,
            @Header(KafkaHeaders.RECEIVED_PARTITION) int partition) {
        log.info("Processing order {} from partition {}", event.getOrderId(), partition);
        processOrderLogic(event);
    }
    
    // Consumer Group 2 - Analytics (consumes all messages)
    @KafkaListener(
        topics = "order-events-partitioned",
        groupId = "analytics-group"
    )
    public void analyzeOrder(OrderEvent event) {
        log.info("Analyzing order: {}", event.getOrderId());
        performAnalytics(event);
    }
    
    // Consumer Group 3 - Notification
    @KafkaListener(
        topics = "order-events-partitioned",
        groupId = "notification-group"
    )
    public void notifyOrder(OrderEvent event) {
        log.info("Sending notification for order: {}", event.getOrderId());
        sendNotification(event);
    }
}
```

**Advanced consumer group configuration:**
```yaml
spring:
  kafka:
    consumer:
      group-id: order-service-group
      auto-offset-reset: earliest
      enable-auto-commit: false
      max-poll-records: 500
      properties:
        max.poll.interval.ms: 300000
        session.timeout.ms: 30000
        heartbeat.interval.ms: 10000
        partition.assignment.strategy: org.apache.kafka.clients.consumer.RangeAssignor
```

**Rebalancing and partition assignment:**
```java
@Component
public class ConsumerRebalanceHandler implements ConsumerRebalanceListener {
    
    @Override
    public void onPartitionsRevoked(Collection<TopicPartition> partitions) {
        log.warn("Partitions revoked: {}", partitions);
        // Cleanup logic before rebalancing
        partitions.forEach(partition -> {
            log.info("Releasing partition: {}", partition.partition());
            // Flush pending operations
        });
    }
    
    @Override
    public void onPartitionsAssigned(Collection<TopicPartition> partitions) {
        log.info("Partitions assigned: {}", partitions);
        // Initialization logic after rebalancing
        partitions.forEach(partition -> {
            log.info("Assigned partition: {}", partition.partition());
            // Setup partition-specific resources
        });
    }
}
```

---

### **RabbitMQ with Spring Cloud Stream**

**Spring Cloud Stream configuration with RabbitMQ:**
```yaml
spring:
  cloud:
    stream:
      default-binder: rabbit
      rabbit:
        bindings:
          orderProcessor-in-0:
            consumer:
              auto-bind-dlq: true
              dlq-ttl: 300000
              dlq-dead-letter-exchange: order-dlx
              republish-to-dlq: true
          orderProcessor-out-0:
            producer:
              routing-key-expression: headers['routingKey']
              delayed-exchange: true
      bindings:
        orderProcessor-in-0:
          destination: order.exchange
          group: order-service-group
          consumer:
            max-attempts: 3
            back-off-initial-interval: 1000
        orderProcessor-out-0:
          destination: processed-order.exchange
```

**Functions with RabbitMQ:**
```java
@Configuration
public class RabbitMQStreamFunctions {
    
    @Bean
    public Function<Message<OrderEvent>, Message<ProcessedOrder>> orderProcessor() {
        return message -> {
            OrderEvent event = message.getPayload();
            
            // Extract RabbitMQ specific headers
            String routingKey = (String) message.getHeaders().get("amqp_receivedRoutingKey");
            String exchange = (String) message.getHeaders().get("amqp_receivedExchange");
            
            log.info("Received from exchange: {}, routing key: {}", exchange, routingKey);
            
            ProcessedOrder processed = processOrder(event);
            
            // Set custom routing key for output
            return MessageBuilder
                .withPayload(processed)
                .setHeader("routingKey", determineRoutingKey(processed))
                .setHeader("priority", processed.getPriority())
                .build();
        };
    }
    
    @Bean
    public Consumer<Message<OrderEvent>> priorityOrderConsumer() {
        return message -> {
            OrderEvent event = message.getPayload();
            Integer priority = (Integer) message.getHeaders().get("priority");
            
            log.info("Processing priority {} order: {}", priority, event.getOrderId());
            processPriorityOrder(event, priority);
        };
    }
}
```

---

### **Event-Driven Microservices Architecture**

**Message Microservice with Spring Cloud Functions:**
```java
@SpringBootApplication
public class MessageMicroserviceApplication {
    public static void main(String[] args) {
        SpringApplication.run(MessageMicroserviceApplication.class, args);
    }
}

@Configuration
public class MessageServiceFunctions {
    
    private final NotificationService notificationService;
    
    public MessageServiceFunctions(NotificationService notificationService) {
        this.notificationService = notificationService;
    }
    
    // Consume events from different topics
    @Bean
    public Consumer<OrderEvent> orderEventHandler() {
        return event -> {
            log.info("Received order event: {}", event.getOrderId());
            notificationService.sendOrderNotification(event);
        };
    }
    
    @Bean
    public Consumer<PaymentEvent> paymentEventHandler() {
        return event -> {
            log.info("Received payment event: {}", event.getPaymentId());
            notificationService.sendPaymentNotification(event);
        };
    }
    
    @Bean
    public Consumer<ShipmentEvent> shipmentEventHandler() {
        return event -> {
            log.info("Received shipment event: {}", event.getShipmentId());
            notificationService.sendShipmentNotification(event);
        };
    }
}
```

**Event choreography pattern:**
```java
@Service
public class OrderOrchestrationService {
    private final OrderRepository orderRepository;
    private final StreamBridge streamBridge;
    
    public OrderOrchestrationService(OrderRepository orderRepository,
                                    StreamBridge streamBridge) {
        this.orderRepository = orderRepository;
        this.streamBridge = streamBridge;
    }
    
    // Start the workflow
    public void createOrder(CreateOrderRequest request) {
        Order order = orderRepository.save(new Order(request));
        
        // Publish OrderCreated event
        OrderCreatedEvent event = OrderCreatedEvent.from(order);
        streamBridge.send("order-created", event);
        
        log.info("Order created event published: {}", order.getId());
    }
    
    // React to events from other services
    @Bean
    public Consumer<PaymentCompletedEvent> handlePaymentCompleted() {
        return event -> {
            log.info("Payment completed for order: {}", event.getOrderId());
            
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow(() -> new OrderNotFoundException(event.getOrderId()));
            
            order.setStatus(OrderStatus.PAID);
            orderRepository.save(order);
            
            // Publish the next event in the chain
            OrderPaidEvent orderPaidEvent = OrderPaidEvent.from(order);
            streamBridge.send("order-paid", orderPaidEvent);
            
            log.info("Order paid event published: {}", order.getId());
        };
    }
    
    @Bean
    public Consumer<InventoryReservedEvent> handleInventoryReserved() {
        return event -> {
            log.info("Inventory reserved for order: {}", event.getOrderId());
            
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow(() -> new OrderNotFoundException(event.getOrderId()));
            
            order.setStatus(OrderStatus.INVENTORY_RESERVED);
            orderRepository.save(order);
            
            // Trigger shipment
            ShipmentRequestEvent shipmentEvent = ShipmentRequestEvent.from(order);
            streamBridge.send("shipment-request", shipmentEvent);
            
            log.info("Shipment request event published: {}", order.getId());
        };
    }
    
    @Bean
    public Consumer<ShipmentCompletedEvent> handleShipmentCompleted() {
        return event -> {
            log.info("Shipment completed for order: {}", event.getOrderId());
            
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow(() -> new OrderNotFoundException(event.getOrderId()));
            
            order.setStatus(OrderStatus.SHIPPED);
            orderRepository.save(order);
            
            // Final event
            OrderCompletedEvent completedEvent = OrderCompletedEvent.from(order);
            streamBridge.send("order-completed", completedEvent);
            
            log.info("Order completed event published: {}", order.getId());
        };
    }
}
```

**Event sourcing pattern:**
```java
@Service
public class EventSourcingOrderService {
    private final EventStore eventStore;
    private final StreamBridge streamBridge;
    
    public EventSourcingOrderService(EventStore eventStore, StreamBridge streamBridge) {
        this.eventStore = eventStore;
        this.streamBridge = streamBridge;
    }
    
    public Order createOrder(CreateOrderCommand command) {
        // Create the event
        OrderCreatedEvent event = OrderCreatedEvent.builder()
            .orderId(UUID.randomUUID())
            .customerId(command.getCustomerId())
            .items(command.getItems())
            .totalAmount(command.getTotalAmount())
            .timestamp(Instant.now())
            .version(1L)
            .build();
        
        // Persist the event
        eventStore.saveEvent(event);
        
        // Publish the event
        streamBridge.send("order-events", event);
        
        // Reconstruct the aggregate from events
        return reconstructOrderFromEvents(event.getOrderId());
    }
    
    public Order updateOrderStatus(UpdateOrderStatusCommand command) {
        Order currentOrder = reconstructOrderFromEvents(command.getOrderId());
        
        OrderStatusUpdatedEvent event = OrderStatusUpdatedEvent.builder()
            .orderId(command.getOrderId())
            .previousStatus(currentOrder.getStatus())
            .newStatus(command.getNewStatus())
            .timestamp(Instant.now())
            .version(currentOrder.getVersion() + 1)
            .build();
        
        eventStore.saveEvent(event);
        streamBridge.send("order-events", event);
        
        return reconstructOrderFromEvents(command.getOrderId());
    }
    
    private Order reconstructOrderFromEvents(UUID orderId) {
        List<DomainEvent> events = eventStore.getEventsForAggregate(orderId);
        
        Order order = new Order();
        events.forEach(event -> order.apply(event));
        
        return order;
    }
}

@Entity
public class EventStore {
    
    @Id
    @GeneratedValue
    private Long id;
    
    private UUID aggregateId;
    private String eventType;
    private String eventData;
    private Long version;
    private Instant timestamp;
    
    // Repository methods
}
```

**CQRS Pattern with Event-Driven:**
```java
// Command Side
@Service
public class OrderCommandService {
    private final OrderRepository orderRepository;
    private final StreamBridge streamBridge;
    
    @Transactional
    public OrderCommandResult createOrder(CreateOrderCommand command) {
        // Validation
        validateCommand(command);
        
        // Create order
        Order order = Order.builder()
            .customerId(command.getCustomerId())
            .items(command.getItems())
            .status(OrderStatus.PENDING)
            .build();
        
        order = orderRepository.save(order);
        
        // Publish event
        OrderCreatedEvent event = mapToEvent(order);
        streamBridge.send("order-events", event);
        
        return OrderCommandResult.success(order.getId());
    }
}

// Query Side
@Service
public class OrderQueryService {
    private final OrderReadRepository orderReadRepository;
    
    public OrderDTO getOrder(UUID orderId) {
        return orderReadRepository.findById(orderId)
            .map(this::mapToDTO)
            .orElseThrow(() -> new OrderNotFoundException(orderId));
    }
    
    public List<OrderDTO> getOrdersByCustomer(UUID customerId) {
        return orderReadRepository.findByCustomerId(customerId)
            .stream()
            .map(this::mapToDTO)
            .collect(Collectors.toList());
    }
    
    // Event handler to update the read view
    @Bean
    public Consumer<OrderEvent> orderEventProjection() {
        return event -> {
            switch (event.getType()) {
                case ORDER_CREATED:
                    handleOrderCreated((OrderCreatedEvent) event);
                    break;
                case ORDER_UPDATED:
                    handleOrderUpdated((OrderUpdatedEvent) event);
                    break;
                case ORDER_CANCELLED:
                    handleOrderCancelled((OrderCancelledEvent) event);
                    break;
            }
        };
    }
    
    private void handleOrderCreated(OrderCreatedEvent event) {
        OrderReadModel readModel = OrderReadModel.from(event);
        orderReadRepository.save(readModel);
    }
}
```

---

### **Saga Pattern for Distributed Transactions**

**Choreography-based Saga:**
```java
@Service
public class OrderSaga {
    private final StreamBridge streamBridge;
    private final OrderRepository orderRepository;
    
    // Step 1: Order Created
    public void startOrderSaga(Order order) {
        order.setStatus(OrderStatus.SAGA_STARTED);
        orderRepository.save(order);
        
        OrderCreatedEvent event = OrderCreatedEvent.from(order);
        streamBridge.send("order-created", event);
    }
    
    // Step 2: Payment Success
    @Bean
    public Consumer<PaymentSuccessEvent> handlePaymentSuccess() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.PAYMENT_COMPLETED);
            orderRepository.save(order);
            
            // Continue saga - reserve inventory
            InventoryReservationRequest request = new InventoryReservationRequest(
                event.getOrderId(), 
                order.getItems()
            );
            streamBridge.send("inventory-reservation-request", request);
        };
    }
    
    // Step 3: Payment Failure - Compensation
    @Bean
    public Consumer<PaymentFailedEvent> handlePaymentFailed() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.PAYMENT_FAILED);
            orderRepository.save(order);
            
            // Compensating transaction
            OrderCancelledEvent cancelEvent = OrderCancelledEvent.from(order);
            streamBridge.send("order-cancelled", cancelEvent);
            
            log.error("Order saga failed at payment step: {}", event.getOrderId());
        };
    }
    
    // Step 4: Inventory Reserved
    @Bean
    public Consumer<InventoryReservedEvent> handleInventoryReserved() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.INVENTORY_RESERVED);
            orderRepository.save(order);
            
            // Continue saga - create shipment
            ShipmentCreationRequest shipmentRequest = new ShipmentCreationRequest(
                event.getOrderId(),
                order.getShippingAddress()
            );
            streamBridge.send("shipment-creation-request", shipmentRequest);
        };
    }
    
    // Step 5: Inventory Reservation Failed - Compensation
    @Bean
    public Consumer<InventoryReservationFailedEvent> handleInventoryFailed() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.INVENTORY_FAILED);
            orderRepository.save(order);
            
            // Compensate - refund payment
            PaymentRefundRequest refundRequest = new PaymentRefundRequest(
                event.getOrderId(),
                order.getPaymentId()
            );
            streamBridge.send("payment-refund-request", refundRequest);
            
            log.error("Order saga failed at inventory step: {}", event.getOrderId());
        };
    }
    
    // Step 6: Saga Completed
    @Bean
    public Consumer<ShipmentCreatedEvent> handleShipmentCreated() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.SAGA_COMPLETED);
            order.setShipmentId(event.getShipmentId());
            orderRepository.save(order);
            
            // Final event
            OrderCompletedEvent completedEvent = OrderCompletedEvent.from(order);
            streamBridge.send("order-completed", completedEvent);
            
            log.info("Order saga completed successfully: {}", event.getOrderId());
        };
    }
}
```

**Orchestration-based Saga:**
```java
@Service
public class OrderSagaOrchestrator {
    private final SagaStateRepository sagaStateRepository;
    private final StreamBridge streamBridge;
    
    public void startSaga(CreateOrderCommand command) {
        // Create saga state
        SagaState sagaState = SagaState.builder()
            .sagaId(UUID.randomUUID())
            .orderId(command.getOrderId())
            .currentStep(SagaStep.ORDER_CREATED)
            .status(SagaStatus.STARTED)
            .build();
        
        sagaStateRepository.save(sagaState);
        
        // Execute first step
        executeNextStep(sagaState);
    }
    
    private void executeNextStep(SagaState sagaState) {
        switch (sagaState.getCurrentStep()) {
            case ORDER_CREATED:
                processPayment(sagaState);
                break;
            case PAYMENT_COMPLETED:
                reserveInventory(sagaState);
                break;
            case INVENTORY_RESERVED:
                createShipment(sagaState);
                break;
            case SHIPMENT_CREATED:
                completeSaga(sagaState);
                break;
        }
    }
    
    private void processPayment(SagaState sagaState) {
        PaymentRequest request = buildPaymentRequest(sagaState);
        
        Message<PaymentRequest> message = MessageBuilder
            .withPayload(request)
            .setHeader("sagaId", sagaState.getSagaId())
            .setHeader("sagaStep", SagaStep.PAYMENT_COMPLETED)
            .build();
        
        streamBridge.send("payment-request", message);
    }
    
    @Bean
    public Consumer<Message<PaymentResponse>> handlePaymentResponse() {
        return message -> {
            PaymentResponse response = message.getPayload();
            UUID sagaId = (UUID) message.getHeaders().get("sagaId");
            
            SagaState sagaState = sagaStateRepository.findById(sagaId)
                .orElseThrow();
            
            if (response.isSuccess()) {
                sagaState.setCurrentStep(SagaStep.PAYMENT_COMPLETED);
                sagaStateRepository.save(sagaState);
                executeNextStep(sagaState);
            } else {
                compensate(sagaState, SagaStep.PAYMENT_COMPLETED);
            }
        };
    }
    
    private void compensate(SagaState sagaState, SagaStep failedStep) {
        log.error("Saga compensation started for saga: {}, failed at: {}", 
            sagaState.getSagaId(), failedStep);
        
        sagaState.setStatus(SagaStatus.COMPENSATING);
        sagaStateRepository.save(sagaState);
        
        // Execute compensation in reverse order
        switch (failedStep) {
            case SHIPMENT_CREATED:
                cancelShipment(sagaState);
            case INVENTORY_RESERVED:
                releaseInventory(sagaState);
            case PAYMENT_COMPLETED:
                refundPayment(sagaState);
                break;
        }
        
        sagaState.setStatus(SagaStatus.COMPENSATED);
        sagaStateRepository.save(sagaState);
    }
}
```

---

### **Error Handling and Resilience**

**Dead Letter Queue with retry:**
```java
@Configuration
public class ErrorHandlingConfig {
    
    @Bean
    public NewTopic orderEventsDLQ() {
        return TopicBuilder.name("order-events.DLQ")
            .partitions(3)
            .replicas(2)
            .build();
    }
    
    @Bean
    public ConsumerFactory<String, OrderEvent> errorHandlingConsumerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers);
        props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class);
        props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, ErrorHandlingDeserializer.class);
        props.put(ErrorHandlingDeserializer.VALUE_DESERIALIZER_CLASS, JsonDeserializer.class);
        props.put(JsonDeserializer.TRUSTED_PACKAGES, "*");
        return new DefaultKafkaConsumerFactory<>(props);
    }
}

@Service
public class ResilientOrderConsumer {
    private final KafkaTemplate<String, OrderEvent> kafkaTemplate;
    
    @KafkaListener(
        topics = "order-events",
        groupId = "order-service-group",
        errorHandler = "kafkaErrorHandler"
    )
    public void consumeWithRetry(
            @Payload OrderEvent event,
            @Header(KafkaHeaders.RECEIVED_TOPIC) String topic,
            @Header(KafkaHeaders.OFFSET) long offset,
            @Header(value = "retry-count", required = false) Integer retryCount) {
        
        int currentRetry = retryCount != null ? retryCount : 0;
        
        try {
            log.info("Processing order event (attempt {}): {}", currentRetry + 1, event.getOrderId());
            processOrder(event);
        } catch (RecoverableException e) {
            if (currentRetry < 3) {
                // Retry with backoff
                sendToRetryTopic(event, currentRetry + 1);
            } else {
                // Send to DLQ after max retries
                sendToDLQ(event, e);
            }
        } catch (NonRecoverableException e) {
            // Send directly to DLQ
            sendToDLQ(event, e);
        }
    }
    
    private void sendToRetryTopic(OrderEvent event, int retryCount) {
        ProducerRecord<String, OrderEvent> record = new ProducerRecord<>(
            "order-events.retry",
            event.getOrderId().toString(),
            event
        );
        
        record.headers().add("retry-count", String.valueOf(retryCount).getBytes());
        record.headers().add("original-topic", "order-events".getBytes());
        
        kafkaTemplate.send(record);
        log.warn("Sent to retry topic (attempt {}): {}", retryCount, event.getOrderId());
    }
    
    private void sendToDLQ(OrderEvent event, Exception e) {
        ProducerRecord<String, OrderEvent> record = new ProducerRecord<>(
            "order-events.DLQ",
            event.getOrderId().toString(),
            event
        );
        
        record.headers().add("error-message", e.getMessage().getBytes());
        record.headers().add("error-class", e.getClass().getName().getBytes());
        record.headers().add("failed-at", Instant.now().toString().getBytes());
        
        kafkaTemplate.send(record);
        log.error("Sent to DLQ: {}, Error: {}", event.getOrderId(), e.getMessage());
    }
}

@Component
public class KafkaErrorHandler implements ConsumerAwareListenerErrorHandler {
    
    @Override
    public Object handleError(Message<?> message, ListenerExecutionFailedException exception,
                             Consumer<?, ?> consumer) {
        log.error("Error in consumer: {}", exception.getMessage());
        
        // Log error metrics
        recordErrorMetrics(message, exception);
        
        // Custom error handling logic
        if (exception.getCause() instanceof DeserializationException) {
            log.error("Deserialization error, message will be skipped");
            return null; // Skip message
        }
        
        throw exception; // Rethrow for retry logic
    }
    
    private void recordErrorMetrics(Message<?> message, Exception exception) {
        // Metrics recording logic
    }
}
```

**Circuit Breaker with event-driven:**
```java
@Service
public class ResilientEventPublisher {
    private final CircuitBreaker circuitBreaker;
    private final KafkaTemplate<String, OrderEvent> kafkaTemplate;
    private final FallbackEventStore fallbackEventStore;
    
    public ResilientEventPublisher(
            CircuitBreakerFactory circuitBreakerFactory,
            KafkaTemplate<String, OrderEvent> kafkaTemplate,
            FallbackEventStore fallbackEventStore) {
        this.circuitBreaker = circuitBreakerFactory.create("kafka-publisher");
        this.kafkaTemplate = kafkaTemplate;
        this.fallbackEventStore = fallbackEventStore;
    }
    
    public void publishEvent(OrderEvent event) {
        circuitBreaker.run(
            () -> {
                kafkaTemplate.send("order-events", event.getOrderId().toString(), event)
                    .get(5, TimeUnit.SECONDS);
                return null;
            },
            throwable -> {
                log.error("Circuit breaker opened, storing event locally", throwable);
                fallbackEventStore.store(event);
                return null;
            }
        );
    }
}

// Background job to republish stored events
@Scheduled(fixedDelay = 60000)
public void republishStoredEvents() {
    List<OrderEvent> storedEvents = fallbackEventStore.retrieveAll();
    
    storedEvents.forEach(event -> {
        try {
            kafkaTemplate.send("order-events", event.getOrderId().toString(), event)
                .get(5, TimeUnit.SECONDS);
            fallbackEventStore.remove(event.getId());
            log.info("Republished stored event: {}", event.getOrderId());
        } catch (Exception e) {
            log.warn("Failed to republish event: {}", event.getOrderId());
        }
    });
}
```

---

### **Monitoring and Observability**

**Metrics and tracing for event-driven:**
```java
@Configuration
public class EventObservabilityConfig {
    
    @Bean
    public MeterRegistryCustomizer<MeterRegistry> metricsCommonTags() {
        return registry -> registry.config().commonTags(
            "application", "order-service",
            "environment", "production"
        );
    }
}

@Service
public class ObservableOrderEventProducer {
    private final KafkaTemplate<String, OrderEvent> kafkaTemplate;
    private final MeterRegistry meterRegistry;
    private final Counter eventPublishedCounter;
    private final Timer eventPublishTimer;
    
    public ObservableOrderEventProducer(
            KafkaTemplate<String, OrderEvent> kafkaTemplate,
            MeterRegistry meterRegistry) {
        this.kafkaTemplate = kafkaTemplate;
        this.meterRegistry = meterRegistry;
        
        this.eventPublishedCounter = Counter.builder("events.published")
            .description("Number of events published")
            .tag("event-type", "order")
            .register(meterRegistry);
            
        this.eventPublishTimer = Timer.builder("events.publish.duration")
            .description("Time taken to publish events")
            .register(meterRegistry);
    }
    
    public void publishEvent(OrderEvent event) {
        eventPublishTimer.record(() -> {
            try {
                kafkaTemplate.send("order-events", event.getOrderId().toString(), event)
                    .whenComplete((result, ex) -> {
                        if (ex == null) {
                            eventPublishedCounter.increment();
                            recordPublishSuccess(event);
                        } else {
                            recordPublishFailure(event, ex);
                        }
                    });
            } catch (Exception e) {
                recordPublishFailure(event, e);
                throw e;
            }
        });
    }
    
    private void recordPublishSuccess(OrderEvent event) {
        meterRegistry.counter("events.published.success",
            "event-type", event.getType().toString()
        ).increment();
    }
    
    private void recordPublishFailure(OrderEvent event, Throwable ex) {
        meterRegistry.counter("events.published.failure",
            "event-type", event.getType().toString(),
            "error", ex.getClass().getSimpleName()
        ).increment();
    }
}

@Aspect
@Component
public class EventProcessingAspect {
    private final MeterRegistry meterRegistry;
    
    @Around("@annotation(org.springframework.kafka.annotation.KafkaListener)")
    public Object monitorEventProcessing(ProceedingJoinPoint joinPoint) throws Throwable {
        String methodName = joinPoint.getSignature().getName();
        Timer.Sample sample = Timer.start(meterRegistry);
        
        try {
            Object result = joinPoint.proceed();
            
            sample.stop(Timer.builder("events.processing.duration")
                .tag("method", methodName)
                .tag("status", "success")
                .register(meterRegistry));
            
            meterRegistry.counter("events.processed",
                "method", methodName,
                "status", "success"
            ).increment();
            
            return result;
        } catch (Exception e) {
            sample.stop(Timer.builder("events.processing.duration")
                .tag("method", methodName)
                .tag("status", "failure")
                .register(meterRegistry));
            
            meterRegistry.counter("events.processed",
                "method", methodName,
                "status", "failure",
                "error", e.getClass().getSimpleName()
            ).increment();
            
            throw e;
        }
    }
}
```

**Distributed tracing configuration:**
```yaml
management:
  tracing:
    sampling:
      probability: 1.0
  zipkin:
    tracing:
      endpoint: http://localhost:9411/api/v2/spans
  metrics:
    distribution:
      percentiles-histogram:
        kafka.consumer.fetch.manager.records.lag: true
        kafka.producer.request.latency: true
```

---

This deep expertise in asynchronous inter-service communication covers all modern patterns (Event-Driven, CQRS, Event Sourcing, Saga), the key technologies (Kafka, RabbitMQ), and the Spring Cloud frameworks (Functions, Stream), allowing you to build robust, scalable, and resilient microservice architectures.

Here are detailed Q&A for an interview on asynchronous inter-service communication:

---

## **Q&A - Asynchronous Inter-Service Communication**

### **General Questions**

**Q1: What is the difference between synchronous and asynchronous communication in microservices?**

**A:**

**Synchronous Communication:**
- The client waits for an immediate response from the service
- Strong temporal coupling between services
- Blocking I/O
- Example: REST API with RestClient
```java
// The thread is blocked until the response arrives
Order order = restClient.get()
    .uri("/api/orders/{id}", orderId)
    .retrieve()
    .body(Order.class);
```

**Asynchronous Communication:**
- The client does not wait for an immediate response
- Temporal decoupling between services
- Non-blocking, event-driven
- Services can operate independently
- Example: Message broker (Kafka, RabbitMQ)
```java
// Publishes the event and continues
kafkaTemplate.send("order-events", orderEvent);
// No waiting - the consumer will process the event later
```

**Advantages of async:**
- Better scalability
- Increased resilience (services can be temporarily unavailable)
- Strong decoupling between services
- Handles traffic spikes via buffering

**Disadvantages:**
- More complexity (eventual consistency)
- Harder to debug
- Requires a message broker
- More complex error handling

---

### **Questions about RabbitMQ**

**Q2: How does RabbitMQ work and what are its key concepts?**

**A:** RabbitMQ is a message broker based on the AMQP protocol with several key components:

**Main concepts:**

**1. Producer (Publisher):** Sends messages
```java
public void publishMessage(OrderEvent event) {
    rabbitTemplate.convertAndSend(
        "order.exchange",      // Exchange
        "order.routing.key",   // Routing key
        event                  // Message
    );
}
```

**2. Exchange:** Routes messages to queues
- **Direct Exchange:** Exact routing by key
- **Topic Exchange:** Pattern-based routing (wildcards)
- **Fanout Exchange:** Broadcasts to all queues
- **Headers Exchange:** Routing by headers

```java
// Direct Exchange
@Bean
public DirectExchange orderExchange() {
    return new DirectExchange("order.exchange");
}

// Topic Exchange
@Bean
public TopicExchange notificationExchange() {
    return new TopicExchange("notification.exchange");
}

// Fanout Exchange
@Bean
public FanoutExchange auditExchange() {
    return new FanoutExchange("audit.exchange");
}
```

**3. Queue:** Stores messages
```java
@Bean
public Queue orderQueue() {
    return QueueBuilder.durable("order.queue")
        .withArgument("x-message-ttl", 60000)
        .withArgument("x-max-length", 10000)
        .build();
}
```

**4. Binding:** Links exchange and queue
```java
@Bean
public Binding orderBinding() {
    return BindingBuilder
        .bind(orderQueue())
        .to(orderExchange())
        .with("order.created");
}

// Topic binding with wildcard
@Bean
public Binding notificationBinding() {
    return BindingBuilder
        .bind(notificationQueue())
        .to(notificationExchange())
        .with("order.*.notification"); // * = one word, # = 0 or more
}
```

**5. Consumer:** Consumes messages
```java
@RabbitListener(queues = "order.queue")
public void handleOrderEvent(OrderEvent event) {
    processOrder(event);
}
```

**Q3: How do you configure a Dead Letter Queue (DLQ) with RabbitMQ?**

**A:** The DLQ captures messages that cannot be processed:

```java
@Configuration
public class DLQConfiguration {
    
    // Main queue with DLQ configured
    @Bean
    public Queue orderQueue() {
        return QueueBuilder.durable("order.queue")
            .withArgument("x-dead-letter-exchange", "order.dlx")
            .withArgument("x-dead-letter-routing-key", "order.dlq")
            .withArgument("x-message-ttl", 300000) // 5 min TTL
            .build();
    }
    
    // Dead Letter Exchange
    @Bean
    public DirectExchange deadLetterExchange() {
        return new DirectExchange("order.dlx");
    }
    
    // Dead Letter Queue
    @Bean
    public Queue deadLetterQueue() {
        return QueueBuilder.durable("order.dlq")
            .build();
    }
    
    // Binding DLX -> DLQ
    @Bean
    public Binding dlqBinding() {
        return BindingBuilder
            .bind(deadLetterQueue())
            .to(deadLetterExchange())
            .with("order.dlq");
    }
}
```

**Consumer with rejection to DLQ:**
```java
@Service
public class OrderConsumer {
    
    @RabbitListener(queues = "order.queue")
    public void handleOrder(OrderEvent event, 
                           @Header(AmqpHeaders.DELIVERY_TAG) long tag,
                           Channel channel) throws IOException {
        try {
            processOrder(event);
            channel.basicAck(tag, false); // Success
        } catch (RecoverableException e) {
            // Reject and requeue
            channel.basicNack(tag, false, true);
        } catch (NonRecoverableException e) {
            // Reject to DLQ (no requeue)
            channel.basicNack(tag, false, false);
        }
    }
}

// Consumer for the DLQ
@RabbitListener(queues = "order.dlq")
public void handleDeadLetters(OrderEvent event,
                              @Header("x-death") List<Map<String, ?>> xDeath) {
    log.error("Processing dead letter: {}", event.getOrderId());
    log.error("Death info: {}", xDeath);
    
    // Alerting, logging, manual intervention
    alertOperations(event);
}
```

**Q4: How do you handle the retry mechanism with RabbitMQ?**

**A:** Several retry strategies are available:

**1. Retry with delay queue:**
```java
@Configuration
public class RetryConfiguration {
    
    // Main queue
    @Bean
    public Queue orderQueue() {
        return QueueBuilder.durable("order.queue")
            .withArgument("x-dead-letter-exchange", "order.retry.exchange")
            .withArgument("x-dead-letter-routing-key", "order.retry")
            .build();
    }
    
    // Retry queue with TTL
    @Bean
    public Queue retryQueue() {
        return QueueBuilder.durable("order.retry.queue")
            .withArgument("x-message-ttl", 5000) // 5 seconds
            .withArgument("x-dead-letter-exchange", "order.exchange")
            .withArgument("x-dead-letter-routing-key", "order.routing.key")
            .build();
    }
    
    @Bean
    public DirectExchange retryExchange() {
        return new DirectExchange("order.retry.exchange");
    }
    
    @Bean
    public Binding retryBinding() {
        return BindingBuilder
            .bind(retryQueue())
            .to(retryExchange())
            .with("order.retry");
    }
}
```

**2. Retry with exponential backoff:**
```java
@Service
public class RetryableOrderConsumer {
    private static final int MAX_RETRIES = 3;
    
    @RabbitListener(queues = "order.queue")
    public void handleOrder(OrderEvent event,
                           @Header(value = "x-retry-count", required = false) Integer retryCount,
                           @Header(AmqpHeaders.DELIVERY_TAG) long tag,
                           Channel channel) throws IOException {
        
        int currentRetry = retryCount != null ? retryCount : 0;
        
        try {
            processOrder(event);
            channel.basicAck(tag, false);
        } catch (Exception e) {
            if (currentRetry < MAX_RETRIES) {
                // Calculate backoff delay
                long delay = calculateBackoff(currentRetry);
                
                // Send to retry queue with delay
                rabbitTemplate.convertAndSend(
                    "order.retry.exchange",
                    "order.retry",
                    event,
                    message -> {
                        message.getMessageProperties().setExpiration(String.valueOf(delay));
                        message.getMessageProperties().setHeader("x-retry-count", currentRetry + 1);
                        return message;
                    }
                );
                channel.basicAck(tag, false);
            } else {
                // Max retries exceeded, send to DLQ
                channel.basicNack(tag, false, false);
            }
        }
    }
    
    private long calculateBackoff(int retryCount) {
        // Exponential backoff: 1s, 2s, 4s, 8s
        return (long) (Math.pow(2, retryCount) * 1000);
    }
}
```

**Q5: How do you ensure message persistence with RabbitMQ?**

**A:** Persistence requires several configurations:

**1. Durable queue:**
```java
@Bean
public Queue durableQueue() {
    return QueueBuilder.durable("order.queue") // durable = true
        .build();
}
```

**2. Durable exchange:**
```java
@Bean
public DirectExchange durableExchange() {
    return new DirectExchange("order.exchange", true, false);
    // name, durable, auto-delete
}
```

**3. Persistent messages:**
```java
public void publishPersistentMessage(OrderEvent event) {
    rabbitTemplate.convertAndSend(
        "order.exchange",
        "order.routing.key",
        event,
        message -> {
            message.getMessageProperties()
                .setDeliveryMode(MessageDeliveryMode.PERSISTENT);
            return message;
        }
    );
}
```

**Global configuration:**
```yaml
spring:
  rabbitmq:
    template:
      default-receive-queue: order.queue
      mandatory: true
      exchange: order.exchange
    listener:
      simple:
        acknowledge-mode: manual
        prefetch: 10
        default-requeue-rejected: false
```

**Publisher confirms:**
```java
@Configuration
public class PublisherConfirmsConfig {
    
    @Bean
    public RabbitTemplate rabbitTemplate(ConnectionFactory connectionFactory) {
        RabbitTemplate template = new RabbitTemplate(connectionFactory);
        template.setConfirmCallback((correlationData, ack, cause) -> {
            if (ack) {
                log.info("Message confirmed: {}", correlationData);
            } else {
                log.error("Message not confirmed: {}, cause: {}", correlationData, cause);
            }
        });
        template.setReturnsCallback(returned -> {
            log.error("Message returned: {}", returned.getMessage());
        });
        return template;
    }
}

// Configuration
spring:
  rabbitmq:
    publisher-confirm-type: correlated
    publisher-returns: true
```

---

### **Questions about Kafka**

**Q6: What is the difference between RabbitMQ and Kafka?**

**A:**

| Aspect | RabbitMQ | Kafka |
|--------|----------|-------|
| **Type** | Message Broker | Distributed Streaming Platform |
| **Model** | Push (broker pushes to consumer) | Pull (consumer pulls from broker) |
| **Persistence** | Messages deleted after consumption | Messages kept based on retention |
| **Performance** | ~20K msg/sec | ~Millions msg/sec |
| **Ordering** | Per queue | Per partition |
| **Use Case** | Task queues, RPC, complex routing | Event streaming, logs, metrics |
| **Protocol** | AMQP | Custom binary protocol |

**When to use RabbitMQ:**
- Complex routing (topic, headers exchange)
- Message prioritization
- Workflows with retry/DLQ
- Low latency required
- Request/reply pattern

**When to use Kafka:**
- High throughput needed
- Event sourcing / CQRS
- Stream processing
- Event replay
- Log aggregation
- Real-time analytics

**Q7: Explain the concepts of topics, partitions, and consumer groups in Kafka.**

**A:**

**Topics:**
- Logical category of messages
- Comparable to a database table
```java
@Bean
public NewTopic orderEventsTopic() {
    return TopicBuilder.name("order-events")
        .partitions(3)
        .replicas(2)
        .build();
}
```

**Partitions:**
- Physical division of a topic
- Enables parallelism and scalability
- Messages are ordered within a partition
- Each partition = ordered log

```
Topic: order-events (3 partitions)
Partition 0: [msg1, msg2, msg5, msg8, ...]
Partition 1: [msg3, msg6, msg9, ...]
Partition 2: [msg4, msg7, msg10, ...]
```

**Partition Key:**
```java
// Messages with the same key go to the same partition
public void publishOrder(OrderEvent event) {
    String key = event.getCustomerId().toString();
    kafkaTemplate.send("order-events", key, event);
    // All events from the same customer are ordered
}
```

**Consumer Groups:**
- Group of consumers that share partitions
- Each partition is assigned to only one consumer in the group
- Enables horizontal scalability

```
Topic: order-events (6 partitions)
Consumer Group: order-processing-group (3 consumers)

Consumer 1: partitions [0, 1]
Consumer 2: partitions [2, 3]
Consumer 3: partitions [4, 5]
```

**Configuration:**
```java
@KafkaListener(
    topics = "order-events",
    groupId = "order-processing-group",
    concurrency = "3" // 3 consumer threads
)
public void processOrder(OrderEvent event) {
    // Each consumer processes its assigned partitions
}
```

**Multiple consumer groups:**
```java
// Group 1 - Order Processing
@KafkaListener(
    topics = "order-events",
    groupId = "order-processing-group"
)
public void processOrder(OrderEvent event) {
    processOrderLogic(event);
}

// Group 2 - Analytics (consumes the same events)
@KafkaListener(
    topics = "order-events",
    groupId = "analytics-group"
)
public void analyzeOrder(OrderEvent event) {
    performAnalytics(event);
}

// Group 3 - Notification
@KafkaListener(
    topics = "order-events",
    groupId = "notification-group"
)
public void notifyOrder(OrderEvent event) {
    sendNotification(event);
}
```

**Q8: How do offsets work in Kafka?**

**A:** Offsets represent the position of a message in a partition:

**Types of offsets:**
- **Current Offset:** Current position of the consumer
- **Committed Offset:** Last confirmed position
- **Log-End Offset:** End of the log (last message)

**Auto-commit vs Manual commit:**

**1. Auto-commit (default):**
```yaml
spring:
  kafka:
    consumer:
      enable-auto-commit: true
      auto-commit-interval: 5000 # 5 seconds
```

**Risks:**
- Message loss if crash before processing
- Messages processed multiple times after rebalance

**2. Manual commit:**
```yaml
spring:
  kafka:
    consumer:
      enable-auto-commit: false
```

```java
@KafkaListener(
    topics = "order-events",
    groupId = "order-service-group"
)
public void processOrder(
        @Payload OrderEvent event,
        @Header(KafkaHeaders.OFFSET) long offset,
        Acknowledgment acknowledgment) {
    try {
        log.info("Processing message at offset: {}", offset);
        processOrderLogic(event);
        
        // Manual commit after successful processing
        acknowledgment.acknowledge();
    } catch (Exception e) {
        log.error("Error processing message at offset: {}", offset, e);
        // Do not commit = message will be reprocessed
    }
}
```

**3. Batch commit:**
```java
@KafkaListener(
    topics = "order-events",
    groupId = "batch-processing-group",
    containerFactory = "batchKafkaListenerContainerFactory"
)
public void processBatch(
        List<OrderEvent> events,
        Acknowledgment acknowledgment) {
    try {
        events.forEach(this::processOrder);
        acknowledgment.acknowledge(); // Commit the full batch
    } catch (Exception e) {
        // The entire batch will be reprocessed
    }
}
```

**Offset management:**
```java
@Service
public class OffsetManagementService {
    private final KafkaListenerEndpointRegistry registry;
    
    // Seek to specific offset
    public void seekToOffset(String listenerId, int partition, long offset) {
        MessageListenerContainer container = registry.getListenerContainer(listenerId);
        container.pausePartition(new TopicPartition("order-events", partition));
        
        // Seek
        Consumer<?, ?> consumer = getConsumer(container);
        consumer.seek(new TopicPartition("order-events", partition), offset);
        
        container.resumePartition(new TopicPartition("order-events", partition));
    }
    
    // Seek to beginning
    public void reprocessFromBeginning(String topic, int partition) {
        Consumer<?, ?> consumer = getConsumer();
        TopicPartition topicPartition = new TopicPartition(topic, partition);
        consumer.seekToBeginning(Collections.singletonList(topicPartition));
    }
    
    // Get current offset
    public long getCurrentOffset(String topic, int partition) {
        Consumer<?, ?> consumer = getConsumer();
        TopicPartition topicPartition = new TopicPartition(topic, partition);
        return consumer.position(topicPartition);
    }
}
```

**Q9: How do you create Kafka topics programmatically?**

**A:** Several approaches to create topics:

**1. Configuration Bean (at startup):**
```java
@Configuration
public class KafkaTopicConfig {
    
    @Bean
    public KafkaAdmin kafkaAdmin() {
        Map<String, Object> configs = new HashMap<>();
        configs.put(AdminClientConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        return new KafkaAdmin(configs);
    }
    
    @Bean
    public NewTopic orderEventsTopic() {
        return TopicBuilder.name("order-events")
            .partitions(6)
            .replicas(3)
            .config(TopicConfig.RETENTION_MS_CONFIG, "604800000") // 7 days
            .config(TopicConfig.COMPRESSION_TYPE_CONFIG, "snappy")
            .config(TopicConfig.MIN_IN_SYNC_REPLICAS_CONFIG, "2")
            .build();
    }
    
    @Bean
    public NewTopic paymentEventsTopic() {
        return TopicBuilder.name("payment-events")
            .partitions(3)
            .replicas(2)
            .compact() // Log compaction enabled
            .build();
    }
    
    @Bean
    public NewTopic notificationEventsTopic() {
        Map<String, String> configs = new HashMap<>();
        configs.put(TopicConfig.SEGMENT_MS_CONFIG, "3600000"); // 1 hour
        configs.put(TopicConfig.MAX_MESSAGE_BYTES_CONFIG, "1048576"); // 1MB
        
        return TopicBuilder.name("notification-events")
            .partitions(5)
            .replicas(2)
            .configs(configs)
            .build();
    }
}
```

**2. Dynamic creation via service:**
```java
@Service
public class KafkaTopicService {
    private final AdminClient adminClient;
    
    public KafkaTopicService(KafkaAdmin kafkaAdmin) {
        this.adminClient = AdminClient.create(
            kafkaAdmin.getConfigurationProperties()
        );
    }
    
    public void createTopic(String topicName, int partitions, short replicationFactor) {
        NewTopic newTopic = new NewTopic(topicName, partitions, replicationFactor);
        
        Map<String, String> configs = new HashMap<>();
        configs.put(TopicConfig.RETENTION_MS_CONFIG, "86400000"); // 1 day
        configs.put(TopicConfig.CLEANUP_POLICY_CONFIG, "delete");
        newTopic.configs(configs);
        
        CreateTopicsResult result = adminClient.createTopics(
            Collections.singleton(newTopic)
        );
        
        try {
            result.all().get();
            log.info("Topic {} created successfully", topicName);
        } catch (Exception e) {
            log.error("Failed to create topic {}", topicName, e);
        }
    }
    
    public void deleteTopic(String topicName) {
        DeleteTopicsResult result = adminClient.deleteTopics(
            Collections.singleton(topicName)
        );
        
        try {
            result.all().get();
            log.info("Topic {} deleted", topicName);
        } catch (Exception e) {
            log.error("Failed to delete topic {}", topicName, e);
        }
    }
    
    public boolean topicExists(String topicName) {
        try {
            ListTopicsResult result = adminClient.listTopics();
            Set<String> topics = result.names().get();
            return topics.contains(topicName);
        } catch (Exception e) {
            log.error("Error checking topic existence", e);
            return false;
        }
    }
    
    public Map<String, TopicDescription> describeTopic(String topicName) {
        try {
            DescribeTopicsResult result = adminClient.describeTopics(
                Collections.singleton(topicName)
            );
            return result.all().get();
        } catch (Exception e) {
            log.error("Error describing topic", e);
            return Collections.emptyMap();
        }
    }
    
    public void increasePartitions(String topicName, int totalPartitions) {
        Map<String, NewPartitions> newPartitions = new HashMap<>();
        newPartitions.put(topicName, NewPartitions.increaseTo(totalPartitions));
        
        CreatePartitionsResult result = adminClient.createPartitions(newPartitions);
        
        try {
            result.all().get();
            log.info("Increased partitions for topic {} to {}", topicName, totalPartitions);
        } catch (Exception e) {
            log.error("Failed to increase partitions", e);
        }
    }
}
```

**3. Conditional creation:**
```java
@Component
public class TopicInitializer implements ApplicationRunner {
    private final KafkaTopicService kafkaTopicService;
    
    @Override
    public void run(ApplicationArguments args) {
        String topicName = "order-events";
        
        if (!kafkaTopicService.topicExists(topicName)) {
            kafkaTopicService.createTopic(topicName, 6, (short) 2);
            log.info("Created topic: {}", topicName);
        } else {
            log.info("Topic {} already exists", topicName);
        }
    }
}
```

**YAML Configuration:**
```yaml
spring:
  kafka:
    bootstrap-servers: localhost:9092
    admin:
      auto-create: true
      properties:
        request.timeout.ms: 30000
    producer:
      key-serializer: org.apache.kafka.common.serialization.StringSerializer
      value-serializer: org.springframework.kafka.support.serializer.JsonSerializer
```

**Q10: How do you serialize/deserialize complex Java objects with Kafka?**

**A:** Several serialization strategies are available:

**1. JSON Serialization (Spring Kafka):**
```java
@Configuration
public class KafkaJsonConfig {
    
    @Bean
    public ProducerFactory<String, OrderEvent> jsonProducerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class);
        props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, JsonSerializer.class);
        
        // JSON configuration
        props.put(JsonSerializer.ADD_TYPE_INFO_HEADERS, false);
        props.put(JsonSerializer.TYPE_MAPPINGS, 
            "orderEvent:com.example.events.OrderEvent");
        
        return new DefaultKafkaProducerFactory<>(props);
    }
    
    @Bean
    public ConsumerFactory<String, OrderEvent> jsonConsumerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class);
        props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, JsonDeserializer.class);
        
        // JSON configuration
        props.put(JsonDeserializer.TRUSTED_PACKAGES, "com.example.*");
        props.put(JsonDeserializer.VALUE_DEFAULT_TYPE, OrderEvent.class);
        props.put(JsonDeserializer.USE_TYPE_INFO_HEADERS, false);
        
        return new DefaultKafkaConsumerFactory<>(props);
    }
}
```

**2. Custom Serializer/Deserializer:**
```java
public class OrderEventSerializer implements Serializer<OrderEvent> {
    private final ObjectMapper objectMapper;
    
    public OrderEventSerializer() {
        this.objectMapper = new ObjectMapper();
        objectMapper.registerModule(new JavaTimeModule());
        objectMapper.disable(SerializationFeature.WRITE_DATES_AS_TIMESTAMPS);
        objectMapper.setSerializationInclusion(JsonInclude.Include.NON_NULL);
    }
    
    @Override
    public void configure(Map<String, ?> configs, boolean isKey) {
        // Optional configuration
    }
    
    @Override
    public byte[] serialize(String topic, OrderEvent data) {
        if (data == null) {
            return null;
        }
        
        try {
            return objectMapper.writeValueAsBytes(data);
        } catch (JsonProcessingException e) {
            throw new SerializationException("Error serializing OrderEvent", e);
        }
    }
    
    @Override
    public void close() {
        // Cleanup if needed
    }
}

public class OrderEventDeserializer implements Deserializer<OrderEvent> {
    private final ObjectMapper objectMapper;
    
    public OrderEventDeserializer() {
        this.objectMapper = new ObjectMapper();
        objectMapper.registerModule(new JavaTimeModule());
        objectMapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        objectMapper.configure(DeserializationFeature.FAIL_ON_NULL_FOR_PRIMITIVES, false);
    }
    
    @Override
    public void configure(Map<String, ?> configs, boolean isKey) {
        // Optional configuration
    }
    
    @Override
    public OrderEvent deserialize(String topic, byte[] data) {
        if (data == null || data.length == 0) {
            return null;
        }
        
        try {
            return objectMapper.readValue(data, OrderEvent.class);
        } catch (IOException e) {
            throw new SerializationException("Error deserializing OrderEvent", e);
        }
    }
    
    @Override
    public void close() {
        // Cleanup if needed
    }
}
```

**Configuration with custom serializers:**
```java
@Configuration
public class CustomSerializationConfig {
    
    @Bean
    public ProducerFactory<String, OrderEvent> customProducerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class);
        props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, OrderEventSerializer.class);
        
        return new DefaultKafkaProducerFactory<>(props);
    }
    
    @Bean
    public ConsumerFactory<String, OrderEvent> customConsumerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class);
        props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, OrderEventDeserializer.class);
        
        return new DefaultKafkaConsumerFactory<>(props);
    }
}
```

**3. Avro Serialization with Schema Registry:**
```java
@Configuration
public class AvroSerializationConfig {
    
    @Value("${spring.kafka.properties.schema.registry.url}")
    private String schemaRegistryUrl;
    
    @Bean
    public ProducerFactory<String, OrderEventAvro> avroProducerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class);
        props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, 
            io.confluent.kafka.serializers.KafkaAvroSerializer.class);
        props.put("schema.registry.url", schemaRegistryUrl);
        props.put("value.subject.name.strategy", 
            io.confluent.kafka.serializers.subject.TopicRecordNameStrategy.class);
        
        return new DefaultKafkaProducerFactory<>(props);
    }
    
    @Bean
    public ConsumerFactory<String, OrderEventAvro> avroConsumerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class);
        props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, 
            io.confluent.kafka.serializers.KafkaAvroDeserializer.class);
        props.put("schema.registry.url", schemaRegistryUrl);
        props.put("specific.avro.reader", true);
        
        return new DefaultKafkaConsumerFactory<>(props);
    }
}
```

**Avro Schema (order-event.avsc):**
```json
{
  "type": "record",
  "name": "OrderEventAvro",
  "namespace": "com.example.avro",
  "fields": [
    {"name": "orderId", "type": "string"},
    {"name": "customerId", "type": "string"},
    {"name": "totalAmount", "type": "double"},
    {"name": "status", "type": "string"},
    {"name": "timestamp", "type": "long", "logicalType": "timestamp-millis"}
  ]
}
```

**4. Protobuf Serialization:**
```java
public class ProtobufSerializer implements Serializer<OrderEventProto> {
    
    @Override
    public byte[] serialize(String topic, OrderEventProto data) {
        if (data == null) {
            return null;
        }
        return data.toByteArray();
    }
}

public class ProtobufDeserializer implements Deserializer<OrderEventProto> {
    
    @Override
    public OrderEventProto deserialize(String topic, byte[] data) {
        if (data == null || data.length == 0) {
            return null;
        }
        try {
            return OrderEventProto.parseFrom(data);
        } catch (InvalidProtocolBufferException e) {
            throw new SerializationException("Error deserializing protobuf", e);
        }
    }
}
```

**5. Polymorphic Serialization (multiple types):**
```java
@Configuration
public class PolymorphicSerializationConfig {
    
    @Bean
    public ProducerFactory<String, DomainEvent> polymorphicProducerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class);
        props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, JsonSerializer.class);
        props.put(JsonSerializer.TYPE_MAPPINGS, 
            "orderCreated:com.example.OrderCreatedEvent," +
            "orderPaid:com.example.OrderPaidEvent," +
            "orderShipped:com.example.OrderShippedEvent"
        );
        
        return new DefaultKafkaProducerFactory<>(props);
    }
    
    @Bean
    public ConsumerFactory<String, DomainEvent> polymorphicConsumerFactory() {
        Map<String, Object> props = new HashMap<>();
        props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class);
        props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, JsonDeserializer.class);
        props.put(JsonDeserializer.TRUSTED_PACKAGES, "com.example.*");
        props.put(JsonDeserializer.TYPE_MAPPINGS,
            "orderCreated:com.example.OrderCreatedEvent," +
            "orderPaid:com.example.OrderPaidEvent," +
            "orderShipped:com.example.OrderShippedEvent"
        );
        
        return new DefaultKafkaConsumerFactory<>(props);
    }
}

// Consumer handling multiple event types
@KafkaListener(topics = "order-events", groupId = "polymorphic-group")
public void handleEvent(DomainEvent event) {
    switch (event) {
        case OrderCreatedEvent created -> handleOrderCreated(created);
        case OrderPaidEvent paid -> handleOrderPaid(paid);
        case OrderShippedEvent shipped -> handleOrderShipped(shipped);
        default -> log.warn("Unknown event type: {}", event.getClass());
    }
}
```

---

### **Questions about Spring Cloud Functions**

**Q11: What is Spring Cloud Functions and what are its advantages?**

**A:** Spring Cloud Functions is a framework for building event-driven applications with a modern functional approach.

**Advantages:**
- **Portability:** Code independent of the message broker (Kafka, RabbitMQ, etc.)
- **Simplicity:** Pure functional approach (Supplier, Consumer, Function)
- **Testing:** Easy to test (pure functions)
- **Abstraction:** Decouples from messaging infrastructure
- **Composition:** Function chaining

**Types of functions:**

**1. Supplier (Producer):**
```java
@Configuration
public class OrderSupplierFunctions {
    
    // Generates events periodically
    @Bean
    public Supplier<OrderEvent> orderEventSupplier() {
        return () -> {
            OrderEvent event = generateOrderEvent();
            log.info("Supplying order event: {}", event.getOrderId());
            return event;
        };
    }
    
    // Continuous reactive flux
    @Bean
    public Supplier<Flux<OrderEvent>> orderEventStreamSupplier() {
        return () -> Flux.interval(Duration.ofSeconds(5))
            .map(tick -> generateOrderEvent())
            .doOnNext(event -> log.info("Generated: {}", event.getOrderId()));
    }
}
```

**2. Consumer (Event Handler):**
```java
@Configuration
public class OrderConsumerFunctions {
    private final OrderService orderService;
    
    public OrderConsumerFunctions(OrderService orderService) {
        this.orderService = orderService;
    }
    
    // Simple consumer
    @Bean
    public Consumer<OrderEvent> orderEventConsumer() {
        return event -> {
            log.info("Consuming order event: {}", event.getOrderId());
            orderService.processOrder(event);
        };
    }
    
    // Reactive consumer with batch processing
    @Bean
    public Consumer<Flux<OrderEvent>> batchOrderConsumer() {
        return orderFlux -> orderFlux
            .buffer(Duration.ofSeconds(5))
            .doOnNext(batch -> log.info("Processing batch of {} orders", batch.size()))
            .flatMap(batch -> Flux.fromIterable(batch)
                .flatMap(order -> orderService.processOrderAsync(order)))
            .subscribe();
    }
    
    // Consumer with Message wrapper (access headers)
    @Bean
    public Consumer<Message<OrderEvent>> orderEventConsumerWithHeaders() {
        return message -> {
            OrderEvent event = message.getPayload();
            Map<String, Object> headers = message.getHeaders();
            
            log.info("Event: {}, Headers: {}", event.getOrderId(), headers);
            orderService.processOrder(event);
        };
    }
}
```

**3. Function (Transformation):**
```java
@Configuration
public class OrderTransformationFunctions {
    
    // Simple transformation
    @Bean
    public Function<OrderEvent, PaymentEvent> orderToPayment() {
        return orderEvent -> {
            log.info("Transforming order {} to payment", orderEvent.getOrderId());
            return PaymentEvent.builder()
                .orderId(orderEvent.getOrderId())
                .amount(orderEvent.getTotalAmount())
                .customerId(orderEvent.getCustomerId())
                .timestamp(Instant.now())
                .build();
        };
    }
    
    // Reactive transformation with filtering
    @Bean
    public Function<Flux<OrderEvent>, Flux<OrderEvent>> validateOrders() {
        return orderFlux -> orderFlux
            .filter(order -> order.getTotalAmount().compareTo(BigDecimal.ZERO) > 0)
            .filter(order -> order.getCustomerId() != null)
            .doOnNext(order -> log.info("Valid order: {}", order.getOrderId()))
            .onErrorContinue((error, obj) -> 
                log.error("Validation error for order: {}", obj, error));
    }
    
    // Enrichment function
    @Bean
    public Function<OrderEvent, EnrichedOrderEvent> enrichOrder() {
        return orderEvent -> {
            Customer customer = customerService.getCustomer(orderEvent.getCustomerId());
            List<ProductDetails> products = productService.getProducts(orderEvent.getItems());
            
            return EnrichedOrderEvent.builder()
                .orderEvent(orderEvent)
                .customerDetails(customer)
                .productDetails(products)
                .enrichedAt(Instant.now())
                .build();
        };
    }
}
```

**4. BiFunction (Combining Streams):**
```java
@Configuration
public class OrderCombiningFunctions {
    
    @Bean
    public BiFunction<OrderEvent, CustomerEvent, EnrichedOrderEvent> combineOrderAndCustomer() {
        return (orderEvent, customerEvent) -> {
            log.info("Combining order {} with customer {}", 
                orderEvent.getOrderId(), customerEvent.getCustomerId());
            
            return EnrichedOrderEvent.builder()
                .orderEvent(orderEvent)
                .customerEvent(customerEvent)
                .combinedAt(Instant.now())
                .build();
        };
    }
    
    // Reactive bi-function with window
    @Bean
    public BiFunction<Flux<OrderEvent>, Flux<PaymentEvent>, Flux<OrderPaymentPair>> 
            correlateOrdersAndPayments() {
        return (orderFlux, paymentFlux) -> orderFlux
            .window(Duration.ofSeconds(10))
            .flatMap(orderWindow -> {
                Map<UUID, OrderEvent> orderMap = new ConcurrentHashMap<>();
                return orderWindow
                    .doOnNext(order -> orderMap.put(order.getOrderId(), order))
                    .thenMany(paymentFlux
                        .filter(payment -> orderMap.containsKey(payment.getOrderId()))
                        .map(payment -> new OrderPaymentPair(
                            orderMap.get(payment.getOrderId()), 
                            payment
                        ))
                    );
            });
    }
}
```

**application.yml configuration:**
```yaml
spring:
  cloud:
    function:
      definition: orderEventSupplier;orderEventConsumer;orderToPayment
    stream:
      bindings:
        # Supplier binding
        orderEventSupplier-out-0:
          destination: order-events
          content-type: application/json
        
        # Consumer binding
        orderEventConsumer-in-0:
          destination: order-events
          group: order-service-group
          content-type: application/json
        
        # Function bindings (input and output)
        orderToPayment-in-0:
          destination: order-events
          group: payment-transformer-group
        orderToPayment-out-0:
          destination: payment-events
          content-type: application/json
      
      # Kafka specific
      kafka:
        binder:
          brokers: localhost:9092
```

**Q12: How do you compose multiple functions together?**

**A:** Spring Cloud Functions allows composing functions in several ways:

**1. Pipe composition (chaining):**
```yaml
spring:
  cloud:
    function:
      definition: validateOrder|enrichOrder|processOrder
      # validateOrder -> enrichOrder -> processOrder
```

```java
@Configuration
public class ComposedFunctions {
    
    @Bean
    public Function<OrderEvent, OrderEvent> validateOrder() {
        return order -> {
            if (!isValid(order)) {
                throw new ValidationException("Invalid order");
            }
            log.info("Order validated: {}", order.getOrderId());
            return order;
        };
    }
    
    @Bean
    public Function<OrderEvent, EnrichedOrderEvent> enrichOrder() {
        return order -> {
            log.info("Enriching order: {}", order.getOrderId());
            return EnrichedOrderEvent.from(order);
        };
    }
    
    @Bean
    public Consumer<EnrichedOrderEvent> processOrder() {
        return enrichedOrder -> {
            log.info("Processing enriched order: {}", enrichedOrder.getOrderId());
            saveOrder(enrichedOrder);
        };
    }
}
```

**2. Programmatic composition:**
```java
@Configuration
public class ProgrammaticComposition {
    
    @Bean
    public Function<OrderEvent, ProcessedOrder> orderPipeline() {
        return validateOrder()
            .andThen(enrichOrder())
            .andThen(calculateTotals())
            .andThen(applyDiscounts())
            .andThen(finalizeOrder());
    }
    
    private Function<OrderEvent, OrderEvent> validateOrder() {
        return order -> {
            // validation logic
            return order;
        };
    }
    
    private Function<OrderEvent, EnrichedOrderEvent> enrichOrder() {
        return order -> {
            // enrichment logic
            return EnrichedOrderEvent.from(order);
        };
    }
    
    private Function<EnrichedOrderEvent, OrderWithTotals> calculateTotals() {
        return enriched -> {
            // calculation logic
            return new OrderWithTotals(enriched);
        };
    }
    
    private Function<OrderWithTotals, OrderWithDiscount> applyDiscounts() {
        return withTotals -> {
            // discount logic
            return new OrderWithDiscount(withTotals);
        };
    }
    
    private Function<OrderWithDiscount, ProcessedOrder> finalizeOrder() {
        return withDiscount -> {
            // finalization logic
            return new ProcessedOrder(withDiscount);
        };
    }
}
```

**3. Reactive composition:**
```java
@Configuration
public class ReactiveComposition {
    
    @Bean
    public Function<Flux<OrderEvent>, Flux<ProcessedOrder>> reactiveOrderPipeline() {
        return orderFlux -> orderFlux
            .filter(this::isValid)
            .flatMap(this::enrichWithCustomer)
            .flatMap(this::enrichWithProducts)
            .map(this::calculateTotals)
            .flatMap(this::applyPromotions)
            .map(this::finalizeOrder)
            .onErrorContinue((error, order) -> 
                log.error("Error processing order: {}", order, error));
    }
    
    private boolean isValid(OrderEvent order) {
        return order.getTotalAmount().compareTo(BigDecimal.ZERO) > 0;
    }
    
    private Mono<EnrichedOrderEvent> enrichWithCustomer(OrderEvent order) {
        return customerService.getCustomerAsync(order.getCustomerId())
            .map(customer -> EnrichedOrderEvent.withCustomer(order, customer));
    }
    
    private Mono<EnrichedOrderEvent> enrichWithProducts(EnrichedOrderEvent enriched) {
        return productService.getProductsAsync(enriched.getItems())
            .collectList()
            .map(products -> enriched.withProducts(products));
    }
}
```

**4. Splitting and routing:**
```java
@Configuration
public class RoutingFunctions {
    
    @Bean
    public Function<OrderEvent, Tuple2<HighPriorityOrder, StandardOrder>> routeByPriority() {
        return order -> {
            if (order.getTotalAmount().compareTo(BigDecimal.valueOf(1000)) > 0) {
                return Tuples.of(new HighPriorityOrder(order), null);
            } else {
                return Tuples.of(null, new StandardOrder(order));
            }
        };
    }
}

// Configuration
spring:
  cloud:
    stream:
      bindings:
        routeByPriority-in-0:
          destination: order-events
        routeByPriority-out-0:
          destination: high-priority-orders
        routeByPriority-out-1:
          destination: standard-orders
```

---

### **Questions about Spring Cloud Stream**

**Q13: What is the difference between Spring Cloud Functions and Spring Cloud Stream?**

**A:**

**Spring Cloud Functions:**
- FaaS (Function as a Service) framework
- Pure functional approach
- Portable across different runtimes (AWS Lambda, Azure Functions, etc.)
- Focused on business logic
- No dependency on messaging

**Spring Cloud Stream:**
- Messaging framework
- Abstraction over message brokers (Kafka, RabbitMQ)
- Declarative binding configuration
- Advanced features (partitioning, consumer groups, error handling)
- Integration with Spring Cloud Functions

**Relationship:**
Spring Cloud Stream uses Spring Cloud Functions as its programming model.

```java
// Pure Spring Cloud Function
@Bean
public Function<OrderEvent, PaymentEvent> orderToPayment() {
    return order -> convertToPayment(order);
}

// Spring Cloud Stream binding (application.yml)
spring:
  cloud:
    stream:
      bindings:
        orderToPayment-in-0:
          destination: order-events    # Kafka topic or RabbitMQ exchange
          group: payment-group
        orderToPayment-out-0:
          destination: payment-events
```

**Q14: How do you configure Spring Cloud Stream with Kafka?**

**A:** Full configuration for Kafka:

**application.yml:**
```yaml
spring:
  cloud:
    function:
      definition: orderProcessor;paymentProcessor
    
    stream:
      # Global configuration
      default-binder: kafka
      
      # Bindings
      bindings:
        # Input binding
        orderProcessor-in-0:
          destination: order-events
          group: order-processing-group
          content-type: application/json
          consumer:
            max-attempts: 3
            back-off-initial-interval: 1000
            back-off-multiplier: 2.0
            back-off-max-interval: 10000
        
        # Output binding
        orderProcessor-out-0:
          destination: processed-orders
          content-type: application/json
          producer:
            partition-key-expression: headers['customerId']
            partition-count: 3
      
      # Kafka specific configuration
      kafka:
        binder:
          brokers: localhost:9092
          auto-create-topics: true
          auto-add-partitions: true
          min-partition-count: 3
          replication-factor: 2
          configuration:
            security.protocol: PLAINTEXT
            ssl.endpoint.identification.algorithm:
        
        # Consumer specific
        bindings:
          orderProcessor-in-0:
            consumer:
              auto-commit-offset: false
              start-offset: earliest
              enable-dlq: true
              dlq-name: order-events.DLQ
              auto-rebalance-enabled: true
              configuration:
                max.poll.records: 500
                session.timeout.ms: 30000
                heartbeat.interval.ms: 10000
        
        # Producer specific
        bindings:
          orderProcessor-out-0:
            producer:
              configuration:
                compression.type: snappy
                acks: all
                retries: 3
                max.in.flight.requests.per.connection: 5
                enable.idempotence: true
```

**Java Configuration:**
```java
@Configuration
public class StreamKafkaConfig {
    
    @Bean
    public Function<Message<OrderEvent>, Message<ProcessedOrder>> orderProcessor() {
        return message -> {
            OrderEvent event = message.getPayload();
            Map<String, Object> headers = message.getHeaders();
            
            log.info("Processing order: {}", event.getOrderId());
            
            ProcessedOrder processed = processOrder(event);
            
            // Propagate headers and add new ones
            return MessageBuilder
                .withPayload(processed)
                .copyHeaders(headers)
                .setHeader("customerId", event.getCustomerId())
                .setHeader("processed-at", Instant.now().toString())
                .build();
        };
    }
    
    // Error handler
    @Bean
    public Consumer<ErrorMessage> errorHandler() {
        return errorMessage -> {
            Throwable throwable = errorMessage.getPayload();
            Message<?> originalMessage = errorMessage.getOriginalMessage();
            
            log.error("Error processing message: {}", originalMessage, throwable);
            
            // Custom error handling logic
            alertOperations(throwable, originalMessage);
        };
    }
}
```

**Q15: How do you manage partitioning with Spring Cloud Stream and Kafka?**

**A:** Spring Cloud Stream provides several ways to handle partitioning:

**1. Declarative configuration:**
```yaml
spring:
  cloud:
    stream:
      bindings:
        orderProcessor-out-0:
          destination: order-events
          producer:
            partition-key-expression: headers['customerId']
            partition-count: 6
        
        orderProcessor-in-0:
          destination: order-events
          group: order-processing-group
          consumer:
            partitioned: true
            instance-count: 3  # Total number of instances
            instance-index: 0  # Index of this instance (0, 1, 2)
```

**2. Custom partition key extractor:**
```java
@Configuration
public class PartitioningConfig {
    
    @Bean
    public Function<Message<OrderEvent>, Message<ProcessedOrder>> partitionedOrderProcessor() {
        return message -> {
            OrderEvent event = message.getPayload();
            
            // Custom partitioning logic
            String partitionKey = determinePartitionKey(event);
            
            ProcessedOrder processed = processOrder(event);
            
            return MessageBuilder
                .withPayload(processed)
                .setHeader("partitionKey", partitionKey)
                .build();
        };
    }
    
    private String determinePartitionKey(OrderEvent event) {
        // Business logic to determine the partition
        if (event.getTotalAmount().compareTo(BigDecimal.valueOf(1000)) > 0) {
            return "high-value-" + event.getCustomerId();
        } else {
            return "standard-" + event.getCustomerId();
        }
    }
}
```

**3. Custom partitioner:**
```java
public class CustomOrderPartitioner implements PartitionHandler {
    
    @Override
    public int determinePartition(Message<?> message, int partitionCount) {
        OrderEvent event = (OrderEvent) message.getPayload();
        
        // Custom partitioning algorithm
        if (event.isPriority()) {
            // Priority orders go to partition 0
            return 0;
        } else {
            // Distribute other orders across remaining partitions
            int customerId = event.getCustomerId().hashCode();
            return (Math.abs(customerId) % (partitionCount - 1)) + 1;
        }
    }
}

@Configuration
public class CustomPartitionerConfig {
    
    @Bean
    public PartitionHandler customPartitioner() {
        return new CustomOrderPartitioner();
    }
}
```

**Configuration with custom partitioner:**
```yaml
spring:
  cloud:
    stream:
      bindings:
        orderProcessor-out-0:
          producer:
            partitioned: true
            partition-key-extractor-name: customPartitioner
            partition-count: 6
```

**4. Reactive partitioned processing:**
```java
@Configuration
public class ReactivePartitionedProcessing {
    
    @Bean
    public Function<Flux<Message<OrderEvent>>, Flux<Message<ProcessedOrder>>> 
            partitionedProcessor() {
        return orderFlux -> orderFlux
            .groupBy(message -> extractPartitionKey(message))
            .flatMap(grouped -> grouped
                .map(Message::getPayload)
                .buffer(Duration.ofSeconds(5))
                .flatMap(batch -> processBatch(batch, grouped.key()))
                .map(processed -> MessageBuilder
                    .withPayload(processed)
                    .setHeader("partitionKey", grouped.key())
                    .build())
            );
    }
    
    private String extractPartitionKey(Message<OrderEvent> message) {
        return message.getPayload().getCustomerId().toString();
    }
    
    private Flux<ProcessedOrder> processBatch(List<OrderEvent> batch, String partitionKey) {
        log.info("Processing batch of {} orders for partition key: {}", 
            batch.size(), partitionKey);
        
        return Flux.fromIterable(batch)
            .map(this::processOrder);
    }
}
```

---

### **Questions about Event-Driven Architecture**

**Q16: What is an Event-Driven Microservice and what are its patterns?**

**A:** An Event-Driven Microservice reacts to events rather than direct requests.

**Main patterns:**

**1. Event Notification:**
- A service publishes a minimal event
- Other services react based on their own needs

```java
@Service
public class OrderService {
    private final StreamBridge streamBridge;
    
    public Order createOrder(CreateOrderRequest request) {
        Order order = orderRepository.save(new Order(request));
        
        // Simple notification event
        OrderCreatedEvent event = OrderCreatedEvent.builder()
            .orderId(order.getId())
            .timestamp(Instant.now())
            .build();
        
        streamBridge.send("order-created", event);
        
        return order;
    }
}

// Multiple services react
@Configuration
public class EventHandlers {
    
    @Bean
    public Consumer<OrderCreatedEvent> inventoryHandler() {
        return event -> {
            // Reserve inventory
            inventoryService.reserve(event.getOrderId());
        };
    }
    
    @Bean
    public Consumer<OrderCreatedEvent> notificationHandler() {
        return event -> {
            // Send notification
            notificationService.notify(event.getOrderId());
        };
    }
    
    @Bean
    public Consumer<OrderCreatedEvent> analyticsHandler() {
        return event -> {
            // Update analytics
            analyticsService.trackOrder(event.getOrderId());
        };
    }
}
```

**2. Event-Carried State Transfer:**
- The event contains all the necessary data
- No need to query for details

```java
@Service
public class OrderService {
    
    public Order createOrder(CreateOrderRequest request) {
        Order order = orderRepository.save(new Order(request));
        
        // Event with complete state
        OrderCreatedEvent event = OrderCreatedEvent.builder()
            .orderId(order.getId())
            .customerId(order.getCustomerId())
            .items(order.getItems())
            .totalAmount(order.getTotalAmount())
            .shippingAddress(order.getShippingAddress())
            .timestamp(Instant.now())
            .build();
        
        streamBridge.send("order-created", event);
        
        return order;
    }
}

// Consumer has all data
@Bean
public Consumer<OrderCreatedEvent> paymentProcessor() {
    return event -> {
        // All data available in event - no need to query
        Payment payment = Payment.builder()
            .orderId(event.getOrderId())
            .amount(event.getTotalAmount())
            .customerId(event.getCustomerId())
            .build();
        
        paymentService.processPayment(payment);
    };
}
```

**3. Event Sourcing:**
- State is reconstructed from events
- All changes are events

```java
@Service
public class EventSourcedOrderService {
    private final EventStore eventStore;
    private final StreamBridge streamBridge;
    
    public Order createOrder(CreateOrderCommand command) {
        UUID orderId = UUID.randomUUID();
        
        // Create event
        OrderCreatedEvent event = OrderCreatedEvent.builder()
            .orderId(orderId)
            .customerId(command.getCustomerId())
            .items(command.getItems())
            .version(1L)
            .timestamp(Instant.now())
            .build();
        
        // Store event
        eventStore.save(event);
        
        // Publish event
        streamBridge.send("order-events", event);
        
        // Reconstruct state
        return reconstructOrder(orderId);
    }
    
    public Order updateOrderStatus(UUID orderId, OrderStatus newStatus) {
        Order current = reconstructOrder(orderId);
        
        OrderStatusChangedEvent event = OrderStatusChangedEvent.builder()
            .orderId(orderId)
            .previousStatus(current.getStatus())
            .newStatus(newStatus)
            .version(current.getVersion() + 1)
            .timestamp(Instant.now())
            .build();
        
        eventStore.save(event);
        streamBridge.send("order-events", event);
        
        return reconstructOrder(orderId);
    }
    
    private Order reconstructOrder(UUID orderId) {
        List<DomainEvent> events = eventStore.getEventsForAggregate(orderId);
        
        Order order = new Order();
        events.forEach(event -> order.apply(event));
        
        return order;
    }
}
```

**4. CQRS (Command Query Responsibility Segregation):**
- Separation of write and read models

```java
// Command Side
@Service
public class OrderCommandService {
    private final OrderRepository orderRepository;
    private final StreamBridge streamBridge;
    
    @Transactional
    public UUID createOrder(CreateOrderCommand command) {
        Order order = new Order(command);
        orderRepository.save(order);
        
        OrderCreatedEvent event = OrderCreatedEvent.from(order);
        streamBridge.send("order-events", event);
        
        return order.getId();
    }
}

// Query Side
@Service
public class OrderQueryService {
    private final OrderReadRepository orderReadRepository;
    
    public OrderDTO getOrder(UUID orderId) {
        return orderReadRepository.findById(orderId)
            .map(this::toDTO)
            .orElseThrow();
    }
    
    public List<OrderDTO> getOrdersByCustomer(UUID customerId) {
        return orderReadRepository.findByCustomerId(customerId)
            .stream()
            .map(this::toDTO)
            .collect(Collectors.toList());
    }
    
    // Event handler to update read model
    @Bean
    public Consumer<OrderEvent> orderEventProjection() {
        return event -> {
            OrderReadModel readModel = OrderReadModel.from(event);
            orderReadRepository.save(readModel);
        };
    }
}
```

**Q17: How do you implement the Saga pattern with events?**

**A:** The Saga pattern handles distributed transactions with two approaches:

**1. Choreography-based Saga:**
Each service publishes events and reacts to events from others.

```java
@Service
public class OrderSagaChoreography {
    private final OrderRepository orderRepository;
    private final StreamBridge streamBridge;
    
    // Step 1: Create Order
    public void createOrder(CreateOrderRequest request) {
        Order order = new Order(request);
        order.setStatus(OrderStatus.PENDING);
        orderRepository.save(order);
        
        OrderCreatedEvent event = OrderCreatedEvent.from(order);
        streamBridge.send("order-created", event);
    }
    
    // Step 2: React to Payment Success
    @Bean
    public Consumer<PaymentSuccessEvent> handlePaymentSuccess() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.PAYMENT_COMPLETED);
            orderRepository.save(order);
            
            // Trigger next step
            InventoryReservationRequest request = new InventoryReservationRequest(
                order.getId(),
                order.getItems()
            );
            streamBridge.send("inventory-reservation-request", request);
        };
    }
    
    // Step 3: React to Payment Failure - Compensation
    @Bean
    public Consumer<PaymentFailedEvent> handlePaymentFailed() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.PAYMENT_FAILED);
            order.setFailureReason(event.getReason());
            orderRepository.save(order);
            
            // Compensate - Cancel order
            OrderCancelledEvent cancelEvent = OrderCancelledEvent.builder()
                .orderId(order.getId())
                .reason("Payment failed: " + event.getReason())
                .timestamp(Instant.now())
                .build();
            
            streamBridge.send("order-cancelled", cancelEvent);
            
            log.error("Saga compensation: Order {} cancelled due to payment failure", 
                order.getId());
        };
    }
    
    // Step 4: React to Inventory Reserved
    @Bean
    public Consumer<InventoryReservedEvent> handleInventoryReserved() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.INVENTORY_RESERVED);
            orderRepository.save(order);
            
            // Trigger shipment
            ShipmentRequest shipmentRequest = ShipmentRequest.builder()
                .orderId(order.getId())
                .items(order.getItems())
                .address(order.getShippingAddress())
                .build();
            
            streamBridge.send("shipment-request", shipmentRequest);
        };
    }
    
    // Step 5: React to Inventory Reservation Failed - Compensation
    @Bean
    public Consumer<InventoryReservationFailedEvent> handleInventoryFailed() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.INVENTORY_RESERVATION_FAILED);
            orderRepository.save(order);
            
            // Compensate - Refund payment
            PaymentRefundRequest refundRequest = PaymentRefundRequest.builder()
                .orderId(order.getId())
                .paymentId(order.getPaymentId())
                .amount(order.getTotalAmount())
                .reason("Inventory not available")
                .build();
            
            streamBridge.send("payment-refund-request", refundRequest);
            
            log.error("Saga compensation: Refunding payment for order {}", order.getId());
        };
    }
    
    // Step 6: Saga Success - Shipment Created
    @Bean
    public Consumer<ShipmentCreatedEvent> handleShipmentCreated() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.SHIPPED);
            order.setShipmentId(event.getShipmentId());
            orderRepository.save(order);
            
            // Saga completed
            OrderCompletedEvent completedEvent = OrderCompletedEvent.from(order);
            streamBridge.send("order-completed", completedEvent);
            
            log.info("Saga completed successfully for order {}", order.getId());
        };
    }
    
    // Compensation for shipment failure
    @Bean
    public Consumer<ShipmentFailedEvent> handleShipmentFailed() {
        return event -> {
            Order order = orderRepository.findById(event.getOrderId())
                .orElseThrow();
            
            order.setStatus(OrderStatus.SHIPMENT_FAILED);
            orderRepository.save(order);
            
            // Compensate in reverse order
            // 1. Release inventory
            InventoryReleaseRequest releaseRequest = InventoryReleaseRequest.builder()
                .orderId(order.getId())
                .items(order.getItems())
                .build();
            streamBridge.send("inventory-release-request", releaseRequest);
            
            // 2. Refund payment
            PaymentRefundRequest refundRequest = PaymentRefundRequest.builder()
                .orderId(order.getId())
                .paymentId(order.getPaymentId())
                .amount(order.getTotalAmount())
                .reason("Shipment failed")
                .build();
            streamBridge.send("payment-refund-request", refundRequest);
            
            log.error("Saga compensation: Full rollback for order {}", order.getId());
        };
    }
}
```

**2. Orchestration-based Saga:**
A central orchestrator coordinates the workflow.

```java
@Service
public class OrderSagaOrchestrator {
    private final SagaStateRepository sagaStateRepository;
    private final StreamBridge streamBridge;
    
    public void startSaga(CreateOrderCommand command) {
        // Create saga instance
        SagaState saga = SagaState.builder()
            .sagaId(UUID.randomUUID())
            .orderId(command.getOrderId())
            .currentStep(SagaStep.STARTED)
            .status(SagaStatus.IN_PROGRESS)
            .startedAt(Instant.now())
            .data(Map.of(
                "customerId", command.getCustomerId(),
                "totalAmount", command.getTotalAmount(),
                "items", command.getItems()
            ))
            .build();
        
        sagaStateRepository.save(saga);
        
        // Start first step
        executeNextStep(saga);
    }
    
    private void executeNextStep(SagaState saga) {
        switch (saga.getCurrentStep()) {
            case STARTED:
                processPayment(saga);
                break;
            case PAYMENT_COMPLETED:
                reserveInventory(saga);
                break;
            case INVENTORY_RESERVED:
                createShipment(saga);
                break;
            case SHIPMENT_CREATED:
                completeSaga(saga);
                break;
        }
    }
    
    private void processPayment(SagaState saga) {
        saga.setCurrentStep(SagaStep.PROCESSING_PAYMENT);
        sagaStateRepository.save(saga);
        
        PaymentRequest request = PaymentRequest.builder()
            .orderId(saga.getOrderId())
            .amount((BigDecimal) saga.getData().get("totalAmount"))
            .customerId((UUID) saga.getData().get("customerId"))
            .build();
        
        Message<PaymentRequest> message = MessageBuilder
            .withPayload(request)
            .setHeader("sagaId", saga.getSagaId())
            .setHeader("correlationId", saga.getSagaId().toString())
            .build();
        
        streamBridge.send("payment-request", message);
        
        log.info("Saga {}: Payment processing initiated", saga.getSagaId());
    }
    
    @Bean
    public Consumer<Message<PaymentResponse>> handlePaymentResponse() {
        return message -> {
            PaymentResponse response = message.getPayload();
            UUID sagaId = (UUID) message.getHeaders().get("sagaId");
            
            SagaState saga = sagaStateRepository.findById(sagaId)
                .orElseThrow(() -> new SagaNotFoundException(sagaId));
            
            if (response.isSuccess()) {
                saga.setCurrentStep(SagaStep.PAYMENT_COMPLETED);
                saga.getData().put("paymentId", response.getPaymentId());
                sagaStateRepository.save(saga);
                
                log.info("Saga {}: Payment completed", sagaId);
                executeNextStep(saga);
            } else {
                log.error("Saga {}: Payment failed - {}", sagaId, response.getErrorMessage());
                compensate(saga, SagaStep.PROCESSING_PAYMENT, response.getErrorMessage());
            }
        };
    }
    
    private void reserveInventory(SagaState saga) {
        saga.setCurrentStep(SagaStep.RESERVING_INVENTORY);
        sagaStateRepository.save(saga);
        
        InventoryReservationRequest request = InventoryReservationRequest.builder()
            .orderId(saga.getOrderId())
            .items((List<OrderItem>) saga.getData().get("items"))
            .build();
        
        Message<InventoryReservationRequest> message = MessageBuilder
            .withPayload(request)
            .setHeader("sagaId", saga.getSagaId())
            .build();
        
        streamBridge.send("inventory-reservation-request", message);
        
        log.info("Saga {}: Inventory reservation initiated", saga.getSagaId());
    }
    
    @Bean
    public Consumer<Message<InventoryReservationResponse>> handleInventoryResponse() {
        return message -> {
            InventoryReservationResponse response = message.getPayload();
            UUID sagaId = (UUID) message.getHeaders().get("sagaId");
            
            SagaState saga = sagaStateRepository.findById(sagaId)
                .orElseThrow(() -> new SagaNotFoundException(sagaId));
            
            if (response.isSuccess()) {
                saga.setCurrentStep(SagaStep.INVENTORY_RESERVED);
                saga.getData().put("reservationId", response.getReservationId());
                sagaStateRepository.save(saga);
                
                log.info("Saga {}: Inventory reserved", sagaId);
                executeNextStep(saga);
            } else {
                log.error("Saga {}: Inventory reservation failed", sagaId);
                compensate(saga, SagaStep.RESERVING_INVENTORY, response.getErrorMessage());
            }
        };
    }
    
    private void createShipment(SagaState saga) {
        saga.setCurrentStep(SagaStep.CREATING_SHIPMENT);
        sagaStateRepository.save(saga);
        
        ShipmentRequest request = ShipmentRequest.builder()
            .orderId(saga.getOrderId())
            .items((List<OrderItem>) saga.getData().get("items"))
            .build();
        
        Message<ShipmentRequest> message = MessageBuilder
            .withPayload(request)
            .setHeader("sagaId", saga.getSagaId())
            .build();
        
        streamBridge.send("shipment-request", message);
        
        log.info("Saga {}: Shipment creation initiated", saga.getSagaId());
    }
    
    @Bean
    public Consumer<Message<ShipmentResponse>> handleShipmentResponse() {
        return message -> {
            ShipmentResponse response = message.getPayload();
            UUID sagaId = (UUID) message.getHeaders().get("sagaId");
            
            SagaState saga = sagaStateRepository.findById(sagaId)
                .orElseThrow(() -> new SagaNotFoundException(sagaId));
            
            if (response.isSuccess()) {
                saga.setCurrentStep(SagaStep.SHIPMENT_CREATED);
                saga.getData().put("shipmentId", response.getShipmentId());
                sagaStateRepository.save(saga);
                
                log.info("Saga {}: Shipment created", sagaId);
                executeNextStep(saga);
            } else {
                log.error("Saga {}: Shipment creation failed", sagaId);
                compensate(saga, SagaStep.CREATING_SHIPMENT, response.getErrorMessage());
            }
        };
    }
    
    private void completeSaga(SagaState saga) {
        saga.setStatus(SagaStatus.COMPLETED);
        saga.setCompletedAt(Instant.now());
        sagaStateRepository.save(saga);
        
        OrderCompletedEvent event = OrderCompletedEvent.builder()
            .orderId(saga.getOrderId())
            .sagaId(saga.getSagaId())
            .completedAt(Instant.now())
            .build();
        
        streamBridge.send("order-completed", event);
        
        log.info("Saga {}: Completed successfully", saga.getSagaId());
    }
    
    private void compensate(SagaState saga, SagaStep failedStep, String reason) {
        saga.setStatus(SagaStatus.COMPENSATING);
        saga.getData().put("failureReason", reason);
        sagaStateRepository.save(saga);
        
        log.warn("Saga {}: Starting compensation from step {}", saga.getSagaId(), failedStep);
        
        // Execute compensation in reverse order
        switch (failedStep) {
            case CREATING_SHIPMENT:
                compensateInventory(saga);
                compensatePayment(saga);
                break;
            case RESERVING_INVENTORY:
                compensatePayment(saga);
                break;
            case PROCESSING_PAYMENT:
                // No compensation needed
                break;
        }
        
        saga.setStatus(SagaStatus.COMPENSATED);
        saga.setCompletedAt(Instant.now());
        sagaStateRepository.save(saga);
        
        // Publish failure event
        OrderFailedEvent failureEvent = OrderFailedEvent.builder()
            .orderId(saga.getOrderId())
            .sagaId(saga.getSagaId())
            .failedStep(failedStep)
            .reason(reason)
            .timestamp(Instant.now())
            .build();
        
        streamBridge.send("order-failed", failureEvent);
        
        log.error("Saga {}: Compensation completed", saga.getSagaId());
    }
    
    private void compensateInventory(SagaState saga) {
        if (saga.getData().containsKey("reservationId")) {
            InventoryReleaseRequest request = InventoryReleaseRequest.builder()
                .orderId(saga.getOrderId())
                .reservationId((UUID) saga.getData().get("reservationId"))
                .build();
            
            streamBridge.send("inventory-release-request", request);
            log.info("Saga {}: Inventory compensation sent", saga.getSagaId());
        }
    }
    
    private void compensatePayment(SagaState saga) {
        if (saga.getData().containsKey("paymentId")) {
            PaymentRefundRequest request = PaymentRefundRequest.builder()
                .orderId(saga.getOrderId())
                .paymentId((UUID) saga.getData().get("paymentId"))
                .amount((BigDecimal) saga.getData().get("totalAmount"))
                .reason("Saga compensation")
                .build();
            
            streamBridge.send("payment-refund-request", request);
            log.info("Saga {}: Payment compensation sent", saga.getSagaId());
        }
    }
}

// Saga state entity
@Entity
@Data
@Builder
public class SagaState {
    @Id
    private UUID sagaId;
    private UUID orderId;
    
    @Enumerated(EnumType.STRING)
    private SagaStep currentStep;
    
    @Enumerated(EnumType.STRING)
    private SagaStatus status;
    
    @Convert(converter = JsonConverter.class)
    private Map<String, Object> data;
    
    private Instant startedAt;
    private Instant completedAt;
}

enum SagaStep {
    STARTED,
    PROCESSING_PAYMENT,
    PAYMENT_COMPLETED,
    RESERVING_INVENTORY,
    INVENTORY_RESERVED,
    CREATING_SHIPMENT,
    SHIPMENT_CREATED
}

enum SagaStatus {
    IN_PROGRESS,
    COMPLETED,
    COMPENSATING,
    COMPENSATED,
    FAILED
}
```

**Q18: How do you handle errors and implement retry in an event-driven system?**

**A:** Several error handling and retry strategies:

**1. Retry with exponential backoff:**
```java
@Configuration
public class RetryConfiguration {
    
    @Bean
    public Function<Flux<Message<OrderEvent>>, Flux<Message<ProcessedOrder>>> 
            resilientOrderProcessor() {
        return orderFlux -> orderFlux
            .flatMap(message -> processWithRetry(message)
                .retryWhen(Retry.backoff(3, Duration.ofSeconds(1))
                    .maxBackoff(Duration.ofSeconds(10))
                    .filter(this::isRetryable)
                    .doBeforeRetry(signal -> 
                        log.warn("Retry attempt {} for order", signal.totalRetries())
                    )
                    .onRetryExhaustedThrow((retryBackoffSpec, retrySignal) -> 
                        new MaxRetriesExceededException(
                            "Max retries exceeded", 
                            retrySignal.failure()
                        )
                    )
                )
                .onErrorResume(this::handleUnrecoverableError)
            );
    }
    
    private Mono<Message<ProcessedOrder>> processWithRetry(Message<OrderEvent> message) {
        return Mono.fromCallable(() -> {
            OrderEvent event = message.getPayload();
            ProcessedOrder processed = processOrder(event);
            return MessageBuilder.withPayload(processed).build();
        });
    }
    
    private boolean isRetryable(Throwable throwable) {
        return throwable instanceof TemporaryException 
            || throwable instanceof TimeoutException
            || throwable instanceof ServiceUnavailableException;
    }
    
    private Mono<Message<ProcessedOrder>> handleUnrecoverableError(Throwable throwable) {
        log.error("Unrecoverable error", throwable);
        // Send to DLQ
        return Mono.empty();
    }
}
```

**2. Dead Letter Queue with Spring Cloud Stream:**
```yaml
spring:
  cloud:
    stream:
      kafka:
        bindings:
          orderProcessor-in-0:
            consumer:
              enable-dlq: true
              dlq-name: order-events.DLQ
              dlq-producer-properties:
                configuration:
                  key.serializer: org.apache.kafka.common.serialization.StringSerializer
                  value.serializer: org.springframework.kafka.support.serializer.JsonSerializer
              auto-commit-offset: false
```

```java
@Configuration
public class DLQConfiguration {
    
    @Bean
    public Function<Message<OrderEvent>, Message<ProcessedOrder>> orderProcessor() {
        return message -> {
            try {
                OrderEvent event = message.getPayload();
                ProcessedOrder processed = processOrder(event);
                return MessageBuilder.withPayload(processed).build();
            } catch (Exception e) {
                log.error("Error processing order, sending to DLQ", e);
                throw new RuntimeException(e); // Will be sent to DLQ
            }
        };
    }
    
    // DLQ consumer for manual processing
    @Bean
    public Consumer<Message<OrderEvent>> dlqConsumer() {
        return message -> {
            OrderEvent event = message.getPayload();
            Map<String, Object> headers = message.getHeaders();
            
            log.error("Processing message from DLQ: {}", event.getOrderId());
            log.error("Original error: {}", headers.get("x-exception-message"));
            log.error("Failed at: {}", headers.get("x-exception-stacktrace"));
            
            // Alert operations team
            alertOperations(event, headers);
            
            // Attempt manual recovery
            if (canRecover(event)) {
                republishToMainQueue(event);
            } else {
                persistForManualIntervention(event, headers);
            }
        };
    }
    
    private void republishToMainQueue(OrderEvent event) {
        streamBridge.send("order-events", event);
        log.info("Republished event from DLQ: {}", event.getOrderId());
    }
}
```

**3. Circuit Breaker for event processing:**
```java
@Configuration
public class CircuitBreakerConfig {
    
    @Bean
    public Customizer<Resilience4JCircuitBreakerFactory> defaultCustomizer() {
        return factory -> factory.configureDefault(id -> new Resilience4JConfigBuilder(id)
            .circuitBreakerConfig(CircuitBreakerConfig.custom()
                .slidingWindowSize(10)
                .minimumNumberOfCalls(5)
                .failureRateThreshold(50)
                .waitDurationInOpenState(Duration.ofSeconds(30))
                .permittedNumberOfCallsInHalfOpenState(3)
                .build())
            .timeLimiterConfig(TimeLimiterConfig.custom()
                .timeoutDuration(Duration.ofSeconds(5))
                .build())
            .build());
    }
}

@Service
public class ResilientEventProcessor {
    private final CircuitBreakerFactory circuitBreakerFactory;
    private final ExternalService externalService;
    
    @Bean
    public Function<OrderEvent, ProcessedOrder> circuitBreakerOrderProcessor() {
        return event -> {
            CircuitBreaker circuitBreaker = circuitBreakerFactory.create("orderProcessor");
            
            return circuitBreaker.run(
                () -> processOrder(event),
                throwable -> fallbackProcessing(event, throwable)
            );
        };
    }
    
    private ProcessedOrder processOrder(OrderEvent event) {
        // Call to potentially failing external service
        ExternalData data = externalService.getData(event.getOrderId());
        return new ProcessedOrder(event, data);
    }
    
    private ProcessedOrder fallbackProcessing(OrderEvent event, Throwable throwable) {
        log.warn("Circuit breaker fallback for order: {}", event.getOrderId(), throwable);
        
        // Return partial result or default
        return ProcessedOrder.builder()
            .orderId(event.getOrderId())
            .status(ProcessingStatus.PARTIAL)
            .requiresReprocessing(true)
            .build();
    }
}
```

**4. Idempotent event processing:**
```java
@Service
public class IdempotentEventProcessor {
    private final ProcessedEventRepository processedEventRepository;
    
    @Bean
    public Consumer<Message<OrderEvent>> idempotentOrderConsumer() {
        return message -> {
            OrderEvent event = message.getPayload();
            String eventId = (String) message.getHeaders().get("eventId");
            
            // Check if already processed
            if (processedEventRepository.existsByEventId(eventId)) {
                log.info("Event {} already processed, skipping", eventId);
                return;
            }
            
            try {
                // Process event
                processOrder(event);
                
                // Mark as processed
                ProcessedEvent processedEvent = ProcessedEvent.builder()
                    .eventId(eventId)
                    .orderId(event.getOrderId())
                    .processedAt(Instant.now())
                    .build();
                
                processedEventRepository.save(processedEvent);
                
                log.info("Event {} processed successfully", eventId);
            } catch (Exception e) {
                log.error("Error processing event {}", eventId, e);
                throw e; // Will trigger retry or DLQ
            }
        };
    }
}

@Entity
@Data
@Builder
public class ProcessedEvent {
    @Id
    private String eventId;
    private UUID orderId;
    private Instant processedAt;
    
    @Index
    @Column(nullable = false)
    private Instant createdAt = Instant.now();
}
```

**5. Saga recovery mechanism:**
```java
@Service
public class SagaRecoveryService {
    private final SagaStateRepository sagaStateRepository;
    
    @Scheduled(fixedDelay = 60000) // Every minute
    public void recoverStaleSagas() {
        Instant staleThreshold = Instant.now().minus(Duration.ofMinutes(10));
        
        List<SagaState> staleSagas = sagaStateRepository
            .findByStatusAndStartedAtBefore(SagaStatus.IN_PROGRESS, staleThreshold);
        
        staleSagas.forEach(saga -> {
            log.warn("Recovering stale saga: {}", saga.getSagaId());
            
            try {
                // Attempt to continue from current step
                continueOrCompensate(saga);
            } catch (Exception e) {
                log.error("Failed to recover saga {}", saga.getSagaId(), e);
                markSagaAsFailed(saga);
            }
        });
    }
    
    private void continueOrCompensate(SagaState saga) {
        // Check if we can continue
        if (canContinue(saga)) {
            log.info("Continuing saga {}", saga.getSagaId());
            sagaOrchestrator.executeNextStep(saga);
        } else {
            log.warn("Compensating saga {}", saga.getSagaId());
            sagaOrchestrator.compensate(saga, saga.getCurrentStep(), "Recovery timeout");
        }
    }
    
    private boolean canContinue(SagaState saga) {
        // Business logic to determine if continuation is possible
        return saga.getCurrentStep() != SagaStep.STARTED 
            && saga.getData().containsKey("paymentId");
    }
}
```

---

This complete expertise in asynchronous communication covers all the essential aspects: the technologies (RabbitMQ, Kafka), the frameworks (Spring Cloud Functions, Stream), the architectural patterns (Event-Driven, Saga, CQRS, Event Sourcing), and advanced error handling and resilience. This allows you to build distributed systems that are robust, scalable, and maintainable.
