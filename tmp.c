public class LocalDateTimeUtcSerializer extends StdSerializer<LocalDateTime> {
    public LocalDateTimeUtcSerializer() {
        super(LocalDateTime.class);
    }

    @Override
    public void serialize(LocalDateTime value, JsonGenerator gen, SerializerProvider provider) throws IOException {
        gen.writeString(value.atOffset(ZoneOffset.UTC).format(DateTimeFormatter.ISO_OFFSET_DATE_TIME));
    }
}

====================================================
    public class MyDto {
    @JsonSerialize(using = LocalDateTimeUtcSerializer.class)
    private LocalDateTime timestamp;
}
=========================================================
    public class LocalDateTimeToUtcSerializer extends StdSerializer<LocalDateTime> {
    private static final ZoneId PARIS_ZONE = ZoneId.of("Europe/Paris");

    public LocalDateTimeToUtcSerializer() {
        super(LocalDateTime.class);
    }

    @Override
    public void serialize(LocalDateTime value, JsonGenerator gen, SerializerProvider provider) 
            throws IOException {
        // Step 1: Treat the LocalDateTime as Paris time
        ZonedDateTime parisTime = value.atZone(PARIS_ZONE);
        
        // Step 2: Convert to UTC
        Instant utcInstant = parisTime.toInstant();
        
        // Step 3: Format with 'Z'
        gen.writeString(utcInstant.toString()); // Outputs "15:30:00Z"
    }
}
